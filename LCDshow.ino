/*****************************************************************************/
/* global declaration */
static int32_t iAltLoc;
int mday_old = 0;  // for day-wrap detection

/*****************************************************************************/
/*! \brief initClock
    \param void
    \return void
*/
void initGUIClock(void)
{
  char sLn1[80];
  mday_old = 0;
  System_Init();
	
  LCD_SCAN_DIR  lcd_scan_dir = Landscape;
  
	LCD_Init(lcd_scan_dir, 1000);
  LCD_Clear(BLACK);
  
  GUI_DrawRectangle( 0, 0, 479,  40, GRAY, DRAW_FULL, DOT_PIXEL_DFT);
  GUI_DisString( 80,   8, " Clima- Sensors ", &Font24, WHITE, BLUE);
  // GUI_DisString( 88,   8, BOARD_NAME, &Font24, WHITE, BLUE);
  GUI_DisString(  0,  40, "WDay:", &Font24, BLACK, WHITE);
  GUI_DisString(  0,  64, "Date:", &Font24, BLACK, WHITE);
  GUI_DisString(  0,  88, "Time:", &Font24, BLACK, WHITE);
  GUI_DisString(  0, 112, "RTC-Tmp:", &Font24, BLACK, WHITE);
  GUI_DisString(  0, 136, "CPU-Tmp:", &Font24, BLACK, WHITE);
  GUI_DisString(  0, 160, "SD-Card:", &Font24, BLACK, WHITE);
  if (bBME680) {
    GUI_DisString(  0, 184, "Sen-Tmp:", &Font24, BLACK, WHITE);
    GUI_DisString(  0, 208, "Sen-Hum:", &Font24, BLACK, WHITE);
    GUI_DisString(  0, 232, "Sen-Pre:", &Font24, BLACK, WHITE);
    GUI_DisString(  0, 256, "QNH-Pre:", &Font24, BLACK, WHITE);
  }
  GUI_DisString(   0, 280, "USys/UB:", &Font24, BLACK, WHITE);
  GUI_DisString( 300, 280, "Freq: ", &Font20, BLACK, WHITE);
  sprintf(sLn1, "%3ld", ((uint32_t) F_CPU/1000000L));
  GUI_DisString( 378, 280, sLn1,  &Font20, BLACK, WHITE);
  GUI_DisString( 420, 280, "MHz", &Font20, BLACK, WHITE);
  
  initClock(iXOfs, iYOfs);
}

