#include "Pico_Dev.h"

/**************************************************/
/*! \brief fnc_ALT                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_ALT(const char* szCmdLn)
{
  /* place your code here */
  //int iResult;
  if (strlen(szCmdLn)>=2){
      iOfAlt= atoi(szCmdLn);
      Serial.print(" (m)");
      iCorPr = fnCurFast_I32((int32_t)iOfAlt, &lAltPres[0]);  
      writeEEPui16(0x0001, iOfAlt );
  } else {
    Serial.print(" ");
    Serial.print(iOfAlt);
    Serial.print(" (m) (");
    Serial.print(iCorPr/100);
    Serial.print(" hPa)");
  }  
  return( eALT );
}  /* end of fnc_ALT */
 
/**************************************************/
/*! \brief fnc_AUTOLOG                              
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_AUTOLOG(const char* szCmdLn)
{
  /* place your code here */
    if (bALog) {
      Serial.print(F(": off"));
      bALog= false;
    } else {
      Serial.print(F(": on"));
      bALog = true;
    } /* end if */
    writeEEP(0x0000, (uint8_t*) bALog, 1);  
  return( eAUTOLOG );
}  /* end of fnc_AUTOLOG */

/**************************************************/
/*! \brief fnc_AUTO                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_AUTO(const char* szCmdLn)
{
    /* place your code here */
    if (bAuto) {
      // tickHd100.detach();
      Serial.print(F(": off"));
      bAuto= false;
    } else {
      // tickHd100.attach_ms(100, task_ti100ms, (int) 1);
      Serial.print(F(": on"));
      bAuto = true;
    } /* end if */
    return( eAUTO );
}  /* end of fnc_AUTO */


/**************************************************/
/*! \brief fnc_CD                                   
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_CD(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE];
  argPathFn( szCmdLn, &sLine[0]);
  if (bSDCRD= SD.begin( SDCRD)) {
    digitalWrite(PIN_LED, 1);
    File dir = SD.open(sLine);
    if (dir) {
      strcpy(sPath, sLine);
    } else {
      Serial.println();
      Serial.print(sLine);
      Serial.print(F(" not found"));
    } /* end if */
    SD.end();
    digitalWrite(PIN_LED, 0);
  } /* end if */
  return( eCD );
}  /* end of fnc_CD */
 
/**************************************************/
/*! \brief fnc_CLS                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_CLS(const char* szCmdLn)
{
   /* place your code here */
   Serial.print(F("\e[0H\e[2J"));
   return( eCLS );
}  /* end of fnc_CLS */
 
/**************************************************/
/*! \brief fnc_COPY                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_COPY(const char* szCmdLn)
{
  /* place your code here */
  File FH1, FH2;
  char sFnTo[ILINE];
  char *psFnTo= &sFnTo[0];
  char sFnFrom[ILINE];
  char s1[20], s2[20];
  #define IBUFFER 1024
  char cBuff[IBUFFER];

  szCmdLn++;
  int iRet= sscanf(szCmdLn,"%s %s", &s1, &s2);
  if (iRet ==2) {
    argPathFn( &s1[0], &sFnFrom[0]);
    if (strncmp(s2, ".", 1) ==0) {
      strncpy(psFnTo, sPath, ILINE-1);
      strncat(psFnTo, "/", ILINE);
      strncat(psFnTo, (strrchr(s1, '/')+1), ILINE-1);
      Serial.println();  
      Serial.println(psFnTo);
    } else {
      argPathFn( &s2[0], &sFnTo[0]);
    } /* end if */
    if (bSDCRD= SD.begin( SDCRD)) {
      digitalWrite(PIN_LED, 1);
      SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
      FH1 = SD.open(sFnFrom, FILE_READ);
      if (FH1) {
        FH2 = SD.open(sFnTo, FILE_WRITE);
        if (FH2) {
          while (FH1.available() > 0) {
            size_t iLen = FH1.readBytes(cBuff, IBUFFER);
            FH2.write(cBuff, iLen);
          }  /* end while */
          Serial.print(F(" OK"));
        } else {
          Serial.print(F(" no Dest."));
        } /* end if */
        FH2.close();
        FH1.close();
      } else {
        Serial.print(sFnFrom);
        Serial.print(F(" no Source"));
      } /* end if */
      digitalWrite(PIN_LED, 0);
    } /* end if */
    SD.end();
  } /* end if */
  return( eCOPY );
}  /* end of fnc_COPY */
 
/**************************************************/
/*! \brief fnc_CONFIG                               
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_CONFIG(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE];
  Serial.println(F(": "));
  Serial.println(BOARD_NAME);
  Serial.print(F("CPU- Frequency: "));
  Serial.print(F_CPU/1000000);
  Serial.println(F(" MHz"));
  sprintf(sLine, "CPU- Temper.  : %5.1f C", fCPUTemp);
  Serial.println(sLine);
  Serial.print(F("Board Voltage : "));
  sprintf(sLine, "%2d.%02d V", (iUSys/1000), (iUSys%1000)/10);
  Serial.println(sLine);

  Serial.print(F("SD-Interface  : "));  
  if (!(bSDCRD= SD.begin( SDCRD))) {
     Serial.println(F("failed"));
  } else {
    digitalWrite(PIN_LED, 1);
    Serial.println(F("OK"));
#if defined MAKERGPIO || defined WS147GPIO
    Serial.print(F("GPIO :  "));
    Serial.print(RP_CLK_GPIO);
    Serial.print(F(", "));
    Serial.print(RP_CMD_GPIO);
    Serial.print(F(", "));
    Serial.println(RP_DAT0_GPIO);
#else
    Serial.print(F("SPI  : "));
    Serial.print(SDSPD);
    Serial.println(F(" Mhz"));
    Serial.print(F("MISO : "));
    Serial.println(PIN_MISO);
    Serial.print(F("MOSI : "));
    Serial.println(PIN_MOSI);
    Serial.print(F("SCK  : "));
    Serial.println(PIN_SCK);
    Serial.print(F("CS   : "));
    Serial.println(PIN_SS);
#endif    
    Serial.print(F("SD-Card type : "));
    switch (SD.type()) {
      case 0:
        Serial.println(F("SD"));
        break;
      case 1:
        Serial.println(F("SD1"));
        break;
      case 2:
        Serial.println(F("SD2"));
        break;
      case 3:
        Serial.println(F("SDHC/SDXC"));
        break;
      default:
      Serial.println(F("Unknown"));
    }  /* end switch */
    Serial.print("Cluster  : ");
    Serial.print(SD.totalClusters());
    Serial.print(" * ");
    Serial.print(SD.clusterSize());
    Serial.println(" =");
    Serial.print("SD-Card Size : ");
    Serial.print(SD.size64());
    Serial.println();
  } /* end if */
  SD.end(); 
  if (bRTC){
    Wire.begin();
    int i2c = adrEEP;
    Serial.print("EEPROM at [");
    Serial.print(i2c, HEX);
    Serial.print(F("H]:  "));
    Wire.beginTransmission(i2c);
    int result = Wire.endTransmission();
    if (result==0) {
      Serial.println(F("found"));
      bEEP = true;
    } else {
      Serial.println(F("missing"));
      bEEP = false;
    }
  } /* end if */
  
  Wire.begin();
  int i2c = BMEADR;
  Serial.print("BME680 at [");
  Serial.print(i2c, HEX);
  Serial.print(F("H]:  "));
  Wire.beginTransmission(i2c);
  int result = Wire.endTransmission();
  if (result==0) {
      Serial.println(F("found"));
      bBME680 = true;
  } else {
      Serial.println(F("missing"));
      bBME680 = false;
  } /* end if */  
  Serial.print(F("Free RAM : ")); 
  Serial.print(freeMemory());  
  Serial.println(" Byte");
  digitalWrite(PIN_LED, 0);
  return( eCONFIG );
}  /* end of fnc_CONFIG */

