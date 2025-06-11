

// Include the library code
#include "upd7225.h"

// Pin definitions
byte N_SCK = 3;
byte SI = 4;
byte CD = 5;
byte RESET = 6;
byte BUSY = 7;
byte CS = 8;


UPD7225 lcd(N_SCK, SI, CD, RESET, CS, BUSY);

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