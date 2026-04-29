/****************************************************************************/
#ifndef SDOSTOOLS
#include "Pico_Dev.h"

/**************************************************/
/*! \brief uicalcCrc                                 
    \param argument char and  UINT16 CRC 
    \return uint16_t- CRC- value
    \ingroup SDOS_Tools */
/**************************************************/
uint16_t uicalcCrc(uint8_t c, uint16_t crc)
{
  int count;
  crc = crc ^ (uint16_t) c << 8;
  count = 8;
  do {
    if (crc & 0x8000) {
      crc = crc << 1 ^ 0x1021;
    } else {
      crc = crc << 1;
    } /* end if */
  } while (--count);
  return crc;
}

/*!
  \brief Write multible bytes at EEP address.
  
  \param address - Memory address.
  \param data - uint8 Data.
  \retval - .
*/
void writeEEP(uint16_t address, uint8_t* data, int len)
{
  uint8_t i;
  for(i = 0; i < len; i++) {
    Wire.beginTransmission(adrEEP);
    Wire.write((uint8_t)((address >> 8) & 0xFF));   // MSB
    Wire.write((uint8_t)(address & 0xFF));          // LSB
    Wire.write(*data++);
    Wire.endTransmission();
    delay(3);
    address++;
  }
}

/*!
  \brief Write unsigned int16 at EEP address
  * @param address - Memory address.
  * @param data - int16_t Data.
  * @retval - .
*/
void writeEEPui16(uint16_t address, int16_t data) 
{
	writeEEP(address, (byte*)&data, 2);
}

/*!
  \brief Write unsigned int32 at EEP address
  *
  * @param address - Memory address.
  * @param data - uint32_t Data.
  * @retval - .
*/
void writeEEPui32(uint16_t address, uint32_t data) 
{
	writeEEP(address, (byte*)&data, 4);
}

/*!
  \brief Read one byte from EEP address.
  *
  * @param memaddress - Memory address.
  * @retval - Byte read.
*/
uint8_t readEEP(uint16_t memaddress)
{
  uint8_t rdata = 0xFF;
  readEEP(memaddress, &rdata, 1);
  return rdata;
}

/*!
  \brief Read multible bytes from EEP address.
  *
  * @param memaddress - Memory address.
  * @retval - Byte read.
*/
void readEEP(uint16_t memaddress, uint8_t* dest, int len)
{
  int iL;

  Wire.beginTransmission(adrEEP);
  Wire.write((uint8_t)(memaddress >> 8));
  Wire.write((uint8_t)(memaddress & 0xFF));
  Wire.endTransmission();

  Wire.requestFrom((int)adrEEP, len);
  for (iL = 0; iL < len; iL++)  {
    if (Wire.available())    {
      *dest++ = Wire.read();
    }
  }
}

/*!
  \brief read signed int16 at EEP address
  * @param address - Memory address.
  * @retval data - int16_t Data.
*/
int16_t readEEPi16(uint16_t memaddress) 
{
  int16_t data;
	readEEP(memaddress, (byte*)&data, 2);
  return(data);
}
 
/*!
  \brief read signed int32 at EEP address
  * @param address - Memory address.
  * @retval data - int32_t Data.
*/
int32_t readEEPi32(uint16_t memaddress) 
{
  int32_t data;
	readEEP(memaddress, (byte*)&data, 4);
  return(data);
} 

/** **/
/*! \brief defLogFn - define the Log- filename - year and month
  \return char-pointer  Create Log- filename - year and month dot log
*/
void defLogFn(int16_t mon,  char* szArgLn)
{
  char sLogFn[15];
  itoa(year, szArgLn, 10);
  switch (mon) {
    case 1: {
      strcat(szArgLn, "Jan");
    }
    break;
    case 2: {
      strcat(szArgLn, "Feb");
    }
    break;
    case 3: {
      strcat(szArgLn, "Mar");
    }
    break;
    case 4: {
      strcat(szArgLn, "Apr");
    }
    break;
    case 5: {
      strcat(szArgLn, "Mai");
    }
    break;
    case 6: {
      strcat(szArgLn, "Jun");
    }
    break;
    case 7: {
      strcat(szArgLn, "Jul");
    }
    break;
    case 8: {
      strcat(szArgLn, "Aug");
    }
    break;
    case 9:{
      strcat(szArgLn, "Sep");
    }
    break;
    case 10:{
      strcat(szArgLn, "Oct");
    }
    break;
    case 11:{
      strcat(szArgLn, "Nov");
    }
    break;
    case 12: {
      strcat(szArgLn, "Dec");
    }
    break;
    default: {
      strcat(szArgLn, "XXX");
    }
  }
  strcat(szArgLn,".LOG");
}