/**************************************************/
/*! \brief fnc_MEM                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_MEM(const char* szCmdLn)
{
  /* place your code here */
  digitalWrite(PIN_LED, 1);
  Serial.print(F("\rFree RAM : ")); 
  Serial.print(freeMemory());  
  Serial.println(" Byte");
  digitalWrite(PIN_LED, 0);  
  return( eMEM );
} /* end of fnc_MEM */
     
/**************************************************/
/*! \brief fnc_DATE                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_DATE(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE];
  if (strlen(szCmdLn) >= 8){
    int16_t iResult= sscanf( szCmdLn, "%02d.%02d.%04d", &day, &mon, &year);   
    if (iResult == 3) {
      mytm.tm_year = year -1900;
      mytm.tm_mon  = mon -1;
      mytm.tm_mday = day;     
      if (bRTC) {
        RTC.setDate((uint8_t) day, (uint8_t) mon, (uint16_t) year);
      } /* end if */
      tiV.tv_sec = mktime(&mytm);
      tiV.tv_usec = 0;
      settimeofday(&tiV, nullptr);
    } /* end if */
  } else {
    Serial.print(F(" : "));
    if (bRTC){
      Serial.print(F("RTC: "));
      sprintf(sLine,"%2.2d.%2.2d.%4.4d ", RTC.getDay(), RTC.getMonth(), RTC.getYear());
      Serial.print(sLine);
    } /* end if */
    Serial.print(F("CPU: "));
    time(&tiUx);
    strftime(sLine, sizeof(sLine), "(%A) %0d.%0m.20%0y", localtime(&tiUx));
    Serial.print(sLine);
  } /* end if */
  return( eDATE );
}  /* end of fnc_DATE */
 
/**************************************************/
/*! \brief fnc_DEL                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_DEL(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE];
  argPathFn( szCmdLn, &sLine[0]);
  Serial.print(F(" : "));
  if (bSDCRD= SD.begin(SDCRD)) {
    if (SD.exists(sLine)) {
      digitalWrite(PIN_LED, 1);
      int16_t iRes= SD.remove(sLine);
      if (iRes>0) {
        Serial.print(F("removed"));
      } else {
        Serial.print(F("not removed"));
      } /* end if */
    } else {
      Serial.print(F("not Found"));
    } /* end if */
    SD.end();
    digitalWrite(PIN_LED, 0);
  } /* end if */   
  return( eDEL );
}  /* end of fnc_DEL */
 
/**************************************************/
/*! \brief fnc_DIR                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
void printDirectory(File dir, int numTabs) {
  static File entry;
  while ( entry =  dir.openNextFile()) {
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print("\e[");
      Serial.print(i*2);
      Serial.print("C");
    } /* end for */
    int iLen = strlen(entry.name());
    Serial.print(entry.name());
    if (entry.isDirectory()) {  // files have sizes, directories do not
      Serial.print(F("\\\e["));
      if (iLen <= 27) {
        Serial.print(28);
      } else {
        Serial.print(iLen + 2);
      } /* end if */
      Serial.println(F("G\e[1m<Dir>\e[0m"));
    } else
    if (entry.isFile()) {
      
      if (iLen <= 23) {
        Serial.print(F("\e[24G"));
      } else {
        // Serial.print(F("\e["));
        // Serial.print(iLen + 2);
        // Serial.print(F("G"));
        Serial.print(F("  "));
      } /* end if */
      Serial.print(entry.size(), DEC);
      
      time_t cr = entry.getCreationTime();
      char sLine[28];
      strftime(sLine,sizeof(sLine),"%0d.%0m.20%0y %0H:%0M:%0S",localtime(&cr));
      Serial.print("\t");
      Serial.println(sLine);
    } /* end if */
    entry.close();
  } /* end while */
}

int fnc_DIR(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE];
  argPathFn( szCmdLn, &sLine[0]);
  Serial.println(F(" : "));
  if (bSDCRD= SD.begin(SDCRD)) {
    digitalWrite(PIN_LED, 1);
    SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
    File dir = SD.open(sLine);
    printDirectory(dir, 0);
    SD.end();
    digitalWrite(PIN_LED, 0);
  } /* end if */
   return( eDIR );
} /* end of fnc_DIR */
 
/**************************************************/
/*! \brief fnc_ECHO                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_ECHO(const char* szCmdLn)
{
  /* place your code here */
  File FH1;
  Serial.print(F("\r\n"));
  if (bSDCRD= SD.begin(SDCRD)) {
    digitalWrite(PIN_LED, 1);
    SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
    FH1 = SD.open(sLogFn, FILE_WRITE);
    if (FH1) {
      FH1.println(szCmdLn+1);
      FH1.close();
    } else {
      Serial.print(F("no File ..."));
      Serial.print(sLogFn);
    } /* end if */
    SD.end();
    digitalWrite(PIN_LED, 0);
  } /* end if */
   return( eECHO );
}  /* end of fnc_ECHO */
 
/**************************************************/
/*! \brief fnc_EEP                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_EEP(const char* szCmdLn)
{
  /* place your code here */
  const uint8_t EEPROM = 0x57;
  uint16_t iAdr;
  uint8_t iDat;
  char sLine[20];

  // Serial.println();
  if (bEEP) {
    Wire.setClock(1000000);
    for (iAdr=0x0000; iAdr <=0x0fff; iAdr++) {
      if ((iAdr%16)==0) {
        Serial.println();
        sprintf(sLine,"0x%4.4X ", iAdr);
        Serial.print(sLine);
      }
      iDat= readEEP(iAdr);
      sprintf(sLine,"%2.2X ", iDat);
      Serial.print(sLine);
    }
    Wire.setClock(200000);
  } else {
    Serial.println("No EEPROM");
  }
  return( eEEP );
}  /* end of fnc_EEP */

/**************************************************/
/*! \brief fnc_FORMAT                               
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_FORMAT(const char* szCmdLn)
{
  /* place your code here */
  SdCard* sd_card = nullptr;
  Serial.println(F(" : "));
  if (bSDCRD= SD.begin(SDCRD)) {
    digitalWrite(PIN_LED, 1);
    SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
    Serial.println(F("A \"format\" function was not implemented"));
    Serial.print("SD-Card error : ");
    SD.end();
    digitalWrite(PIN_LED, 0);
  }  /* end if */
  return( eFORMAT );
}  /* end of fnc_FORMAT */
 
