int32_t iCalcAltitude(int32_t iPres)
{
  return ((int32_t) fnCurFast_I32((101325 + iPres - iOfPre), &lPresAlt[0]) + (iOfAlt * 100));
}

float fCalcDensity(float fTemp, float fPres, float fHumid)
{
  // vapor pressure calculation by the Magnus- equation;  
  const float   fMolGasC= 8.31446261815324; // J/(mol*K)
  const float   fMolMass= 0.0289644;        // kg/mol
  const float   fRS = (fMolGasC / fMolMass);// gas const. dry air 287.058 J/(kg*K)
  const float   fRD = 461.523;              // gas const. water vapor in J/(kg*K)
 
  // fHumid = 0;  // force the dry air desity
  float fPD = 6.112 * exp(17.62 * fTemp/(243.12 + fTemp));
  // Serial.print(fPD,4);
  // Serial.println();
  float fRF = fRS / (1 - (fHumid * fPD / (fPres * fRS / fRD)));
  float fDens = 100.0 * fPres / (fRF * (fTemp + 273.15));

  return (fDens);
}