/**************************************************/
/*! \brief fnCurFast_I32 -  interpolate in array of 16 values
    \param argument int32_t iX, const int32_t *iCur with 16 elements
    \return int32_t- result of interpolation
    \ingroup fnCalculation */
 int32_t fnCurFast_I32(int32_t iX, const int32_t *iCur)
 {
    int16_t iL;
    int32_t iY;
    
    if (!(iX >= iCur[16])) {
      if (!(iX <= iCur[1])) {
        if ( iX >= iCur[8] ) {
          if ( iX >= iCur[12] ) {
            if ( iX >= iCur[14] ) {
              if ( iX >= iCur[15] ) {
                iL= 16;
              } else {
                iL= 15;
              }
            } else {
              if ( iX >= iCur[13] ) {
                iL= 14;
              } else {
                iL= 13;
              }
            }
          } else {
            if ( iX >= iCur[10] ) {
              if ( iX >= iCur[11] ) {
                iL= 12;
              } else {
                iL=11;
              }
            } else {
              if ( iX >= iCur[9] ) {
                iL= 10;
              } else {
                iL= 9;
              }
            }
          }
        } else {
          if ( iX >= iCur[4] ) {
            if ( iX >= iCur[6] ) {
              if ( iX >= iCur[7] ) {
                iL= 8;
              } else {
                iL= 7;
              }
            } else {
              if ( iX >= iCur[5] ) {
                iL= 6;
              } else {
                iL= 5;
              }
            }
          } else {
            if ( iX >= iCur[2] ) {
              if ( iX >= iCur[3] ) {
                iL= 4;
              } else {
                iL= 3;
              }
            } else {
              iL= 2;
            }
          }
        }
        iY= (int32_t) iCur[iL+15] + ((iX - iCur[iL-1]) *
              (iCur[iL+16] - iCur[iL+15]) /
              (iCur[iL] - iCur[iL-1]) );
      } else {
        iY= iCur[17];
      }
    } else {
      iY= iCur[32];
    }
  return(iY);
 }

/**************************************************/
/*! \brief fnCurFast_F -  interpolate in array of 16 values
    \param argument float iX, const float *iCur with 16 elements
    \return float- result of interpolation
    \ingroup fnCalculation */
 float fnCurFast_F(float iX, const float *iCur)
 {
    int16_t iL;
    float   iY;
    
    if (!(iX >= iCur[16])) {
      if (!(iX <= iCur[1])) {
        if ( iX >= iCur[8] ) {
          if ( iX >= iCur[12] ) {
            if ( iX >= iCur[14] ) {
              if ( iX >= iCur[15] ) {
                iL= 16;
              } else {
                iL= 15;
              }
            } else {
              if ( iX >= iCur[13] ) {
                iL= 14;
              } else {
                iL= 13;
              }
            }
          } else {
            if ( iX >= iCur[10] ) {
              if ( iX >= iCur[11] ) {
                iL= 12;
              } else {
                iL=11;
              }
            } else {
              if ( iX >= iCur[9] ) {
                iL= 10;
              } else {
                iL= 9;
              }
            }
          }
        } else {
          if ( iX >= iCur[4] ) {
            if ( iX >= iCur[6] ) {
              if ( iX >= iCur[7] ) {
                iL= 8;
              } else {
                iL= 7;
              }
            } else {
              if ( iX >= iCur[5] ) {
                iL= 6;
              } else {
                iL= 5;
              }
            }
          } else {
            if ( iX >= iCur[2] ) {
              if ( iX >= iCur[3] ) {
                iL= 4;
              } else {
                iL= 3;
              }
            } else {
              iL= 2;
            }
          }
        }
        iY= (float) iCur[iL+15] + ((iX - iCur[iL-1]) *
              (iCur[iL+16] - iCur[iL+15]) /
              (iCur[iL] - iCur[iL-1]) );
      } else {
        iY= iCur[17];
      }
    } else {
      iY= iCur[32];
    }
  return(iY);
 }


/**************************************************/
/**
  @brief argPathFn - handle actual Path and Filename
  @param [in, out] pointer to arguments and Buffer to read and edit
*/
int argPathFn(const char* szArgLn, char* szPathFn)
{
  char sLine[ILINE+1];
  char *psL= &sLine[0];
  int iRC;

  if (*szArgLn == ' ') {
    szArgLn++;
  }
  if((iRC= strlen(szArgLn)) > 0) {
    if (strncmp(szArgLn, "/", 1) == 0){
      strcpy(psL, szArgLn);
    } else
    if ((*szArgLn == '.')&&(*(szArgLn+1) == '.')) {
      strcpy(psL, sPath);
      *strrchr(psL, '/') = 0x00;  // find last '/' and substitude to EoS
      if (strlen(psL) <= 1) {
        strcpy(psL, "/");
      }
    } else
    if ((*szArgLn == '.')&&(*(szArgLn+1) == '/')) {
      strcpy(psL, sPath);
      strncat(psL, "/", ILINE);
      strncat(psL, szArgLn +2, ILINE);
    } else {  
      strcpy(psL, sPath);
      if (*(psL+strlen(psL)-1) != '/' ) {
        strncat(psL, "/", ILINE);
      }
      strncat(psL, szArgLn, ILINE);    
    }
  } else {
    strcpy(psL, sPath);
    if (*(psL+strlen(psL)-1) != '/' ) {
      strncat(psL, "/", ILINE);
    }
    iRC= 0;
  }
  strcpy(szPathFn, psL);
  return(iRC);
} /* end of function */