/**************************************************/
/*! \brief fnc_HELP                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_HELP(const char* szCmdLn)
{
  /* place your code here */
  Serial.println(F(" :"));
  Serial.println(F("ALT\t get/set Altitude over NN"));
  Serial.println(F("AUTO\t display temperature, pressure, humidity measuremets each second"));
  Serial.println(F("AUTOLOG\t Save to logfile.txt"));
   Serial.println(F("CD\t change directory"));
  Serial.println(F("CLS\t clearscreen"));
  Serial.println(F("CONFIG\t display configuration"));
  Serial.println(F("COPY\t copy file; <src> <targ>"));
  Serial.println(F("DATE\t display/set date; format <DD.MM.YYYY>"));
  Serial.println(F("DEL\t delete file"));
  Serial.println(F("DIR\t display directory"));
  Serial.println(F("ECHO\t copy argument into logfile"));
  Serial.println(F("EEP\t dump EEPROM"));
  Serial.println(F("FORMAT\t <func. not available>"));
  Serial.println(F("HELP\t this help informations"));
  Serial.println(F("HUM\t display Humidity"));
  Serial.println(F("REN\t rename file; <src> <targ>"));
  Serial.println(F("CD\t change directory"));
  Serial.println(F("MD\t make directory"));
  Serial.println(F("MEM\t Memory info"));
  Serial.println(F("PATH\t display actual path"));
  Serial.println(F("PRES\t display pressure"));
  Serial.println(F("QNH\t display pressure at NN"));
  Serial.println(F("RD\t remove directory"));
  Serial.println(F("TEMP\t display temperature(s)"));
  Serial.println(F("TIME\t display/set time; format <hh.mm.ss>"));
  Serial.println(F("TYPE\t display ASCII-file"));
  Serial.println(F("VER\t display SW- Version"));
  Serial.println(F("VOL\t display SD-Card informations"));
  Serial.println(F("XREC\t XModem- download to uC"));
  Serial.println(F("XTRAN\t XModem- upload"));
  Serial.println(F("YREC\t YModem- download to uC"));
  Serial.println(F("YTRAN\t"));
  return( eHELP );
}  /* end of fnc_HELP */

/**************************************************/
/*! \brief fnc_LCD                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_LCD(const char* szCmdLn)
{
  /* place your code here */
  uint32_t value;
  sscanf(szCmdLn,"%4ld", &value);
  if (value>100) value= 100;
  else if (value<0) value = 0;
  value = (value * LCDBRIGHT) / 100;
  analogWrite(LCD_BKL_PIN, value);
  return( eLCD );
}  /* end of fnc_LCD */
  
/**************************************************/
/*! \brief fnc_MD                                   
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_MD(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE];
  argPathFn( szCmdLn, &sLine[0]);
  Serial.print(F(" : "));
  digitalWrite(PIN_LED, 1);
  if (bSDCRD= SD.begin(SDCRD)) {
    if (!SD.exists(sLine)) {
      int16_t iRes= SD.mkdir(sLine);
      if (iRes>0) {
        Serial.print(F("Direcory created"));
      } else {
        Serial.print(F("Direcory not created"));
      } /* end if */
    } else {
      Serial.print(F(" is an existing Directory"));
    } /* end if */
    SD.end();
    digitalWrite(PIN_LED, 0);
  } /* end if */
  return( eMD );
}  /* end of fnc_MD */

/**************************************************/
/*! \brief fnc_PATH                               
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_PATH(const char* szCmdLn)
{
  /* place your code here */
  Serial.print(" : ");
  Serial.print(sPath);
  return( ePATH );
}  /* end of fnc_PROMPT */
 
/**************************************************/
/*! \brief fnc_PRES                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_PRES(const char* szCmdLn)
{
  /* place your code here */
  char sLine[10];
  Serial.print(" Sensor:");
  sprintf(sLine,"%4d.%02d hPa",(iPres/100), (iPres%100));
  Serial.print(sLine);
  iQNH = iPres + iCorPr;
  Serial.print(" QNH: ");
  sprintf(sLine,"%4d.%02d hPa",(iQNH/100), (iQNH%100));
  Serial.print(sLine); 
  return( ePRES );
}  /* end of fnc_PRES */
 
/**************************************************/
/*! \brief fnc_QNH                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_QNH(const char* szCmdLn)
{
  /* place your code here */
  char sLine[10];
  iQNH = iPres + iCorPr;
  Serial.print(" QNH: ");
  sprintf(sLine,"%4d.%02d hPa",(iQNH/100), (iQNH%100));
  Serial.print(sLine); 
  return( eQNH );
}  /* end of fnc_QNH */

/**************************************************/
/*! \brief fnc_HUM                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_HUM(const char* szCmdLn)
{
  /* place your code here */
  char sLine[10];
  Serial.print(" Humidity: ");
  sprintf(sLine,"%4d.%1d %%",(iHum/1000), (iHum % 1000)/100);
  Serial.print(sLine); 
  return( eHUM );
}  /* end of fnc_HUM */

/**************************************************/
/*! \brief fnc_RD                                   
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_RD(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE];
  argPathFn( szCmdLn, &sLine[0]);
  Serial.print(F(" : "));
  digitalWrite(PIN_LED, 1);
  if (bSDCRD= SD.begin(SDCRD)) {
    if (SD.exists(sLine)) {
      int16_t iRes= SD.rmdir(sLine);
      if (iRes>0) {
        Serial.print(F("Direcory removed"));
      } else {
        Serial.print(F("Direcory not removed"));
      } /* end if */
    } else {
      Serial.print(F(" Directory not found"));
    } /* end if */
    SD.end();
  } /* end if */
  digitalWrite(PIN_LED, 0);
  return( eRD );
}  /* end of fnc_RD */
 
/**************************************************/
/*! \brief fnc_REN                               
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_REN(const char* szCmdLn)
{
  /* place your code here */
  char sFrom[ILINE];
  char sTo[ILINE];
  char s1[40];
  char s2[40];
  Serial.print(F(" : "));
  szCmdLn++;
  int iRet= sscanf(szCmdLn,"%s %s", &s1, &s2);
  if (iRet == 2) {
    argPathFn( &s1[0], &sFrom[0]);
    argPathFn( &s2[0], &sTo[0]);
    if (bSDCRD= SD.begin( SDCRD)) {
      digitalWrite(PIN_LED, 1);
      bool stFS= SD.rename(sFrom, sTo);
      if (stFS) {
        Serial.print(F("OK"));
      } else {
        Serial.print(F("Error"));
      } /* end if */
      SD.end();
      digitalWrite(PIN_LED, 0);
    } /* end if */
  } /* end if */
  return( eREN );
}  /* end of fnc_RENAME */
 
/**************************************************/
/*! \brief fnc_TEMP                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_TEMP(const char* szCmdLn)
{
  /* place your code here */
  char sLine[20];
  if (bRTC) {
    Serial.print(" RTC: ");
    Serial.print(RTC.getTemp());
  }  /* end if */
  Serial.print(" CPU: ");
  Serial.print(analogReadTemp(3.3f));
  sprintf(sLine," Sensor: %3d.%02d °C",(iTemp/100), (iTemp%100));
  Serial.print(sLine);
  return( eTEMP );
}  /* end of fnc_TEMP */
 
/**************************************************/
/*! \brief fnc_TIME                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_TIME(const char* szCmdLn)
{
   /* place your code here */   
  char sLine[ILINE];
  if (strlen(szCmdLn) >= 3) {
    int iResult= sscanf( szCmdLn,"%02d:%02d:%02d", &hour, &minute, &second);
    if (iResult == 3) {
      if (bRTC) {
        RTC.setTime((uint8_t) hour, (uint8_t) minute, (uint8_t) second);
      } /* end if */
      mytm.tm_hour = hour;
      mytm.tm_min  = minute;
      mytm.tm_sec  = second; 
    } else  
    if (iResult == 2) {
      if (bRTC) {
        RTC.setHours((uint8_t)hour);
        RTC.setMinutes((uint8_t)minute);
      } /* end if */
      mytm.tm_hour = hour;
      mytm.tm_min  = minute;
    } /* end if */
    tiV.tv_sec = mktime(&mytm);
    tiV.tv_usec = 0;
    settimeofday(&tiV, nullptr);
  } else {
    Serial.print(" : ");
    if (bRTC) {
      Serial.print("RTC: ");
      sprintf(sLine,"%2.2d:%2.2d:%2.2d", RTC.getHours(), RTC.getMinutes(), RTC.getSeconds());
      Serial.print(sLine);
    } /* end if */
    Serial.print(" CPU: ");
    time(&tiUx);
    strftime(sLine, sizeof(sLine), "%0H:%0M:%0S", localtime(&tiUx));
    Serial.print(sLine);
  } /* end if */
  return( eTIME );
}  /* end of fnc_TIME */
 
