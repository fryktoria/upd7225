

// Include the library code
#include "upd7225.h"

#define PORT_EXTENDER_I2C_ADDRESS 0x21

UPD7225 lcd(PORT_EXTENDER_I2C_ADDRESS, _EXT_SCK, _EXT_SI, _EXT_CD, _EXT_RESET, _EXT_CS, _EXT_BUSY);

void setup()
{
  // Set up the LCD
  lcd.begin();
  lcd.clearDisplay();

  lcd.setSymbol(S_DELAY);
  lcd.setSymbol(S_INPUT_SEL);

  lcd.setBlinkSpeed(blinkSlow);
  lcd.blinkSymbol(S_DELAY);
  delay(3000);
  lcd.noBlinkSymbol(S_DELAY);

  lcd.setSymbol(S_SW_SELECT);  
  lcd.setSymbol(S_PERCENT);  

  lcd.writeCustomChar14(0, 0xFFFF, true);
  lcd.writeCustomChar14(1, 0xFFFF, true);
  lcd.writeCustomChar14(2, 0xFFFF, true);
  lcd.writeCustomChar14(3, 0xFFFF, true);

  lcd.writeChar7(0, '8');
  lcd.writeChar7(1, '8');

delay(5000);
lcd.setBlinkSpeed(blinkFast);
lcd.blinkDisplay();
delay(5000);
lcd.blinkDisplayStop();


}

void loop()
{ 

}