/*****************************************************************************/
/*! \brief showTextData
    \param void
    \return void
*/
void showTextData(void)
{
  // display the text informations
  const uint16_t iLnH = 24, iLnOfs = 40;
  uint16_t iLnCnt= 0;
  uint16_t iLnPos= 0;
  char sLn1[80];

  if (mday_old != mytm.tm_mday) {
    iLnPos = iLnOfs;
    strftime(sLn1, sizeof(sLn1), "%0A", localtime(&tiUx));
    if (strlen(sLn1) <= 6) {
      strcat(sLn1, "   ");
    }
    GUI_DisString(  100, iLnPos, sLn1, &Font24, BLACK, WHITE); 
    iLnCnt++;
    iLnPos= iLnOfs +(iLnH * iLnCnt);
    sprintf(sLn1, "%2.02d-%2.02d-%4.4d", mytm.tm_mday, (mytm.tm_mon+1), (mytm.tm_year+1900));
    GUI_DisString(  100, iLnPos, sLn1, &Font24, BLACK, WHITE);
    mday_old = mytm.tm_mday;
  }
  
  iLnCnt = 2;
  iLnPos= iLnOfs +(iLnH * iLnCnt);  
  sprintf(sLn1, "%2.2d:%2.02d:%2.02d", mytm.tm_hour, mytm.tm_min, mytm.tm_sec);
  GUI_DisString(  100, iLnPos, sLn1, &Font24, BLACK, WHITE); 
  
  iLnCnt = 3;
  iLnPos= iLnOfs +(iLnH * iLnCnt);
  if (bRTC) {
    sprintf(sLn1, "%5.1f C", RTC.getTemp());
    GUI_DisString( 137, iLnPos, sLn1, &Font24,  BLACK, WHITE);
  } else {
    GUI_DisString( 137, iLnPos, "- ", &Font24,  YELLOW, BLACK);
  }
  sprintf(sLn1, "%5.1f C", fCPUTemp);
  iLnCnt++;
  iLnPos= iLnOfs +(iLnH * iLnCnt);  
  GUI_DisString( 137, iLnPos, sLn1, &Font24,  BLACK, WHITE);
  iLnCnt++;
  iLnPos= iLnOfs +(iLnH * iLnCnt);
  if (bSDCRD) {
    GUI_DisString( 137, iLnPos, " OK ", &Font24, GREEN, BLACK);
  } else {
    GUI_DisString( 137, iLnPos, " ERR", &Font24, RED, WHITE);
  } /* end if */  
  
  if(bBME680){
    iLnCnt++;
    iLnPos= iLnOfs +(iLnH * iLnCnt);
    sprintf(sLn1, "%3d.%01d C", (iTemp/100), (iTemp%100)/10);
    GUI_DisString( 137, iLnPos, sLn1, &Font24,  BLACK, WHITE);
    iLnCnt++;
    iLnPos= iLnOfs +(iLnH * iLnCnt);
    sprintf(sLn1, "%3d.%01d pC", (iHum/1000), (iHum % 1000)/100);
    GUI_DisString( 137, iLnPos, sLn1, &Font24,  BLACK, WHITE);
    iLnCnt++;
    iLnPos= iLnOfs +(iLnH * iLnCnt);
    sprintf(sLn1, "%4d.%01d hPa", (iPres/100), (iPres%100)/10);
    GUI_DisString( 137, iLnPos, sLn1, &Font24,  BLACK, WHITE);
    iLnCnt++;
    iLnPos= iLnOfs +(iLnH * iLnCnt);
    iQNH = iPres + iCorPr;
    sprintf(sLn1, "%4d.%01d hPa", (iQNH/100), (iQNH%100)/10);
    GUI_DisString( 137, iLnPos, sLn1, &Font24,  BLACK, WHITE);
  }
  sprintf(sLn1, "%2d.%02d V", (iUSys/1000), (iUSys%1000)/10);
  GUI_DisString( 154, 280, sLn1, &Font24,  BLACK, WHITE);
}

/*****************************************************************************/
/*!
   * @brief   Initial Page 01
   * @details Currently clear and base informations
*/
void initPg01(void) {
  const uint16_t iLnH= 24, iLnOfs=40;
  int16_t iLnCnt= 0;
  int16_t iLnPos;

  if(bBME680) {
      iLnPos  = iLnOfs +(iLnH * iLnCnt);
      GUI_DisString( 1, iLnPos, "Altitude:", &Font24,  BLACK, WHITE);
      iLnCnt++;
      iLnPos  = iLnOfs +(iLnH * iLnCnt);
      GUI_DisString( 1, iLnPos, "Alt.(NN):", &Font24,  BLACK, WHITE);
      iLnCnt++;
      iLnPos  = iLnOfs +(iLnH * iLnCnt);
      GUI_DisString( 1, iLnPos, "Vario(i):        m/s", &Font24,  BLACK, WHITE);
      iLnCnt++;
      iLnPos  = iLnOfs +(iLnH * iLnCnt);
      GUI_DisString( 1, iLnPos, "Vario(a):        m/s", &Font24,  BLACK, WHITE);
    
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 1, iLnPos, "Humidity: ", &Font24, BLACK, WHITE);
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 1, iLnPos, "Max.H2O :        g/m^3", &Font24, BLACK, WHITE);
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 1, iLnPos, "Abs.H2O :        g/m^3", &Font24, BLACK, WHITE);
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 1, iLnPos, "Density :        kg/m^3", &Font24, BLACK, WHITE);
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 1, iLnPos, "Dens.Dry:        kg/m^3", &Font24, BLACK, WHITE);
      iAltLoc = iOfAlt;
  }
}

