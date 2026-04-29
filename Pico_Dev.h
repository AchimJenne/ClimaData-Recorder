#ifndef PICO_DEV
#define PICO_DEV
/**************************************************/
/*! \brief Automatic generated enum- commands
    \defgroup token_parser
    \sa datetime_t */
/**************************************************/
enum eCmdSt {eNoToken=0,
      eALT, eAUTO, eAUTOLOG, eCD, eCLS, eCONFIG, eCOPY, eDATE,
      eDEL, eDIR, eECHO, eEEP, eFORMAT, eHELP, eHUM, eLCD,
      eMD, eMEM, ePATH, ePRES, eQNH, eRD, eREN, eTEMP,
      eTIME, eTYPE, eVER, eVOL, eXREC, eXTRAN, eYREC, eYTRAN};

//XModem
#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define PAD  0x1a
#define X_BSIZE  128
// #define X_BUF_SIZE X_BSIZE + 6 
#define X_RETRANS 10
#define X_TIMEOUT 9500
#define Y_BSIZE   1024

// ANSI/ASCII control character
#define Cntl_C 0x03
#define C_BS  0x08
#define C_ESC 0x1b
#define S_CLS F("\e[0H\e[2J")
#define S_CR  F("\r\n>")
#define PGROL 25

// SD-Card parameter
#define SDSPD     48
#define PIN_MISO  12
#define PIN_MOSI  11
#define PIN_SCK   10
#define PIN_SS    22
#define SDSPI     SPI1
#define SDCRD     PIN_SS,SPI1

#define MAXLCDPAGE 5

// RTC DS3231
const uint8_t adrRTC = 0x68;
// EEPROM at I2C addr 0x57
const uint8_t adrEEP = 0x57;
// BME680 at I2C addr 
const uint8_t BMEADR = 0x77;

const int16_t iNumAlt= 16;
const int32_t lAltPres[(2*iNumAlt)+1] = {iNumAlt,
  -50,     00,    100,    200,    300,   400,  500,  600,  700,  800,   900,  1000,  1100,   1200,   1300,   1400,
  -592,    00,   1175,   2339,   3492,  4634, 5766, 6886, 7996, 9095, 10184, 11263, 12331,  13388,  14436,  15473
};

const int32_t lPresAlt[(2*iNumAlt)+1] = {iNumAlt,
  54644, 74166, 76018, 77908, 79836, 81802, 83807, 85852, 87937, 90062, 92230, 94439, 96691, 98986,101325,107369,
 500000,260000,240000,220000,200000,180000,160000,140000,120000,100000, 80000, 60000, 40000, 20000,    00,-50000,
};

// Absolute mass of water dependig from temperature in gr/m^3
const float fTempAbsWater[(2*iNumAlt)+1] = {iNumAlt,
 -60.0, -50.0, -40.0, -30.0, -20.0, -10.0,   0.0,  5.00,  10.0,  15.0,  20.0,  25.0,  30.0,  35.0,  40.0,  45.0,
 0.020, 0.063, 0.177, 0.455, 1.075, 2.360, 4.848, 6.795, 9.397,12.823,17.285,23.028,30.342,39.562,51.072,65.311,
};

typedef struct {
  int time;
  int temp;
  int press;
  int humid;
  int alt;
  int dens;
} tRecData;

#endif