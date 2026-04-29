# ClimaData-Recorder
Arduino- Raspberry Pico based Data Recorder.<br/> This Software is doing an accurate altitude calculation and can be used for thermal gradients and full Clima-Data measurement/recording.<br/>
## List of commands
**ALT**  -  Set default Altitude for QNH<br/>
**AUTO** - shows Timestamp and clima measurement date every seconds<br/>
**AUTOLOG**  - record weather data on SD-Card<br/>
**CD**, **MD**, **RD** - directory handling<br/>
**CLS** - clear the VT100 screen and set the cursor position top-left<br/>
**CONFIG** - shows the SPI- configuration<br/>
**COPY** - copy a file from => to filename<br/>
**DEL** - removing file<br/>
**DIR** - shows the directory<br/>
**DATE** and **TIME** - get and set time/date value<br/>
**EEP**  - Dump EEPROM<br/>
**HELP**  -  list all commands<br/>
**HUM**  -  shows humidity<br/>
**LCD**  -  Set the background light intensity (0..100%)<br/>
**PRES**  -  shows air- pressure<br/>
**TEMP** - shows temperature measurement values<br/>
**TIME**  -  get and set time value<br/>
**TYPE** - list an ASCII-File on your terminal<br/>
**VER** - shows some software informations<br/>
**XREC** - XModem-CRC upload to uC SD-Card<br/>
**XTRAN** - XModem download from uC SD-Card<br/>
**YREC** - YMODEM-1K CRC upload to uC SD-Card (multible file transfer is possible)<br/>

```ruby
  if (Serial.available()) { 
    inChar = (char) Serial.read();
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
  }
```
>[!NOTE]
>Required Hardware :<BR/> 
- Raspberry Pico (RP2040/RP2350), 3inch5 LCD with SD-Card (IL9688, BME680, DS3231 and/with AT24CS32 (EEP).<br/>
- This software based on external Software-modules, but some functions are modifyed for less ressources.<br/>
