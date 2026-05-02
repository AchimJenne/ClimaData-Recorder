# ClimaData-Recorder
Arduino- Raspberry Pico based Data Recorder.<br/> This Software is doing an accurate Air temperature, pressure and humidity measurement. Based on this value, this SW calculate the altitude, climb-rate, absolute water mass and the air- desity. The main application should be the measurement and calculation the for thermal gradients over the alititude.<br/>
<img width="800" height="534" alt="CliRecBasisDatVarioAlt" src="https://github.com/user-attachments/assets/22820f3b-5457-4b56-b077-2ae65fbf560a" />
Two Testsystems with Waveshare 2350Plus, Akku, LCD (and SD-Card), DS3231- Module (w. EEP) and the BME680 Module<br/>
>[!NOTE]
>The Variometer value based on the accurate sensor value and the altitude calculation. The calculation is working fine, but the LSB's of the sensor-adc let the value toggle. Typical +/- 0.17 m/s up to +/- 0.5 m/s. The average looks right, but in a stand alone variometer applications... - it's not the main application to have an accurate varioneter.<br/> The variometer value is for information only!<BR/> 

## List of commands
**ALT**    -  Set default Altitude for QNH<br/>
**AUTO**   - shows Timestamp and clima measurement date every seconds<br/>
**AUTOLOG** - record primary weather data and write on the SD-Card<br/>
**CD**, **MD**, **RD** - directory handling<br/>
**CLS**    - clear the VT100 screen and set the cursor position top-left<br/>
**CONFIG** - shows the SPI- configuration<br/>
**COPY**   - copy a file from => to filename<br/>
**DEL**    - removing file<br/>
**DIR**    - shows the directory<br/>
**DATE** and **TIME** - get and set time/date value<br/>
**EEP**    - Dump EEPROM<br/>
**HELP**   - list all commands<br/>
**HUM**    - shows humidity<br/>
**LCD**    - Set the background light intensity (0..100%)<br/>
**PRES**   - shows air- pressure<br/>
**TEMP**   - shows temperature measurement values<br/>
**TIME**   - get and set time value<br/>
**TYPE**   - list an ASCII-File on your terminal<br/>
**VER**    - shows some software informations<br/>
**XREC**   - XModem-CRC upload to uC SD-Card<br/>
**XTRAN**  - XModem download from uC SD-Card<br/>
**YREC**   - YMODEM-1K CRC upload to uC SD-Card (multible file transfer is possible)<br/>

For SD-Card access, this Software is using the SPI- mode. Definition of th SD-Card interface (Pico_Dev.h):
```ruby
// SD-Card parameter
#define SDSPD     48
#define PIN_MISO  12
#define PIN_MOSI  11
#define PIN_SCK   10
#define PIN_SS    22
#define SDSPI     SPI1
#define SDCRD     PIN_SS,SPI1
```
>[!NOTE]
>Required Hardware :<BR/> 
- Raspberry Pico (**RP2040/RP2350**),
- 3inch5 LCD (Waveshare **Pico-ResTouch-LCD-3.5** or comp.ILI9488) with SD-Card 
- **BME680**, Senormodule for Temperature, Air-pressure and humidity (Gas sensor not used)
- **DS3231 and/with AT24CS32** (EEP).<br/>
- This software based on external PD- Software-modules, but some functions are modifyed for this application.<br/>
- The Arduino IDE the full team<br/>
- Raspberry Pico GCC-implementation, done by Earle F. Philhower III<br/>
- BME680 function by Bosch GmbH<br/>
- LCD-GUI implementation, the Waveshare team<br/>
- Realtime clock function by Manjunath CV<br/>