/**************************************************/
/*! \brief fnc_TYPE                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_TYPE(const char* szCmdLn)
{
  /* place your code here */
  #define BUFLEN 1024
  char sLine[ILINE]= {""};
  int16_t iBufLen= BUFLEN;
  unsigned char ucBuffer[BUFLEN];
  argPathFn( szCmdLn, &sLine[0]);
  Serial.print(F(" :\n"));
  digitalWrite(LED_BUILTIN, 1);
  if (bSDCRD= SD.begin(SDCRD)) {
    SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
    File FH1 = SD.open(sLine, FILE_READ);
    if (FH1) {
      while (FH1.available()) {
        iBufLen= FH1.read(&ucBuffer[0], BUFLEN);
        Serial.write(&ucBuffer[0], iBufLen);
      } /* end while */
      Serial.print(F("\r\nFilesize: "));
      Serial.print(FH1.size(), DEC);
      Serial.print(F(" Byte"));
      FH1.close();
    } else {
      Serial.print(sLine);
      Serial.println(F(" not found!"));
    } /* end if */ 
    SD.end();
    digitalWrite(LED_BUILTIN, 0);
  } /* end if */
   return( eTYPE );
}  /* end of fnc_TYPE */
 
/**************************************************/
/*! \brief fnc_VER                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_VER(const char* szCmdLn)
{
   /* place your code here */
  Serial.println(F(" : "));
  Serial.print(F("Softwarebuild "));
  Serial.print(__DATE__);
  Serial.print(F(" "));
  Serial.println(__TIME__);
  
  Serial.println(BOARD_NAME);
  Serial.print(F("CPU- Frequency: "));
  Serial.print(F_CPU/1000000);
  Serial.println(F(" MHz"));
  Serial.print(F("Pico SDK Version: "));
  Serial.println(PICO_SDK_VERSION_STRING);
  Serial.print(F("Arduino Version:  "));
  Serial.print(ARDUINO_PICO_VERSION_STR);  
  return( eVER );
}  /* end of fnc_VER */
 
/**************************************************/
/*! \brief fnc_VOL                                  
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_VOL(const char* szCmdLn)
{
  /* place your code here */
  Serial.println(F(": "));
  digitalWrite(PIN_LED, 1);
  if (bSDCRD= SD.begin(SDCRD)) {
    Serial.print(F("SD-Card type: "));
    switch (SD.type()) {
      case SD_CARD_TYPE_SD1:
        Serial.println(F("SD1"));
        break;
      case SD_CARD_TYPE_SD2:
        Serial.println(F("SD2"));
        break;
      case SD_CARD_TYPE_SDHC:
        Serial.println(F("SDHC"));
        break;
      default:
        Serial.println(F("Unknown"));
    } /* end switch */
    Serial.print(F("SD-card size: "));
    Serial.print(SD.size64()/(1024*1024));
    Serial.println(F(" MByte"));
    Serial.printf("Blocksize  : %d Byte\r\n", SD.blockSize());
    Serial.printf("Blocks     : %d\r\n", SD.totalBlocks());
    Serial.printf("Clustersize: %d Byte\r\n", SD.clusterSize());
    Serial.printf("Cluster    : %d\r\n", SD.totalClusters());
  } /* end if */
  SD.end();
  digitalWrite(PIN_LED, 0);
  return( eVOL );
}  /* end of fnc_VOL */
 
/**************************************************/
/*! \brief fnc_XREC                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
#define XENTRY    0
#define XBLKNUM   1
#define XBLKNUMI  2
#define XDATABLK  3
#define XCHKCRC   4
#define XCHKEOT   5
int fnc_XREC(const char* szCmdLn)
{
  /* place your code here */
  #define Y_BSIZE 1024
  #define X_BSIZE 128
  char sLine[ILINE] = {""};
  int32_t iByteCnt = 0, iBlkCnt;
  int8_t iRecState;   // Receiver statemachine
  bool bRecEnd=false;
  unsigned char inChar;
  int16_t iBlkSize;
  unsigned char ucBuffer[Y_BSIZE];
  uint64_t iStrtTi;
  File FH1;
  Serial.print(" : ");
  if (((strlen(szCmdLn)>1)>=1)&& (bSDCRD= SD.begin(SDCRD))) {
    digitalWrite(PIN_LED, 1);
    SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
    argPathFn( szCmdLn, &sLine[0]);

    if (SD.exists(sLine)) {
      SD.remove(sLine);
    } /* end if */
    FH1 = SD.open(sLine, (O_WRITE|O_CREAT));

    iBlkCnt = 0;
    iRecState = XENTRY;

    while (!bRecEnd) {
      if (iRecState == XENTRY) {
        Serial.write(NAK);
        iStrtTi= millis()+ X_TIMEOUT;
        while ( (!Serial.available())  &&(millis()<iStrtTi)) {  }
        if ((Serial.readBytes(&inChar,1)>=1) &&(millis()<iStrtTi)) {
          if ((inChar== STX) ) {
            iBlkSize  = Y_BSIZE;
            iRecState = XBLKNUM; // found Header token 
          } else 
          if ((inChar== SOH) ) {
            iBlkSize  = X_BSIZE;
            iRecState = XBLKNUM; // found Header token 
          } else 
          if ( (inChar == EOT) ) {
            Serial.write((uint8_t)NAK); // end of transmision
          } else {
            bRecEnd= true;
          } /* end if */
        } else
        if ((iBlkCnt < 5)&&(millis()>=iStrtTi) ) {
          iBlkCnt++;
        } else {
          bRecEnd= true;
        } /* end if */
      } else 
      if (iRecState == XBLKNUM) { 
        iStrtTi= millis()+X_TIMEOUT;
        while ( (!Serial.available()) && (millis() < iStrtTi) ) {  }
        if ((Serial.readBytes(&inChar,1)==1) &&(millis()<iStrtTi)) {
          iBlkCnt= inChar;
          iRecState = XBLKNUMI;
        } else {
          bRecEnd= true;
        } /* end if */
      } else 
      if (iRecState == XBLKNUMI) {  // inverse block counter - not used!
        iStrtTi= millis()+X_TIMEOUT;
        while ( (!Serial.available()) && (millis() < iStrtTi) ) { 
        }
        if ((Serial.readBytes(&inChar,1)==1) &&(millis()<iStrtTi)) {
          iRecState = XDATABLK;
        } else {
          bRecEnd= true;
        } /* end if */
      } else
      if (iRecState == XDATABLK) { 
        iStrtTi= millis()+X_TIMEOUT;
        while((Serial.readBytes(&ucBuffer[0],iBlkSize)<iBlkSize)&&(millis()<iStrtTi) ) { }  
        if (millis()<iStrtTi) {
          iByteCnt= iByteCnt + iBlkSize;
          FH1.write(&ucBuffer[0], iBlkSize);
          iRecState = XCHKCRC;
        } else {
          bRecEnd= true;
        } /* end if */
      } else
      if (iRecState == XCHKCRC) { 
        iStrtTi= millis()+X_TIMEOUT;
        while ( (Serial.readBytes(&inChar,1)<1) && (millis() < iStrtTi) ) { }
        if (millis()<iStrtTi) {
          Serial.write((uint8_t)ACK);
          iRecState = XCHKEOT; // found Header token 
        } else {
          bRecEnd= true;
        } /* end if */
      } /* end if */
      if (iRecState == XCHKEOT) { 
        iStrtTi= millis()+X_TIMEOUT;
        while ( (!Serial.available()) && (millis() < iStrtTi) ) { 
        }
        if ((Serial.readBytes(&inChar,1)==1) &&(millis()<iStrtTi)) {
          if ((inChar== STX) && (millis()<iStrtTi)) {
            iBlkSize  = Y_BSIZE;
            iRecState = XBLKNUM; // found token for next data block
          } else
          if ((inChar== SOH) && (millis()<iStrtTi)) {
            iBlkSize  = X_BSIZE;
            iRecState = XBLKNUM; // found token for next data block
          } else
          if ((inChar== EOT) && (millis()<iStrtTi)) {
            Serial.write((uint8_t)ACK);
            bRecEnd   = true;
          } else {
            for (int iL= 1; iL<3; iL++){
              Serial.write((uint8_t)CAN);
            } /* end for */
            bRecEnd= true;
          } /* end if */
        } /* end if */
      } /* end if */
    } /* end while */
    if (iRecState== XCHKEOT) {
      FH1.close();
      Serial.print("\nByte Cnt ");
      Serial.print(iByteCnt);
      Serial.println("\nDone");
    } else {
      for (int iL= 1; iL<3; iL++) {
        Serial.write((uint8_t)CAN);
      } /* end for */
      if (FH1 !=0 ) FH1.close();
      Serial.print("\nstoped at state ");
      Serial.print(iRecState);
      Serial.print("\nBlock Cnt ");
      Serial.print(iBlkCnt);
      Serial.print("\nByte Cnt ");
      Serial.print(iByteCnt);
    } /* end if */
  } else {
    Serial.println(" missing argument!");
  } /* end if */
  SD.end();
  digitalWrite(PIN_LED, 0);
  return( eXREC );
}  /* end of fnc_XREC */