/*****************************************************************************/
/*!
   * @brief   Display Page 01
   * @details calculate altitude and vario value 
*/
void showPg01( void ) {
  const uint16_t iLnH= 24, iLnOfs= 40, iAvrCnt= 8;
  static int32_t iaAltDif[10] = {0,0,0,0,0,0,0,0,0,0};
  static int8_t  iAltCnt= 0 ;

  char sLn1[30];
  int16_t iLnCnt;
  int32_t iAltDif;
  int32_t iVario;
  int16_t iLnPos;
  
  if(bBME680){
      iLnCnt= 0;
      iLnPos  = iLnOfs +(iLnH * iLnCnt);
      iAltDif = iCalcAltitude(iPres) - (iOfAlt * 100);
      iVario  = iAltDif - iAltLoc;
      iAltLoc = iAltDif; 
      sprintf(sLn1, "%7.02f m", ((float)iAltDif/100.0));
      GUI_DisString( 154, iLnPos, sLn1, &Font24,  BLACK, WHITE);

      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      iAltDif = iAltDif + (iOfAlt * 100);
      sprintf(sLn1, "%7.02f m", ((float)iAltDif/100.0));
      GUI_DisString( 154, iLnPos, sLn1, &Font24,  BLACK, WHITE);

      iLnCnt++;
      iLnPos = iLnOfs + (iLnH * iLnCnt);
      sprintf(sLn1, "%2d.%02d", (iVario/100), abs(iVario%100));
      if (iVario == 0) {
        GUI_DisString( 171, iLnPos, " ", &Font24,  BLACK, BLACK);
        GUI_DisString( 188, iLnPos, sLn1, &Font24, BLACK, WHITE);
      } else if (iVario > 0) {
        GUI_DisString( 171, iLnPos, "+", &Font24,  GREEN, BLACK);
        GUI_DisString( 188, iLnPos, sLn1, &Font24, GREEN, BLACK);
      } else {
        GUI_DisString( 171, iLnPos, "-", &Font24,  RED, WHITE);
        GUI_DisString( 188, iLnPos, sLn1, &Font24, RED, WHITE);
      }

      iAltCnt++;
      iAltCnt = iAltCnt % iAvrCnt;
      iaAltDif[iAltCnt] = iVario;

      int32_t iAltDifSum = 0;
      for (int iL=0; iL<iAvrCnt; iL++){
        iAltDifSum = iAltDifSum + iaAltDif[iL];
      }
      iAltDifSum= iAltDifSum / iAvrCnt;

      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      sprintf(sLn1, "%2d.%02d", (iAltDifSum/100), abs(iAltDifSum%100));
      if (iAltDifSum == 0) {
        GUI_DisString( 171, iLnPos, " ", &Font24,  BLACK, BLACK);
        GUI_DisString( 188, iLnPos, sLn1, &Font24, BLACK, WHITE);
      } else if (iAltDifSum > 0) {
        GUI_DisString( 171, iLnPos, "+", &Font24,  GREEN, BLACK);
        GUI_DisString( 188, iLnPos, sLn1, &Font24, GREEN, BLACK);
      } else {
        GUI_DisString( 171, iLnPos, "-", &Font24,  RED, WHITE);
        GUI_DisString( 188, iLnPos, sLn1, &Font24, RED, WHITE);
      }
      
      float fRelHum = (float) iHum / 100000.0f;
      float fTemp   = (float) iTemp / 100.0f;
      float fAbsHum = fnCurFast_F(fTemp, &fTempAbsWater[0]);
      sprintf(sLn1, "%2.02f", fRelHum);
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 188, iLnPos, sLn1, &Font24, BLACK, WHITE);
      sprintf(sLn1, "%2.02f", fAbsHum);
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 188, iLnPos, sLn1, &Font24, BLACK, WHITE);
      fAbsHum = fnCurFast_F(fTemp, &fTempAbsWater[0]) * fRelHum;
      sprintf(sLn1, "%2.02f", fAbsHum);
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 188, iLnPos, sLn1, &Font24, BLACK, WHITE); 

      // vapor pressure calculation by the Magnus- equation;  fRelHum = 0;  // force the dry air desity
      fDens = fCalcDensity(fTemp, ((float) iPres/100.0), fRelHum);
      sprintf(sLn1, "%5.03f", fDens);
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 188, iLnPos, sLn1, &Font24, BLACK, WHITE);
      
      // fDens = (float)(iPres)/(fRS * (fTemp + 273.15));
      fDens = fCalcDensity(fTemp, ((float) iPres/100.0), 0.0);
      sprintf(sLn1, "%5.03f", fDens);
      iLnCnt++;
      iLnPos  = iLnOfs + (iLnH * iLnCnt);
      GUI_DisString( 188, iLnPos, sLn1, &Font24, BLACK, WHITE);
  }
}

