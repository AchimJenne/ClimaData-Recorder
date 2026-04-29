/****************************************************************************/
#ifndef EDITLINE

#include "Pico_Dev.h"
/**************************************************/
/**
  @brief Interactive VT100- Terminal command input 
  @param [in, out] psLine Buffer to read and edit
  @param [in] inChar serial input byte to interpret
*/
bool editLine(char *psLine, char inChar)
{
  static char sBack[ILINE];
  char sTemp[ILINE];
  static unsigned char iIndx= 0;
  static uint8_t iESC= 0, iL2C= 0;
  static bool bEditMode= false;
  if ((inChar != SOH) && (inChar != STX)) {
      if (inChar == '\r') {      // CR key
        Serial.print(F("\e["));
        Serial.print( strlen(psLine) + strlen(sPath)+2);
        Serial.print(F("G"));   // cursor position End of Line
        iIndx= 0;
        iESC= 0;
        strcpy(&sBack[0], psLine);
        bEditMode= true;
      } else {
        bEditMode= false;
        if (inChar == '\n') {  // new line
          Serial.write(inChar);
        } else 
        if ((inChar == '[') && (iESC ==1)) {
          iESC = 2;
        } else 
        if (((inChar == 'A')||(inChar == 'B')) && (iESC ==2)){ // cursor up-down
          for (char i=0; i<iIndx; i++) {
            Serial.print(F("\b \b"));
          }
          strcpy(&sTemp[0], psLine);
          strcpy(psLine, &sBack[0]);
          strcpy(&sBack[0], &sTemp[0]);
          Serial.print(F("\e[0G"));
          Serial.print(sPath);
          Serial.print(F(">"));
          Serial.print(psLine);
          iIndx= strlen(psLine);
          iESC= 0;
        } else 
        if ((inChar == 'D') && (iESC ==2)) { // key left
          if (iIndx >=1) {
            Serial.print(F("\e[D"));
            iIndx--;
          }
          iESC= 0;
        } else 
        if ((inChar == 'C') && (iESC ==2)) {  // key right
          if (strlen(psLine) > iIndx) {
            Serial.print(F("\e[C"));
            iIndx++;
          }
          iESC= 0;
        } else
        if ((inChar == '1') && (iESC ==2)) {
          iL2C= 1;
          iESC= 3;
        } else
        if ((inChar == '4') && (iESC ==2)) {
          iL2C= 4;
          iESC= 3;
        } else 
        if ((inChar == '5') && (iESC ==2)) {
          iL2C= 5;
          iESC= 3;
        } else 
        if ((inChar == '6') && (iESC ==2)) {
          iL2C= 6;
          iESC= 3;
        } else
        if ((inChar == '~') && (iESC ==3)) {  
          if ((iL2C ==1)){
            Serial.print(F("\e["));
            Serial.print( strlen(sPath)+2);
            Serial.print(F("G"));
            iIndx= strlen(psLine);
            iESC= 0;
          } else
          if ((iL2C ==4)){
            Serial.print(F("\e[0G"));
            Serial.print(sPath);
            Serial.print(F(">"));
            Serial.print(psLine);
            iIndx= strlen(psLine);          
            iESC= 0;
          } else
          if ((iL2C ==5)){
            Serial.print(F("\e["));
            Serial.print(PGROL);
            Serial.print(F("A"));
            iESC= 0;
          } else
          if ((iL2C ==6)){
            Serial.print(F("\e["));
            Serial.print(PGROL);
            Serial.print(F("B"));
            iESC= 0;
          }          
        } else
        if ((iESC >=2)) { // unknown key 
          Serial.println();
          Serial.print(inChar);
          Serial.print(" ");
          Serial.println(inChar, HEX);
        } else
        if ((inChar == C_BS)&&(iESC ==0)) { // backpace key 
          if (strlen(psLine) >= 1) {
            Serial.print(F("\b"));
            int iLen= strlen(psLine);
            if (iLen> iIndx) {
              for (int iLoop=iIndx; iLoop <= iLen; iLoop++) {
                *(psLine+iLoop-1) = *(psLine+iLoop);
              }
              iIndx--;
              Serial.print(psLine + iIndx);
              Serial.print(F(" \e["));
              Serial.print(iLen - iIndx);
              Serial.print(F("D"));
            } else {
              Serial.print(F(" \b"));
              iIndx--;
              *(psLine+iIndx) = 0x00;
            }            
          }
        } else 
        if (inChar == C_ESC)    // escape key
        {
          if (iESC >= 1) {
            for (char i=0; i<iIndx; i++) {
               Serial.print(F("\b \b"));
            }
            *psLine= 0;
            iIndx= 0;
            bEditMode = false;
            iESC= 0;
          } else {
            iESC= 1;
          }
        } else 
        if (inChar == Cntl_C) {
          for (char i=0; i<iIndx; i++) {
            Serial.print(F("\b \b"));
          }
          Serial.flush();
          *psLine= 0;
          iIndx= 0;
          bEditMode = false;
          iESC= 0;
        } else                   
        if (inChar == 0x7f) {
          int iLen= strlen(psLine);
          if (iLen> iIndx) {           
            for (int iLoop=iIndx+1; iLoop <= iLen; iLoop++){
              *(psLine+iLoop-1) = *(psLine+iLoop);
            }
            Serial.print(psLine + iIndx);
            Serial.print(F(" \e["));
            Serial.print(iLen - iIndx);
            Serial.print(F("D"));
          }          
        } else {
          if (iIndx < ILINE) {
            Serial.write(inChar);
            int iLen= strlen(psLine);
            if (iLen> iIndx) {
              for (int iLoop=iLen; iLoop >= iIndx; iLoop--){
                *(psLine+iLoop+1) = *(psLine+iLoop);
              } /* end for */
              *(psLine+iIndx) = inChar;
              Serial.print(psLine + iIndx + 1);              
              Serial.print(F("\e["));
              Serial.print(iIndx + 3 + strlen(sPath));
              Serial.print(F("G"));
              iIndx++;
            } else {
              *(psLine+iIndx) = inChar;
              iIndx++;
              *(psLine+iIndx) = 0x00;
            }
          }
        }
      }
    } else {    
      delay(50);
      bEditMode= false;   
    }
  return (bEditMode);
}
#define EDITLINE
#endif