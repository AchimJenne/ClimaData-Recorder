/**************************************************/
/*! @brief Arduino project for Raspberry pico
 *  @details weather mesurement enviroment, based on 
 *  @details Raspberry Pico with DS3231 RTC, BME680 and 
 *            Waveshare SPI 3.5inch LCD, SD-Card
 *   
 *  @defgroup Pico_Clima_Measure
*/
#include <arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>

#include "I2C_RTC.h"
#include "LCD_Driver.h"
#include "DEV_Config.h"
#include "pico/stdlib.h"

#include <SPI.h>
#include <SD.h>
#include <FatFS.h>
#include <SdFat.h>
#include "Pico_Dev.h"
#include "LCD_GUI.h"
#include "LCD_Touch.h"
#include "BME68x.h"
/**************************************************/
/**
*/
#define ILINE   80
#define SECTICK 60
#define METTICK 100

#ifdef BOARD_NAME1
#define CPUTEMP 1.75f
#else
#define CPUTEMP 1.0f
#endif
// #define DEBUG
/* Clock  grafic center-position and Clock face param. */
uint16_t iXOfs = 375, iYOfs = 150;
uint16_t iClkH = 43, iClkM = 68, iClkS = 72;
float   sinTab[SECTICK], cosTab[SECTICK];
float   sinMet[METTICK], cosMet[METTICK];

bool    bSDCRD, bSerOn = false;
time_t  tiUxOld, tiUx;
struct  tm mytm; 
struct  timeval tiV;
volatile bool bAuto = false;
bool    bALog = false;
bool    bRTC  = false;
bool    bEEP  = false;
static  DS3231 RTC;
int     day, mon, year;
int     hour, minute, second;
int     iMDayOld;
File    myFile;
char    sLogFn[40]= "logfile.txt";
char    sPath[ILINE]={"/"};
int16_t iPgCnt = 0, iPgCntOld = 0;
int32_t iOfAlt= 480, iOfTem, iOfHum, iOfPre;
int32_t iTemp=0, iHum=0, iPres=0, iGas=0, iAlt=0;
int32_t iPresOld= iPres;
int32_t iTeAv, iHuAv, iPrAv;
int32_t iQNH;
int32_t iCorPr= 0;
float   fCPUTemp = 15.0;

// const float   fMolGasC= 8.31446261815324; // J/(mol*K)
// const float   fMolMass= 0.0289644;  // kg/mol
// const float   fRS = (fMolGasC / fMolMass);  // gas const. dry air 287.058 J/(kg*K)
// const float   fRD = 461.523;                // gas const. water vapor in J/(kg*K)
float   fDens = 0.0f;

bool    bRecData= false;
tRecData myData[100];