/*****************************************************************************/
/*!
   * @brief   Initialize the altimeter on LCD
   * @details 
   * @param   ixHM : x- position
   * @param   iyHM : y- position
*/   
void initAltMeter(int ixHM, int iyHM)
{
  GUI_DrawCircle(ixHM, iyHM,100, GRAY,  DRAW_EMPTY, DOT_PIXEL_1X1);
  GUI_DrawCircle(ixHM, iyHM, 98, DGRAY, DRAW_FULL,  DOT_PIXEL_1X1);
  GUI_DrawCircle(ixHM, iyHM, 94, LGRAY, DRAW_EMPTY, DOT_PIXEL_1X1);
  GUI_DrawCircle(ixHM, iyHM, 90, BLACK, DRAW_FULL,  DOT_PIXEL_1X1);
  for (int i=0; i < METTICK; i++) { // draw Clock-face Dots
    if ((i%50) == 0) {
      GUI_DrawPoint((uint16_t) (cosMet[i]*88 +ixHM), (uint16_t)(sinMet[i]*88 +iyHM), LGRAY, DOT_PIXEL_5X5, DOT_FILL_AROUND);
    } else
    if ((i%10) == 0) {
      GUI_DrawPoint((uint16_t) (cosMet[i]*84 +ixHM), (uint16_t)(sinMet[i]*84 +iyHM), LGRAY, DOT_PIXEL_3X3, DOT_FILL_AROUND);
    } else {
      GUI_DrawPoint((uint16_t) (cosMet[i]*82 +ixHM), (uint16_t)(sinMet[i]*82 +iyHM), LGRAY, DOT_PIXEL_2X2, DOT_FILL_AROUND);
    } /* end if */
  } /* end for */
}

/*****************************************************************************/
/*!
   * @brief   Initialize the Clock on LCD
   * @details 
   * @param   ixClk : x- position
   * @param   iyClk : y- position
*/   
void initClock(int ixClk, int iyClk)
{
  GUI_DrawCircle(ixClk, iyClk,100, GRAY, DRAW_EMPTY, DOT_PIXEL_1X1);
  GUI_DrawCircle(ixClk, iyClk, 98, DGRAY, DRAW_FULL, DOT_PIXEL_1X1);
  GUI_DrawCircle(ixClk, iyClk, 88, GRAY,  DRAW_FULL, DOT_PIXEL_1X1);
  GUI_DrawCircle(ixClk, iyClk, 76, LGRAY, DRAW_EMPTY,DOT_PIXEL_1X1);
  GUI_DrawCircle(ixClk, iyClk, 72, BLACK, DRAW_FULL, DOT_PIXEL_1X1);
  for (int i=0; i < SECTICK; i++) { // draw Clock-face Dots
    if ((i%15) == 0) {
      GUI_DrawPoint((uint16_t) (cosTab[i]*81 +ixClk), (uint16_t)(sinTab[i]*81 +iyClk), LGRAY, DOT_PIXEL_5X5, DOT_FILL_AROUND);
    } else
    if ((i%5) == 0) {
      GUI_DrawPoint((uint16_t) (cosTab[i]*78 +ixClk), (uint16_t)(sinTab[i]*78 +iyClk), LGRAY, DOT_PIXEL_3X3, DOT_FILL_AROUND);
    } else {
      GUI_DrawPoint((uint16_t) (cosTab[i]*77 +ixClk), (uint16_t)(sinTab[i]*77 +iyClk), LGRAY, DOT_PIXEL_2X2, DOT_FILL_AROUND);
    } /* end if */
  } /* end for */
}
/*****************************************************************************/
/*!
   * @brief   Initialize the altimeter and clock
   * @details 
   * @param   
*/ 
void initAltClock(void)
{
  GUI_DisString( 65, 26, "Altitude       Clock", &Font24, BLACK, WHITE);
  initAltMeter(125, 150);   // Altimeter
  initClock(iXOfs, iYOfs);  // Clock
}