/**************************************************/
/*! \brief fnc_XTRAN                                
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_XTRAN(const char* szCmdLn)
{
  /* place your code here */
  char sLine[ILINE]={""};
  uint64_t iFSize = 0;
  uint16_t iCSum, iCrc;
  uint16_t iBlkCnt = 1;
  uint8_t iReTr = 0; 
  bool bTiOut = false;
  bool bTrans = false;
  bool bCrc   = true;
  char inChar;       
  unsigned char ucBuffer[Y_BSIZE];
  uint64_t iStrtTi;

  Serial.print(F(" : "));
  if (strlen(szCmdLn) > 1) {
    digitalWrite(PIN_LED, 1); 
    argPathFn( szCmdLn, &sLine[0]); 
    if (bSDCRD= SD.begin(SDCRD)) {
      SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
      File FH1 = SD.open(sLine, FILE_READ);
      if (FH1 != 0) {
        iFSize = FH1.size();
        Serial.print(iFSize);
        Serial.print(F(" Bytes"));
        while ((FH1.available()!=0) && (!bTiOut)) {
          iStrtTi = millis() + (6 * X_TIMEOUT);   // Timeout should be near 60 sec
          while ((Serial.readBytes(&inChar,1)==0) && (millis()<iStrtTi)) {  } 
          if (millis()>=iStrtTi){
            for (int iL= 1; iL<3; iL++) {
              Serial.write((uint8_t) CAN);
            } /* end for */
            bTiOut= true;
          } else
          // Datablock is CRC 1kByte
          if (((inChar=='C')&&(!bTrans))||((inChar==ACK)&&(bTrans)&&bCrc)) {
            bCrc = true;
            iStrtTi = millis() + (X_TIMEOUT);
            bTrans= true;
            iReTr = 0;
            int iRdLen= FH1.read(ucBuffer, Y_BSIZE);
            if (iRdLen > 0) {
              if (iRdLen < Y_BSIZE) {
                for (int iL= iRdLen; iL<Y_BSIZE; iL++){
                  ucBuffer[iL]= 0x00;
                } /* end for */
              } /* end if */
              Serial.write((uint8_t) STX);
              Serial.write((uint8_t) iBlkCnt);
              Serial.write((uint8_t) ~iBlkCnt);
              Serial.write(ucBuffer, Y_BSIZE); // block transfer
              iCrc = 0;
              for (int iL=0; iL<Y_BSIZE; iL++){
                iCrc= uicalcCrc(ucBuffer[iL], iCrc);
              } /* end for */
              Serial.write((uint8_t) ((iCrc &0xff00) >>8));
              Serial.write((uint8_t) (iCrc & 0x00ff));
              iBlkCnt++;
              iBlkCnt= iBlkCnt & 0x00ff;
            // } else {
            //   bCrc = false;  // if rest >128 Byte, transfer in 128 byte mode
            } /* end if */
          } else
          // checksum 128 Byte
          if (((inChar==NAK)&&(!bTrans))||((inChar==ACK)&&(bTrans))||!bCrc) {
            bCrc= false;          
            iStrtTi = millis() + (X_TIMEOUT);
            bTrans= true;
            iCSum = 0;
            iReTr = 0;
            int iRdLen= FH1.read(ucBuffer, X_BSIZE);
            if (iRdLen > 0) {
              if (iRdLen < X_BSIZE) {
                for (int iL= iRdLen; iL<X_BSIZE; iL++) {
                  ucBuffer[iL]= 0x00;
                } /* end for */
              } /* end if */
              Serial.write((uint8_t) SOH);
              Serial.write((uint8_t) iBlkCnt);
              Serial.write((uint8_t) ~iBlkCnt);
              // Blocktransfer vs. single char transfer
              Serial.write(ucBuffer, X_BSIZE); // block transfer
              for (int iL=0; iL<X_BSIZE; iL++){
                iCSum = iCSum + ucBuffer[iL];
                iCSum = iCSum & 0x00ff;
              } /* end for */
              Serial.write((uint8_t) iCSum);
              iBlkCnt++;
              iBlkCnt= iBlkCnt & 0x00ff;
            } /* end if */
          } else
          if (inChar == CAN){
            for (int iL= 1; iL<3; iL++) {
              Serial.write((uint8_t) CAN);
            } /* end for */
            bTiOut= true;
          } /* end if */
        } /* end while */ 
        Serial.write((uint8_t) EOT);
        while ((Serial.readBytes(&inChar,1)==0) && (millis()<iStrtTi)) {  } 
        Serial.write((uint8_t) EOT);
        while ((Serial.readBytes(&inChar,1)==0) && (millis()<iStrtTi)) {  } 
        FH1.close();
      } else {
        Serial.print(sLine);
        Serial.println(F(" not found!"));
      } /* end if */ 
      if (!bTiOut){
        Serial.println(F("\ndone!"));
      } else {
        Serial.println(F("\nTimeout!"));
      } /* end if */
      SD.end();
      digitalWrite(PIN_LED,0);
    } else {
      Serial.println("\nno SD-Card!");
    } /* end if */
  } else {
    Serial.println(" no argument!");
  } /* end if */
  return( eXTRAN );
} /* end of fnc_XTRAN */