bool    bBME680  = false;
BME680_Class MySens;
int iUSys = 3300;
// Touchpad parameter
POINT x_0, x_1, x_2, x_3, x_4;
POINT y_0, y_1, y_2, y_3, y_4;
POINT hwFac = 5;
/**************************************************/
/*! @brief Arduino setup function
*/
void setup()
{
  static char sLine[ILINE]; 
  char *psLine = &sLine[0];
  int   err = 0;
  uint32_t iTi;

  pinMode(PIN_LED, OUTPUT);
  pinMode(LCD_BKL_PIN, OUTPUT);
  pinMode(TP_CS_PIN, OUTPUT);
  pinMode(TP_IRQ_PIN, INPUT);
  pinMode( A3, INPUT);
  pinMode( D24, INPUT);
  pinMode( D23, OUTPUT);
  gpio_put( D23, 0);

  Serial.begin(115200);
  // wait on Serial to be available
  if(digitalRead(D24)) {
    iTi = millis()+ 20000;
    while ((!Serial) && (millis() < iTi)){
    } 
  }
  delay(10);
  if (millis() < iTi) bSerOn= true;


#ifdef DEBUG  
  Serial1.begin(115200);
  Serial1.setPinout(PIN_SERIAL1_TX, PIN_SERIAL1_RX);
  delay(100);
  Serial1.print("\e[0H\e[2J Debug port");
  Serial1.print("\n>");
#endif

  for (int i=0; i<SECTICK; i++) {
    sinTab[i]= sin((i*6.0f - 90.0f) * DEG_TO_RAD);
    cosTab[i]= cos((i*6.0f - 90.0f) * DEG_TO_RAD);
  }
  for (int i=0; i<METTICK; i++) {
    sinMet[i]= sin((i*3.6f - 90.0f) * DEG_TO_RAD);
    cosMet[i]= cos((i*3.6f - 90.0f) * DEG_TO_RAD);
  }
  
  Serial.print(F("\e[0H\e[2J"));
  Serial.println(BOARD_NAME);
  Serial.print(F("CPU- Frequency   : "));
  Serial.print(F_CPU/1000000);
  Serial.println(F(" MHz"));
  Serial.print(F("Flash- Memory    : "));
  Serial.print(PICO_FLASH_SIZE_BYTES);
  Serial.println(F(" Bytes"));
  
  Serial.print(F("Pico SDK Version: "));
  Serial.println(PICO_SDK_VERSION_STRING);
  Serial.print(F("Arduino Version : "));
  Serial.println(ARDUINO_PICO_VERSION_STR);
  
	Serial.print("SD_Init...");
  SDSPI.setMISO(PIN_MISO);
  SDSPI.setMOSI(PIN_MOSI);
  SDSPI.setSCK(PIN_SCK);
  SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
  
  gpio_put(SD_CS_PIN,  1);
  gpio_put(TP_CS_PIN , 1);
  gpio_put(LCD_CS_PIN, 1);
  gpio_put(LCD_DC_PIN, 1);

  analogWriteResolution(10);
  analogWrite(LCD_BKL_PIN, LCDBRIGHT-1);

  bool bInitSD = false;
  bInitSD = SD.begin( SDCRD);
  if (bInitSD) {
    Serial.println(F("OK"));
    bSDCRD= true;
  } else {
    Serial.println(F("failed"));
    bSDCRD= false;
  }
  SD.end();

  //*** external RTC Modul ***
  Wire.setClock(400000);
  if (RTC.begin()) {
    bRTC = true;
    if (!RTC.isRunning()) {
      Serial.println(F("RTC starting"));
      RTC.startClock();
    } 
    if (RTC.getHourMode() != CLOCK_H24) {
      Serial.println(F("RTC set to 24h Format"));
      RTC.setHourMode(CLOCK_H24);
    }
    /* load internal RTC from external RTC */
    if (bRTC) {
      mytm.tm_year = RTC.getYear() -1900;
      mytm.tm_mon  = RTC.getMonth() -1;
      mytm.tm_mday = RTC.getDay(); 
      mytm.tm_hour = RTC.getHours();
      mytm.tm_min  = RTC.getMinutes();
      mytm.tm_sec  = RTC.getSeconds();
      tiV.tv_sec   = mktime(&mytm);
      tiV.tv_usec  = 0;
      settimeofday(&tiV, nullptr);
      iMDayOld = mytm.tm_mday;
  
      Serial.print(F("RTC- Temperature:  "));
      Serial.println(RTC.getTemp());
      Serial.print(F("RTC- Date&Time:    "));
      sprintf(sLine, "%2.2d.%2.2d.%4.4d %2.2d:%2.2d:%2.2d",
                  RTC.getDay(),   RTC.getMonth(),   RTC.getYear(), 
                  RTC.getHours(), RTC.getMinutes(), RTC.getSeconds()); 
      Serial.println(sLine); 
      
      Wire.begin();
      Serial.print("EEPROM at [");
      Serial.print(adrEEP, HEX);
      Serial.print("]: ");
      Wire.beginTransmission(adrEEP);
      int result = Wire.endTransmission();
      if (result==0) {
        Serial.print(F("  "));
        bALog = readEEP(0x0000);
        iOfAlt= readEEPi16(0x0001 );
        bEEP = true;
      } else {
        Serial.print(F("not"));
        bEEP = false;
      } /* end if */
      Serial.println(F(" found"));
    }
  } else {
    // set internal RTC to SW build date and time
    Serial.println("Internal RTC only");
    char sMon[5];
    sscanf( __DATE__, "%3s %2d %4d", &sMon, &day, &year);
    mytm.tm_year = year -1900;
    mytm.tm_mon  = func_MonParser(&sMon[0]) -1;
    mytm.tm_mday = day;
    sscanf( __TIME__, "%02d:%02d:%02d", &hour, &minute, &second);
    mytm.tm_hour = hour;
    mytm.tm_min  = minute;
    mytm.tm_sec  = second; 
    tiV.tv_sec   = mktime(&mytm);
    tiV.tv_usec  = 0;
    settimeofday(&tiV, nullptr);    
  }
  time(&tiUx); 
  Serial.print(F("Pico internal RTC: "));
  strftime(sLine, sizeof(sLine), "%0d.%0m.20%0y %0H:%0M:%0S", localtime(&tiUx));
  Serial.println(sLine);

  Wire.beginTransmission(BMEADR);
  err = Wire.endTransmission();
  Serial.print("BME-680 : ");
  if (err == 0) {
    bBME680 = true;
    Serial.println("OK");
    MySens.begin(400000, BMEADR);
    MySens.setOversampling(TemperatureSensor, Oversample16);  // Use enumerated type values
    MySens.setOversampling(HumiditySensor, Oversample16);     // Use enumerated type values
    MySens.setOversampling(PressureSensor, Oversample4);      // Use enumerated type values
    MySens.setIIRFilter(IIR8);                                // Use enumerated type values
    MySens.setGas( 0, 0);                                     // (320C for 150 milliseconds) inactiv heating!
    MySens.getSensorData(iTemp, iHum, iPres, iGas, true);
    delay(10);
    MySens.getSensorData(iTemp, iHum, iPres, iGas, true);    
    iPresOld= iPres;
    iCorPr = fnCurFast_I32((int32_t)iOfAlt, &lAltPres[0]) ;
    iOfPre = iPres;
  } else {
    Serial.println("--");
    bBME680 = false;
  } // end if 
  
  /* LCD- init and setup */
  LCD_SCAN_DIR  lcd_scan_dir = Landscape;
  initGUIClock();   // LCD-Init moved into initGUIClock
  
  /* LCD-Touchpad setup and testing */
  TP_Init(lcd_scan_dir);
  // TP_Dialog(lcd_scan_dir);
  // TP_DrawBoard(lcd_scan_dir);
  // TP_ShowInfo(x_0, y_0, x_1, y_1, x_2, y_2, x_3, y_3, hwFac);
  // TP_Read_ADC_XY(&x_0, &y_0);
  // Serial.print(">");
  // Serial.print(x_0);
  // Serial.print(" ");
  // Serial.println(y_0);

  analogReadResolution(12);
  Serial.print(F("CPU- Temperature:  "));
  fCPUTemp = analogReadTemp(3.3f);
  Serial.println(fCPUTemp);
  gpio_put( D23, 1);
  Serial.print(F("Board Voltage: "));
  iUSys = ((analogRead(A3) + analogRead(A3))*1000)/824;
  gpio_put( D23, 0);
  sprintf(sLine, "%2d.%02d V", (iUSys/1000), (iUSys%1000)/10);
  Serial.println(sLine);

  Serial.print(sPath);
  Serial.print(F(">"));  
}