/*****************************************************************************/
/*!
   * @brief   showing altimeter value
   * @details 
   * @param   ixHM  : x- position
   * @param   iyHM  : y- position
   * @param   iAltM : altimeter value in cm
*/ 
void showAltMeter(uint16_t ixHM, uint16_t iyHM, int16_t iAltM)
{
  static uint16_t iAltMOld;
  uint16_t iXEnd, iYEnd;
  uint16_t iAltTmp;
  uint16_t iHandHM = 78;

  iAltTmp= iAltMOld % 100;
  iXEnd = (uint16_t)(-cosMet[iAltTmp] * iHandHM + ixHM);  // 
  iYEnd = (uint16_t)(-sinMet[iAltTmp] * iHandHM + iyHM);
  GUI_DrawLine(ixHM, iyHM, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_2X2);
  iAltTmp= iAltM % 100;
  iXEnd = (uint16_t)(-cosMet[iAltTmp] * iHandHM + ixHM);  // 
  iYEnd = (uint16_t)(-sinMet[iAltTmp] * iHandHM + iyHM);
  GUI_DrawLine(ixHM, iyHM, iXEnd, iYEnd, WHITE, LINE_SOLID, DOT_PIXEL_2X2);
  GUI_DrawLine(ixHM, iyHM, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_1X1);

  iAltTmp= iAltMOld / 10;
  iXEnd = (uint16_t)(-cosMet[iAltTmp] * (iHandHM*0.75) + ixHM);
  iYEnd = (uint16_t)(-sinMet[iAltTmp] * (iHandHM*0.75) + iyHM);
  GUI_DrawLine(ixHM, iyHM, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_3X3);
  iAltTmp  = iAltM / 10;
  iXEnd = (uint16_t)(-cosMet[iAltTmp] * (iHandHM*0.75) + ixHM);
  iYEnd = (uint16_t)(-sinMet[iAltTmp] * (iHandHM*0.75) + iyHM);
  GUI_DrawLine(ixHM, iyHM, iXEnd, iYEnd, WHITE, LINE_SOLID, DOT_PIXEL_3X3);
  GUI_DrawLine(ixHM, iyHM, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_1X1); 
  iAlt =  (iOfAlt * 100);
  iAltMOld = iAltM;
}