/**************************************************/
/*! \brief fnc_YREC                                 
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
#define YENTRY    0
#define YBLKNUM   1
#define YBLKNUMI  2
#define YDATABLK  3
#define YFNAMBLK  4
#define YCHKCRC   5
#define YCHKEOT   6

#define YMRET 5

int fnc_YREC(const char* szCmdLn)
{
  /* place your code here */
  char      sLine[ILINE] = {0x00};
  int32_t   iByteCnt = 0, iByteSum = 0;
  uint16_t  iBlkCnt=0;
  uint8_t   uiBlkCnt[2];
  int32_t   iFileSize= 0;
  int16_t   iRecState;   // Receiver statemachine
  uint8_t   iChkSum;
  bool      bRecEnd= false;
  unsigned char inChar;
  int16_t   iBlkSize;
  unsigned char ucBuffer[Y_BSIZE];
  uint64_t  iStrtTi;
  File FH1;
  
  Serial.print("\n");
  iBlkCnt = 0;
  iRecState = YENTRY;
  if (bSDCRD= SD.begin(SDCRD)) {
    digitalWrite(PIN_LED, 1);
    SDSPI.beginTransaction(SPISettings(SD_SCK_MHZ(SDSPD), MSBFIRST, SPI_MODE0)); 
    Serial.flush();
    while (!bRecEnd) {
      if (iRecState == YENTRY) {
        Serial.write("C");    // "C" request for 1kB/CRC transmission 
        iStrtTi= millis() + X_TIMEOUT; 
        while ( (!Serial.available()) && (millis()<iStrtTi) ) { }
        if ((Serial.readBytes(&inChar,1)==1) && (millis()<iStrtTi)) {
          if (inChar == STX) {
            iBlkSize  = Y_BSIZE;
            iRecState = YBLKNUM; // found Header token 
          } else 
          if (inChar == SOH ) {
            iBlkSize  = X_BSIZE;
            iRecState = YBLKNUM; // found Header token 
          } else 
          if (inChar == EOT) {
            Serial.write((uint8_t)NAK); // end of transmision
            delay (10);
            Serial.write((uint8_t)ACK);
            iRecState = 901;
            bRecEnd = true;
          } else {
          } /* end if */
        } else
        if ((iBlkCnt < YMRET) && (millis()>=iStrtTi) ) {
          iBlkCnt++;
        } else {
          iRecState = 90;
        } /* end if */
      } else 
      if (iRecState == YBLKNUM) { 
        iStrtTi= millis()+X_TIMEOUT;
        while ( (!Serial.available()) ) { }
        if ((Serial.readBytes(&uiBlkCnt[0],2)==2) &&(millis()<iStrtTi)) {
          iBlkCnt   = uiBlkCnt[0];
          if ((iByteCnt==0) && (iBlkCnt == 0)) {
            iRecState = YFNAMBLK;  // Filename in first block 
          } else {
            iRecState = YDATABLK;
          } /* end if */
        } /* end if */
      } else
      if (iRecState == YDATABLK) { 
        iStrtTi= millis()+X_TIMEOUT;
        while((Serial.readBytes(&ucBuffer[0], iBlkSize) < iBlkSize) ) { }  
        if (millis()<iStrtTi) {
          iRecState = YCHKCRC;
        } else {
          iRecState = 93;
        } /* end if */
      } else
      if (iRecState == YFNAMBLK) { // YMODEM Filename block
        iStrtTi = millis()+X_TIMEOUT;
        while ( (!Serial.available()) && (millis() < iStrtTi) ) { }
        if ((Serial.readBytes(&ucBuffer[0], iBlkSize) == iBlkSize) &&(millis()<iStrtTi)) {
          if (strlen((char*)ucBuffer)>=1) {
            argPathFn( (char*)ucBuffer, &sLine[0]);
            iRecState = YCHKCRC;
          } else { // no filename found
            strcpy(sLine,"");
            iRecState  = YCHKCRC;
          } /* end if */
        } else {
          iRecState  = 931;
        } /* end if */
      } else
      if (iRecState == YCHKCRC) { 
        iStrtTi= millis()+X_TIMEOUT;
        unsigned char cCrc[2];
        while ( (!Serial.available()) ) { }
        if ((Serial.readBytes(&cCrc[0], 2)==2) &&(millis()<iStrtTi)) {
          if ( millis()<iStrtTi ) {
            uint16_t iMyCRC =0;   // my calculated CRC
            for (int iL=0; iL < iBlkSize; iL++) {
              iMyCRC= uicalcCrc(ucBuffer[iL],iMyCRC);
            } /* end for */
            uint16_t iCRC = (cCrc[0]<<8) + cCrc[1]; // remote CRC
            if ((iByteCnt==0) && (iBlkCnt==0)) { // Req. New file
              if (strlen(sLine)>1) {
                if (!FH1) {               // if Filehandle was closed
                  if (SD.exists(sLine)) { // remove if file exists 
                    SD.remove(sLine);
                  } /* end if */
                  FH1 = SD.open(sLine, (O_WRITE|O_CREAT)); // create file
                }
                Serial.write((uint8_t)ACK);
                delay(10);
                Serial.write("C");  // YModem: reopen connection after Filename
              } else {  // no filename - exit function
                Serial.write((uint8_t)ACK);
                bRecEnd= true;
              } /* end if */
            } else {
              FH1.write(ucBuffer, iBlkSize);
              iByteCnt = iByteCnt + iBlkSize;
              Serial.write((uint8_t)ACK);
            } /* end if */
            iRecState = YCHKEOT; //  
          }  /* end if */
        } else {
          iRecState =94;
        } /* end if */
      } else
      if (iRecState == YCHKEOT) { 
        iStrtTi= millis() + X_TIMEOUT;
        while ( (!Serial.available()) && (millis()<iStrtTi) ) { }
        if ((Serial.readBytes(&inChar,1)==1) && (millis()<iStrtTi)) {
          if (inChar== STX) {
            iBlkSize= Y_BSIZE;
            iRecState = YBLKNUM;  // get next data block in 1k-CRC mode
          } else
          if (inChar== SOH) {
            iBlkSize= X_BSIZE;
            iRecState = YBLKNUM;  // get next data block in 128 Byte- CRC mode
          } else
          if (inChar== EOT) {
            if (FH1) {
              FH1.close();          // close file handle
              Serial.write((uint8_t)NAK);
              Serial.readBytes(&inChar,1);
              delay(15);
              Serial.write((uint8_t)ACK);
              delay(15);
              Serial.write("C");
              iStrtTi= millis()+X_TIMEOUT;
              while ( (!Serial.available()) && (millis()<iStrtTi) ) { }
              if ((Serial.readBytes(&inChar,1)==1) && (millis()<iStrtTi)) {
                if ((inChar== SOH) && (millis()<iStrtTi)) { // start of header
                  iBlkSize = X_BSIZE;
                  iByteSum = iByteSum + iByteCnt;
                  iByteCnt = 0;
                  iRecState= YBLKNUM;  // get next file
                } else {
                  iByteSum = iByteSum + iByteCnt;
                  bRecEnd= true;
                } /* end if */
              } /* end if */
            } else {
              iByteSum = iByteSum + iByteCnt;
              Serial.write((uint8_t)ACK);
              delay(5);
              bRecEnd= true;
            } /* end if */
          } /* end if */
        } else {
          iRecState = 95;
        } /* end if */
      } else
      if (iRecState >= 90) {  // something goes wrong
        for (int iL= 1; iL<3; iL++){
          Serial.write((uint8_t)CAN);
        } /* end for */
        bRecEnd= true;
      } /* end if */
    } /* end while */
    if (iRecState == YCHKEOT) {
      Serial.print("\nByte Cnt ");
      Serial.print(iByteSum);
      Serial.println("\nDone");
    } else {
      Serial.print("\nBlock Cnt ");
      Serial.print(iBlkCnt);
      Serial.print(" stoped at state ");
      Serial.print(iRecState);
      Serial.print("\nByte Cnt ");
      Serial.print(iByteSum);
    } /* end if */
    SD.end();
    digitalWrite(PIN_LED, 0);
  } /* end if */
  return( eYREC );
}  /* end of fnc_YREC */