/**************************************************/
/**************************************************/
void loop() {
  bool bEM;
  static bool bTiUx;
  char inChar;
  static char sLine[ILINE]; 
  char sTemp[ILINE];
  
  char *psLine= &sLine[0];
  static int16_t iMinOld, iMonOld;
  static int16_t iTHPCn= 0;
  static int32_t iTeSu= 0, iHuSu= 0, iPrSu= 0;

  // put your main code here, to run repeatedly:
  if (Serial.available()) { 
    inChar = (char) Serial.read();
    /************************************************************/
    if ((!bSerOn)&&(inChar !=0)) {
      Serial.println();
      bSerOn= true;
      delay(100);
      Serial.print(F("\r\n\e[0H\e[2J")); 
      *psLine= 0;
      Serial.print(sPath);
      Serial.print(F(">")); 
    } else
    if (bEM= editLine(psLine, inChar)) {
      psLine = strupr(psLine);
      int iRet= fnSDOS_Parser(psLine);
      spi_init(SPI_PORT, SPI_SPD);
      gpio_set_function(LCD_CLK_PIN,  GPIO_FUNC_SPI);
	    gpio_set_function(LCD_MOSI_PIN, GPIO_FUNC_SPI);
	    gpio_set_function(LCD_MISO_PIN, GPIO_FUNC_SPI);
      *psLine= 0;
      Serial.print(F("\r\n"));
      Serial.print(sPath);
      Serial.print(F(">"));
    } /* end if */
    /************************************************************/
  } else {
    // gpio_put( D23, 1);   // reduce ripple
    time(&tiUx);
    fCPUTemp = (analogReadTemp(3.3f) + analogReadTemp(3.3f)) / 2.0;
    iUSys = ((analogRead(A3) + analogRead(A3))*1000)/824;
    // gpio_put( D23, 0);  // higher efficiency

    if (tiUx != tiUxOld) {    // once second interval
      tiUxOld = tiUx;
      mytm = *localtime(&tiUx);

      if ((bRTC)&&(mytm.tm_mday == iMDayOld)) {
        Wire.setClock(400000);
        mytm.tm_year = RTC.getYear() -1900;
        mytm.tm_mon  = RTC.getMonth() -1;
        mytm.tm_mday = RTC.getDay();
        mytm.tm_hour = RTC.getHours(); 
        mytm.tm_min  = RTC.getMinutes();
        mytm.tm_sec  = RTC.getSeconds();
        tiV.tv_sec   = mktime(&mytm);
        tiV.tv_usec  = 0;
        settimeofday(&tiV, nullptr);
        iMDayOld     = mytm.tm_mday;
      }
      
      gpio_put(SD_CS_PIN,  1);
      gpio_put(LCD_CS_PIN, 1);
      gpio_put(LCD_DC_PIN, 1);
      
      // test LCD- touch IRQ- Signal
      if (!digitalRead(TP_IRQ_PIN)) {
        TP_Read_ADC_XY(&x_0, &y_0);   // get the touchpointer
        if ((x_0>250) && (x_0<4095) && (y_0>2100) && (y_0<3980)) {
          iPgCnt++;
          iPgCnt= (iPgCnt % MAXLCDPAGE);
        } else
        if ((x_0>250) && (x_0<4095) && (y_0>250) && (y_0<=2100)) {
          if (iPgCnt > 0) {
            iPgCnt--;
          } else {
            iPgCnt = MAXLCDPAGE;
          }
        }
      }

      // BME680 only 
      if (bBME680) {
        Wire.setClock(800000);
        Wire.beginTransmission(BMEADR);
        MySens.getSensorData(iTemp, iHum, iPres, iGas, false);
        Wire.endTransmission(); 
        iQNH  = iPres + iCorPr;     
        if (bAuto) {
          strftime(sTemp, sizeof(sTemp), "%0d.%0m.20%0y (%a)  %0H:%0M:%0S  ", localtime(&tiUx));
          Serial.print(sTemp);
          sprintf(sTemp, "Tem:%3d.%01d C ", (iTemp/100), (iTemp%100)/10);
          Serial.print(sTemp);
          sprintf(sTemp, "Pre:%4d.%01d hPa ", (iPres/100), (iPres%100)/10);
          Serial.print(sTemp);
          sprintf(sTemp, "QNH:%4d.%01d hPa ", (iQNH/100), (iQNH%100)/10);
          Serial.print(sTemp);
          sprintf(sTemp, "Hum:%3d.%01d %%", (iHum/1000), (iHum % 1000)/100);
          Serial.println(sTemp);
          Serial.print(sPath);
          Serial.print(">");
        }
        // Logfile option is active
        if (bALog) {
          iTeSu = iTeSu + iTemp;
          iHuSu = iHuSu + iHum;
          iPrSu = iPrSu + iPres;
          iTHPCn++;
          if (iMinOld != mytm.tm_min) { // every minute building the full dataset
            year  = mytm.tm_year + 1900;
            mon   = mytm.tm_mon + 1;
            day   = mytm.tm_mday;
            hour  = mytm.tm_hour;
            iMinOld= minute= mytm.tm_min;
            second= mytm.tm_sec;
                     
            iTeAv  = iTeSu / iTHPCn;
            iHuAv  = iHuSu / iTHPCn;
            iPrAv  = iPrSu / iTHPCn;
            iTeSu  = iHuSu = iPrSu= 0;
            iTHPCn = 0;
            iQNH   = iPrAv + iCorPr;      
            if (iMonOld != mon) {
              iMonOld = mon;
              defLogFn(mon, &sLogFn[0]);
            } /* end if */
            sprintf(sTemp, 
              "%2.02d-%2.02d-%4.4d\t%2.2d:%2.02d:%2.02d\tTem:\t%3d.%02d\t°C\tHum:\t%3d.%01d\t%%\tPre:\t%4d.%02d\thPa\tQNH:\t%4d.%02d\thPa",
              day, mon, year, hour, minute, second,
              (iTeAv / 100), (iTeAv % 100), (iHuAv/1000), (iHuAv % 1000)/100, (iPrAv/100), (iPrAv%100), (iQNH/100), (iQNH%100));
            if (bSDCRD= SD.begin(SDCRD)) {
              digitalWrite(PIN_LED, 1);
              SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
              File FH1 = SD.open(sLogFn, FILE_WRITE);
              if (FH1) {
                FH1.println(sTemp);
                FH1.close();
              } else {
                Serial.print(F("Can\'t open File ..."));
                Serial.print(sLogFn);
              } /* end if */
              SD.end();
              digitalWrite(PIN_LED, 0);
            } /* end if */
            spi_init(SPI_PORT, SPI_SPD);
            gpio_set_function(LCD_CLK_PIN,  GPIO_FUNC_SPI);
	          gpio_set_function(LCD_MOSI_PIN, GPIO_FUNC_SPI);
	          gpio_set_function(LCD_MISO_PIN, GPIO_FUNC_SPI);
          }/* end if */
        } /* end if */
      } /* end if */

      // initialize active page
      if (iPgCnt != iPgCntOld){
        switch (iPgCnt) {
          case 0:
            initGUIClock();
          break;
          case 1:
            LCD_Clear(BLACK);
            initPg01();
            break;
          case 2:
            LCD_Clear(BLACK);
            initAltClock();
            break;
          case 3:
            LCD_Clear(BLACK);
            GUI_DisString( 50, 26, "Variometer     Altitude", &Font24, BLACK, WHITE);
            initVario(125, 150);
            initAltMeter(365, 150);   // Altimeter
          break;
          case 4:
            LCD_Clear(BLACK);
            initAltTemp();
          break;
          default:{
          }
        } /* end switch */
        iPgCntOld = iPgCnt;
      } /* end if */
      // shows the activ screen
      switch (iPgCnt) {
        case 0:
          showClock(iXOfs, iYOfs);
          showTextData();
          break;
        case 1:
          showPg01();
          break;
        case 2:
          showAltClock();
          break;
        case 3:
          showVarioAlt();
        break;
        case 4:
          showAltTemp();
        break;
        default:{
        }        
      } /* end switch */      
    } /* end if */
  } /* end if */
} /* end main loop */