/*****************************************************************************/
/*!
   * @brief   displaying analog Clock on LCD
   * @details 
   * @param   ixClk : x- position
   * @param   iyClk : y- position
*/ 
void showClock(int ixClk, int iyClk)
{
  uint16_t iXEnd, iYEnd;
  struct tm static mytmb;  // backup for removing old clock hands 

  // draw the analog clock
  uint16_t iTmp = (mytmb.tm_hour%12)*5 +(mytmb.tm_min/12); // old hour clock hand
  iXEnd = (uint16_t)(cosTab[iTmp]*iClkH+ixClk);
  iYEnd = (uint16_t)(sinTab[iTmp]*iClkH+iyClk);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_3X3);
  iTmp = (mytm.tm_hour%12)*5 +(mytm.tm_min/12);   // new hour hand
  iXEnd = (uint16_t)(cosTab[iTmp]*iClkH+ixClk);
  iYEnd = (uint16_t)(sinTab[iTmp]*iClkH+iyClk);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, WHITE, LINE_SOLID, DOT_PIXEL_3X3);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_1X1);

  iXEnd = (uint16_t)(cosTab[mytmb.tm_min]*iClkM+ixClk); // old minute hand
  iYEnd = (uint16_t)(sinTab[mytmb.tm_min]*iClkM+iyClk);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_2X2);
  iXEnd = (uint16_t)(cosTab[mytm.tm_min]*iClkM+ixClk);  // new minute hand
  iYEnd = (uint16_t)(sinTab[mytm.tm_min]*iClkM+iyClk);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, WHITE, LINE_SOLID, DOT_PIXEL_2X2);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_1X1);
  iXEnd = (uint16_t)(cosTab[mytmb.tm_sec]*iClkS+ixClk);
  iYEnd = (uint16_t)(sinTab[mytmb.tm_sec]*iClkS+iyClk);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_1X1);
  iXEnd = (uint16_t)(cosTab[mytm.tm_sec]*iClkS+ixClk);
  iYEnd = (uint16_t)(sinTab[mytm.tm_sec]*iClkS+iyClk);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, BRRED, LINE_SOLID, DOT_PIXEL_1X1);
  GUI_DrawPoint(ixClk+3, iyClk+3, DGRAY, DOT_PIXEL_8X8, DOT_FILL_RIGHTUP);
  mytmb = mytm;     // Save the timestamp for analog clock   
}
/*****************************************************************************/
/*!
   * @brief   Displaying Altimeter and clock
   * @details 
*/
void showAltClock( void ) {
  static int32_t iAltOld;
  int32_t iAlt;
   
  showClock(iXOfs,iYOfs);

  iAlt = iCalcAltitude(iPres);
  if (iAlt != iAltOld) {
  char sLn1[30];
    sprintf(sLn1, "%8.02f", (float) (iAlt/100.0));
    GUI_DisString( 1, 290, &sLn1[0], &Font24, BLACK, WHITE);
    showAltMeter( 125, 150, (uint16_t) iAlt/1000 );
    iAltOld = iAlt;
  }
}

/*****************************************************************************/
/*!
   * @brief   Initialize the variometer on the LCD
   * @details 
   * @param   ixClk : x- position
   * @param   iyClk : y- position
*/   
void initVario(int ixClk, int iyClk)
{
  int iL, i;
  GUI_DrawCircle(ixClk, iyClk,100, GRAY, DRAW_EMPTY, DOT_PIXEL_1X1);
  GUI_DrawCircle(ixClk, iyClk, 98, DGRAY, DRAW_FULL, DOT_PIXEL_1X1);
  GUI_DrawCircle(ixClk, iyClk, 88, GRAY,  DRAW_FULL, DOT_PIXEL_1X1);
  GUI_DrawCircle(ixClk, iyClk, 76, LGRAY, DRAW_EMPTY,DOT_PIXEL_1X1);
  GUI_DrawCircle(ixClk, iyClk, 72, BLACK, DRAW_FULL, DOT_PIXEL_1X1);
  for (iL= 0; iL <= 50; iL++) { // draw Clock-face Dots
    i = (iL + 20) % 60;
    if ((i % 5) == 0) {
      GUI_DrawPoint((uint16_t) (cosTab[i]*78 +ixClk), (uint16_t)(sinTab[i]*78 +iyClk), DGRAY, DOT_PIXEL_3X3, DOT_FILL_AROUND);
    } else {
      GUI_DrawPoint((uint16_t) (cosTab[i]*77 +ixClk), (uint16_t)(sinTab[i]*77 +iyClk), LGRAY, DOT_PIXEL_2X2, DOT_FILL_AROUND);
    } /* end if */
  } /* end for */
  GUI_DisString( (ixClk-93) , (iyClk-90), "+2", &Font16, BLACK, WHITE);
  GUI_DisString( (ixClk-117), (iyClk-50), "+1", &Font16, BLACK, WHITE);
  GUI_DisString( (ixClk-117), (iyClk- 8),  "0", &Font16, BLACK, WHITE);
  GUI_DisString( (ixClk-117), (iyClk+32), "-1", &Font16, BLACK, WHITE);
  GUI_DisString( (ixClk-93) , (iyClk+72), "-2", &Font16, BLACK, WHITE);
}