/**************************************************/
/*! \brief func_MonParser function                                 
    \param szCmdLn is a pointer of char Cmd-Line
    \return int value of token; 0: no token found
    \ingroup token_parser
    \sa datetime_t */
/**************************************************/
int func_MonParser(char *szCmdLn)
{
   int iCmdLn;
   int iCmdPos;
   int iRet;
 
   iCmdPos= strcspn(szCmdLn," ");
   if (iCmdPos <= 0) iCmdPos= strlen(szCmdLn);
 
   iCmdLn= strncmp( szCmdLn, "Jul", (iCmdPos>sizeof("Jul")? iCmdPos: sizeof("Jul")));
   if (iCmdLn < 0) // is less than Jul
   {
      iCmdLn= strncmp( szCmdLn, "Dec", (iCmdPos>sizeof("Dec")? iCmdPos: sizeof("Dec")));
      if (iCmdLn < 0) // is less than Dec
      {
         if (strncmp( szCmdLn, "Apr", (iCmdPos>sizeof("Apr")? iCmdPos: sizeof("Apr")))== 0)
         {
            iRet= 4;
         } else { // not Apr
            if (strncmp( szCmdLn, "Aug", (iCmdPos>sizeof("Aug")? iCmdPos: sizeof("Aug")))== 0)
            {
               iRet= 8;
            } else { //unknown token
               iRet= 0;
            } // End of(2:Aug)
         } // End of(1:Apr)
      } else {
         if (iCmdLn > 0) // is higher than Dec
         {
            if (strncmp( szCmdLn, "Feb", (iCmdPos>sizeof("Feb")? iCmdPos: sizeof("Feb")))== 0)
            {
               iRet= 2;
            } else { // not Feb
               if (strncmp( szCmdLn, "Jan", (iCmdPos>sizeof("Jan")? iCmdPos: sizeof("Jan")))== 0)
               {
                  iRet= 1;
               } else { //unknown token
                  iRet= 0;
               } // End of(5:Jan)
            } // End of(4:Feb)
         } else {
            if (iCmdLn == 0) // Token Dec found
            {
               iRet= 12;
            } // End of(3:Dec)
         }
      }
   } else {
      if (iCmdLn > 0) // is higher than Jul
      {
         iCmdLn= strncmp( szCmdLn, "Mar", (iCmdPos>sizeof("Mar")? iCmdPos: sizeof("Mar")));
         if (iCmdLn < 0) // is less than Mar
         {
            if (strncmp( szCmdLn, "Jun", (iCmdPos>sizeof("Jun")? iCmdPos: sizeof("Jun")))== 0)
            {
               iRet= 6;
            } else { // not Jun
               if (strncmp( szCmdLn, "Mai", (iCmdPos>sizeof("Mai")? iCmdPos: sizeof("Mai")))== 0)
               {
                  iRet= 5;
               } else { //unknown token
                  iRet= 0;
               } // End of(8:Mai)
            } // End of(7:Jun)
         } else {
            if (iCmdLn > 0) // is higher than Mar
            {
               iCmdLn= strncmp( szCmdLn, "Oct", (iCmdPos>sizeof("Oct")? iCmdPos: sizeof("Oct")));
               if (iCmdLn == 0)
               {
                  iRet= 10;
               } else { // not Oct
                  if (iCmdLn < 0)
                  {
                     iCmdLn= strncmp( szCmdLn, "Nov", (iCmdPos>sizeof("Nov")? iCmdPos: sizeof("Nov")));
                     if (iCmdLn == 0)
                     {
                        iRet= 11;
                     } else { //unknown token
                        iRet= 0;
                     }
                  } else {
                     if (strncmp( szCmdLn, "Sep", (iCmdPos>sizeof("Sep")? iCmdPos: sizeof("Sep")))== 0)
                     {
                        iRet= 9;
                     } else { //unknown token
                        iRet= 0;
                     } // End of(12:Sep)
                  } // End of(10:Nov)
               } // End of(11:Oct)
            } else {
               if (iCmdLn == 0) // Token Mar found
               {
                  iRet= 3;
               } // End of(9:Mar)
            }
         }
      } else {
         if (iCmdLn == 0) // Token Jul found
         {
            iRet= 7;
         } // End of(6:Jul)
      }
   }
   return(iRet);
} /* end of function func_MonParser */  

/**************************************************/
/**
  * @brief freeMem calculate unused RAM size
**/
extern "C" char* sbrk(int incr);
uint32_t freeMemory() {
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
}
#define SDOSTOOLS
#endif