/**************************************************/
/*! \brief fnc_YTRAN                                
    \param argument string: pointer of char
    \return int- value of token
    \ingroup token_parser */
/**************************************************/
int fnc_YTRAN(const char* szCmdLn)
{
  /* place your code here */
   return( eYTRAN );
}

/**************************************************/
/*! \brief fnc_TokenNotFound
    \param argument string: pointer of char
    \return always 0
    \ingroup token_parser */
/**************************************************/
int fnc_TokenNotFound(const char* szCmdLn)
{
   /* place your code here */
   Serial.print(F(" <?>"));
   return( eNoToken );
}  /* end of fnc_TokenNotFound */

/**************************************************/
/*! \brief parser main function
     fnSDOS_Parser                                  
    \param szCmdLn is a pointer of char Cmd-Line
    \return int value of token; 0: no token found
    \ingroup token_parser
    \sa datetime_t */
/**************************************************/
int fnSDOS_Parser(char *szCmdLn)
{
   int iCmdLn;
   int iCmdPos;
   int iRet;
 
   iCmdPos= strcspn(szCmdLn," ");
   if (iCmdPos <= 0) iCmdPos= strlen(szCmdLn);
 
   iCmdLn= strncmp( szCmdLn, "LCD", (iCmdPos >= sizeof("LCD"))? iCmdPos: sizeof("LCD")-1);
   if (iCmdLn < 0) { // is less than LCD
      iCmdLn= strncmp( szCmdLn, "DATE", (iCmdPos >= sizeof("DATE"))? iCmdPos: sizeof("DATE")-1);
      if (iCmdLn < 0) { // is less than DATE
         iCmdLn= strncmp( szCmdLn, "CD", (iCmdPos >= sizeof("CD"))? iCmdPos: sizeof("CD")-1);
         if (iCmdLn < 0) { // is less than CD
            iCmdLn= strncmp( szCmdLn, "AUTO", (iCmdPos >= sizeof("AUTO"))? iCmdPos: sizeof("AUTO")-1);
            if (iCmdLn == 0) {
               iRet= fnc_AUTO(szCmdLn+sizeof("AUTO")-1);
            } else { // not AUTO
               if (iCmdLn < 0) {
                  iCmdLn= strncmp( szCmdLn, "ALT", (iCmdPos >= sizeof("ALT"))? iCmdPos: sizeof("ALT")-1);
                  if (iCmdLn == 0) {
                     iRet= fnc_ALT(szCmdLn+sizeof("ALT")-1);
                  } else { //unknown token
                     iRet= fnc_TokenNotFound(szCmdLn);
                  }
               } else {
                  if (strncmp( szCmdLn, "AUTOLOG", (iCmdPos >= sizeof("AUTOLOG"))? iCmdPos: sizeof("AUTOLOG")-1)==0) {
                     iRet= fnc_AUTOLOG(szCmdLn+sizeof("AUTOLOG")-1);
                  } else { //unknown token
                     iRet= fnc_TokenNotFound(szCmdLn);
                  } // End of(3:AUTOLOG)
               } // End of(1:ALT)
            } // End of(2:AUTO)
         } else {
            if (iCmdLn > 0) { // is higher than CD
               iCmdLn= strncmp( szCmdLn, "CONFIG", (iCmdPos >= sizeof("CONFIG"))? iCmdPos: sizeof("CONFIG")-1);
               if (iCmdLn == 0) {
                  iRet= fnc_CONFIG(szCmdLn+sizeof("CONFIG")-1);
               } else { // not CONFIG
                  if (iCmdLn < 0) {
                     iCmdLn= strncmp( szCmdLn, "CLS", (iCmdPos >= sizeof("CLS"))? iCmdPos: sizeof("CLS")-1);
                     if (iCmdLn == 0) {
                        iRet= fnc_CLS(szCmdLn+sizeof("CLS")-1);
                     } else { //unknown token
                        iRet= fnc_TokenNotFound(szCmdLn);
                     }
                  } else {
                     if (strncmp( szCmdLn, "COPY", (iCmdPos >= sizeof("COPY"))? iCmdPos: sizeof("COPY")-1)==0) {
                        iRet= fnc_COPY(szCmdLn+sizeof("COPY")-1);
                     } else { //unknown token
                        iRet= fnc_TokenNotFound(szCmdLn);
                     } // End of(7:COPY)
                  } // End of(5:CLS)
               } // End of(6:CONFIG)
            } else {
               if (iCmdLn == 0) { // Token CD found
                  iRet= fnc_CD(szCmdLn+sizeof("CD")-1);
               } // End of(4:CD)
            }
         }
      } else {
         if (iCmdLn > 0) { // is higher than DATE
            iCmdLn= strncmp( szCmdLn, "EEP", (iCmdPos >= sizeof("EEP"))? iCmdPos: sizeof("EEP")-1);
            if (iCmdLn < 0) { // is less than EEP
               iCmdLn= strncmp( szCmdLn, "DIR", (iCmdPos >= sizeof("DIR"))? iCmdPos: sizeof("DIR")-1);
               if (iCmdLn == 0) {
                  iRet= fnc_DIR(szCmdLn+sizeof("DIR")-1);
               } else { // not DIR
                  if (iCmdLn < 0) {
                     iCmdLn= strncmp( szCmdLn, "DEL", (iCmdPos >= sizeof("DEL"))? iCmdPos: sizeof("DEL")-1);
                     if (iCmdLn == 0) {
                        iRet= fnc_DEL(szCmdLn+sizeof("DEL")-1);
                     } else { //unknown token
                        iRet= fnc_TokenNotFound(szCmdLn);
                     }
                  } else {
                     if (strncmp( szCmdLn, "ECHO", (iCmdPos >= sizeof("ECHO"))? iCmdPos: sizeof("ECHO")-1)==0) {
                        iRet= fnc_ECHO(szCmdLn+sizeof("ECHO")-1);
                     } else { //unknown token
                        iRet= fnc_TokenNotFound(szCmdLn);
                     } // End of(11:ECHO)
                  } // End of(9:DEL)
               } // End of(10:DIR)
            } else {
               if (iCmdLn > 0) { // is higher than EEP
                  iCmdLn= strncmp( szCmdLn, "HELP", (iCmdPos >= sizeof("HELP"))? iCmdPos: sizeof("HELP")-1);
                  if (iCmdLn == 0) {
                     iRet= fnc_HELP(szCmdLn+sizeof("HELP")-1);
                  } else { // not HELP
                     if (iCmdLn < 0) {
                        iCmdLn= strncmp( szCmdLn, "FORMAT", (iCmdPos >= sizeof("FORMAT"))? iCmdPos: sizeof("FORMAT")-1);
                        if (iCmdLn == 0) {
                           iRet= fnc_FORMAT(szCmdLn+sizeof("FORMAT")-1);
                        } else { //unknown token
                           iRet= fnc_TokenNotFound(szCmdLn);
                        }
                     } else {
                        if (strncmp( szCmdLn, "HUM", (iCmdPos >= sizeof("HUM"))? iCmdPos: sizeof("HUM")-1)==0) {
                           iRet= fnc_HUM(szCmdLn+sizeof("HUM")-1);
                        } else { //unknown token
                           iRet= fnc_TokenNotFound(szCmdLn);
                        } // End of(15:HUM)
                     } // End of(13:FORMAT)
                  } // End of(14:HELP)
               } else {
                  if (iCmdLn == 0) { // Token EEP found
                     iRet= fnc_EEP(szCmdLn+sizeof("EEP")-1);
                  } // End of(12:EEP)
               }
            }
         } else {
            if (iCmdLn == 0) { // Token DATE found
               iRet= fnc_DATE(szCmdLn+sizeof("DATE")-1);
            } // End of(8:DATE)
         }
      }
   } else {
      if (iCmdLn > 0) { // is higher than LCD
         iCmdLn= strncmp( szCmdLn, "TEMP", (iCmdPos >= sizeof("TEMP"))? iCmdPos: sizeof("TEMP")-1);
         if (iCmdLn < 0) { // is less than TEMP
            iCmdLn= strncmp( szCmdLn, "PRES", (iCmdPos >= sizeof("PRES"))? iCmdPos: sizeof("PRES")-1);
            if (iCmdLn < 0) { // is less than PRES
               iCmdLn= strncmp( szCmdLn, "MEM", (iCmdPos >= sizeof("MEM"))? iCmdPos: sizeof("MEM")-1);
               if (iCmdLn == 0) {
                  iRet= fnc_MEM(szCmdLn+sizeof("MEM")-1);
               } else { // not MEM
                  if (iCmdLn < 0) {
                     iCmdLn= strncmp( szCmdLn, "MD", (iCmdPos >= sizeof("MD"))? iCmdPos: sizeof("MD")-1);
                     if (iCmdLn == 0) {
                        iRet= fnc_MD(szCmdLn+sizeof("MD")-1);
                     } else { //unknown token
                        iRet= fnc_TokenNotFound(szCmdLn);
                     }
                  } else {
                     if (strncmp( szCmdLn, "PATH", (iCmdPos >= sizeof("PATH"))? iCmdPos: sizeof("PATH")-1)==0) {
                        iRet= fnc_PATH(szCmdLn+sizeof("PATH")-1);
                     } else { //unknown token
                        iRet= fnc_TokenNotFound(szCmdLn);
                     } // End of(19:PATH)
                  } // End of(17:MD)
               } // End of(18:MEM)
            } else {
               if (iCmdLn > 0) { // is higher than PRES
                  iCmdLn= strncmp( szCmdLn, "RD", (iCmdPos >= sizeof("RD"))? iCmdPos: sizeof("RD")-1);
                  if (iCmdLn == 0) {
                     iRet= fnc_RD(szCmdLn+sizeof("RD")-1);
                  } else { // not RD
                     if (iCmdLn < 0) {
                        iCmdLn= strncmp( szCmdLn, "QNH", (iCmdPos >= sizeof("QNH"))? iCmdPos: sizeof("QNH")-1);
                        if (iCmdLn == 0) {
                           iRet= fnc_QNH(szCmdLn+sizeof("QNH")-1);
                        } else { //unknown token
                           iRet= fnc_TokenNotFound(szCmdLn);
                        }
                     } else {
                        if (strncmp( szCmdLn, "REN", (iCmdPos >= sizeof("REN"))? iCmdPos: sizeof("REN")-1)==0) {
                           iRet= fnc_REN(szCmdLn+sizeof("REN")-1);
                        } else { //unknown token
                           iRet= fnc_TokenNotFound(szCmdLn);
                        } // End of(23:REN)
                     } // End of(21:QNH)
                  } // End of(22:RD)
               } else {
                  if (iCmdLn == 0) { // Token PRES found
                     iRet= fnc_PRES(szCmdLn+sizeof("PRES")-1);
                  } // End of(20:PRES)
               }
            }
         } else {
            if (iCmdLn > 0) { // is higher than TEMP
               iCmdLn= strncmp( szCmdLn, "VOL", (iCmdPos >= sizeof("VOL"))? iCmdPos: sizeof("VOL")-1);
               if (iCmdLn < 0) { // is less than VOL
                  iCmdLn= strncmp( szCmdLn, "TYPE", (iCmdPos >= sizeof("TYPE"))? iCmdPos: sizeof("TYPE")-1);
                  if (iCmdLn == 0) {
                     iRet= fnc_TYPE(szCmdLn+sizeof("TYPE")-1);
                  } else { // not TYPE
                     if (iCmdLn < 0) {
                        iCmdLn= strncmp( szCmdLn, "TIME", (iCmdPos >= sizeof("TIME"))? iCmdPos: sizeof("TIME")-1);
                        if (iCmdLn == 0) {
                           iRet= fnc_TIME(szCmdLn+sizeof("TIME")-1);
                        } else { //unknown token
                           iRet= fnc_TokenNotFound(szCmdLn);
                        }
                     } else {
                        if (strncmp( szCmdLn, "VER", (iCmdPos >= sizeof("VER"))? iCmdPos: sizeof("VER")-1)==0) {
                           iRet= fnc_VER(szCmdLn+sizeof("VER")-1);
                        } else { //unknown token
                           iRet= fnc_TokenNotFound(szCmdLn);
                        } // End of(27:VER)
                     } // End of(25:TIME)
                  } // End of(26:TYPE)
               } else {
                  if (iCmdLn > 0) { // is higher than VOL
                     iCmdLn= strncmp( szCmdLn, "XTRAN", (iCmdPos >= sizeof("XTRAN"))? iCmdPos: sizeof("XTRAN")-1);
                     if (iCmdLn < 0) { // is less than XTRAN
                        if (strncmp( szCmdLn, "XREC" ,(iCmdPos >= sizeof("XREC"))? iCmdPos: sizeof("XREC")-1)==0) {
                           iRet= fnc_XREC(szCmdLn+sizeof("XREC")-1);
                        } else { //unknown token)
                           iRet= fnc_TokenNotFound(szCmdLn);
                        } // End of(29:XREC)
                     } else {
                        if (iCmdLn > 0) { // is higher than XTRAN
                           if (strncmp( szCmdLn, "YREC", (iCmdPos >= sizeof("YREC"))? iCmdPos: sizeof("YREC")-1)==0) {
                              iRet= fnc_YREC(szCmdLn+sizeof("YREC")-1);
                           } else { // not YREC
                              if (strncmp( szCmdLn, "YTRAN", (iCmdPos >= sizeof("YTRAN"))? iCmdPos: sizeof("YTRAN")-1)==0) {
                                 iRet= fnc_YTRAN(szCmdLn+sizeof("YTRAN")-1);
                              } else { //unknown token
                                 iRet= fnc_TokenNotFound(szCmdLn);
                              } // End of(32:YTRAN)
                           } // End of(31:YREC)
                        } else {
                           if (iCmdLn == 0) { // Token XTRAN found
                              iRet= fnc_XTRAN(szCmdLn+sizeof("XTRAN")-1);
                           } // End of(30:XTRAN)
                        }
                     }
                  } else {
                     if (iCmdLn == 0) { // Token VOL found
                        iRet= fnc_VOL(szCmdLn+sizeof("VOL")-1);
                     } // End of(28:VOL)
                  }
               }
            } else {
               if (iCmdLn == 0) { // Token TEMP found
                  iRet= fnc_TEMP(szCmdLn+sizeof("TEMP")-1);
               } // End of(24:TEMP)
            }
         }
      } else {
         if (iCmdLn == 0) { // Token LCD found
            iRet= fnc_LCD(szCmdLn+sizeof("LCD")-1);
         } // End of(16:LCD)
      }
   }
   return(iRet);
} /* end of function fnSDOS_Parser */