/*****************************************************************************/
/*!
   * @brief   show variometer on LCD
   * @details 
   * @param   ixClk : x- position
   * @param   iyClk : y- position
   * @param   iVario : altimeter value in cm
*/   
void showVario(int ixClk, int iyClk, int iVario)
{
  const int iClkH = 70;
  int iXEnd, iYEnd, iTmp;
  static int iTmpOld;
  iXEnd = (uint16_t)(cosTab[iTmpOld]*iClkH+ixClk);
  iYEnd = (uint16_t)(sinTab[iTmpOld]*iClkH+iyClk);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_3X3);

  iTmp = (((( iVario * 5)/100) +45) % 60);
  iXEnd = (uint16_t)(cosTab[iTmp]*iClkH+ixClk);
  iYEnd = (uint16_t)(sinTab[iTmp]*iClkH+iyClk);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, WHITE, LINE_SOLID, DOT_PIXEL_3X3);
  GUI_DrawLine(ixClk, iyClk, iXEnd, iYEnd, BLACK, LINE_SOLID, DOT_PIXEL_1X1);
  iTmpOld = iTmp;
}

/*****************************************************************************/
/*!
   * @brief   Displaying  Vario and altimeter
   * @details 
*/
void showVarioAlt( void ) {
  static int32_t iAltOld, iVarioOld = 0;
  int32_t iAlt, iVario;

  iAlt = iCalcAltitude(iPres);
  iVario= (iAlt - iAltOld);
  if (iVario != iVarioOld) {
    char sLn1[30];
    // Serial.println(iVario);
    sprintf(sLn1, "%6.02f", (float) (iVario/100.0));
    if (iVario == 0) {
      GUI_DisString( 10, 290, &sLn1[0], &Font24, BLACK, WHITE);
    } else 
    if (iVario > 0) {
      GUI_DisString( 10, 290, &sLn1[0], &Font24, GREEN, BLACK);
    } else {
      GUI_DisString( 10, 290, &sLn1[0], &Font24, RED, WHITE);
    }
    showVario( 125, 150, iVario );
    iVarioOld = iVario;
  }
  if (iAlt != iAltOld) {
    char sLn1[30];
    sprintf(sLn1, "%8.02f", (float) (iAlt/100.0));
    GUI_DisString( 240, 290, &sLn1[0], &Font24, BLACK, WHITE);
    showAltMeter( 365, 150, (uint16_t) iAlt/1000 );
    iAltOld = iAlt;
  }
}

/*****************************************************************************/
/*!
   * @brief   init altimeter and temperature
   * @details 
*/
void initAltTemp( void ) {
  GUI_DisString( 65, 26, "Altitude     Temperature", &Font24, BLACK, WHITE);
  initAltMeter(125, 150);   // Altimeter
}

/*****************************************************************************/
/*!
   * @brief   Displaying altimeter and temperature
   * @details 
*/
void showAltTemp( void ) {
  static int32_t iAltOld= 0, iTempOld= 15;
  int32_t iAlt;
  char sLn1[30];

  iAlt = iCalcAltitude(iPres);
  if (iAlt != iAltOld) {
    sprintf(sLn1, "%8.02f", (float) (iAlt/100.0));
    GUI_DisString( 40, 290, &sLn1[0], &Font24, BLACK, WHITE);
    showAltMeter( 125, 150, (uint16_t) iAlt/1000 );  // Altimeter

    sprintf(sLn1, "%8.02f", (float) (iTemp/100.0));
    GUI_DisString( 240, 290, &sLn1[0], &Font24, BLACK, WHITE);
    iTempOld = iTemp;
    iAltOld = iAlt;

  }
}