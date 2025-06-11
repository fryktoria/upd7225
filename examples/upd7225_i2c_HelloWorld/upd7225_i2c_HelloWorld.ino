
// Include the library code
#include "upd7225.h"

#define PORT_EXTENDER_I2C_ADDRESS 0x21

// Initialize the library with the interface pins of the port extender
UPD7225 lcd(PORT_EXTENDER_I2C_ADDRESS, _EXT_SCK, _EXT_SI, _EXT_CD, _EXT_RESET, _EXT_CS, _EXT_BUSY);



void setup()
{
  Serial.begin(115200);
  // Set up the LCD
  lcd.begin();
  lcd.clearDisplay();

  lcd.setSymbol(S_DELAY);
  lcd.setSymbol(S_INPUT_SEL);
  lcd.setSymbol(S_ON);
  lcd.setSymbol(S_OFF);
  lcd.setSymbol(S_SLV);
  lcd.setSymbol(S_MAS);
  lcd.setSymbol(S_MODEL);
  lcd.setSymbol(S_NAME);
  lcd.setSymbol(S_LOCK);
  lcd.setSymbol(S_CH);
  lcd.setSymbol(S_R);
  lcd.setSymbol(S_N);
  lcd.setSymbol(S_1);
  lcd.setSymbol(S_2);
  lcd.setSymbol(S_3);
  lcd.setSymbol(S_4);
  lcd.setSymbol(S_5);
  lcd.setSymbol(S_6);
  lcd.setSymbol(S_7);
  lcd.setSymbol(S_8);
  lcd.setSymbol(S_ARROW_OVER_1);
  lcd.setSymbol(S_ARROW_OVER_2); 
  lcd.setSymbol(S_ARROW_OVER_3);
  lcd.setSymbol(S_ARROW_OVER_4);
  lcd.setSymbol(S_ARROW_OVER_5);
  lcd.setSymbol(S_ARROW_OVER_6);
  lcd.setSymbol(S_ARROW_OVER_7);
  lcd.setSymbol(S_ARROW_OVER_8);
  lcd.setSymbol(S_ARROW_UNDER_1);
  lcd.setSymbol(S_ARROW_UNDER_2);
  lcd.setSymbol(S_ARROW_UNDER_3);
  lcd.setSymbol(S_ARROW_UNDER_4);
  lcd.setSymbol(S_ARROW_UNDER_5);
  lcd.setSymbol(S_ARROW_UNDER_6);
  lcd.setSymbol(S_ARROW_UNDER_7);
  lcd.setSymbol(S_ARROW_UNDER_8);
  lcd.setSymbol(S_SWASH);
  lcd.setSymbol(S_WING);
  lcd.setSymbol(S_LU);
  lcd.setSymbol(S_RD);
  lcd.setSymbol(S_COLON);
  lcd.setSymbol(S_HOR_CROSS);
  lcd.setSymbol(S_VERT_CROSS);
  lcd.setSymbol(S_I);
  lcd.setSymbol(S_NOR); 
  lcd.setSymbol(S_ST3);
  lcd.setSymbol(S_ST2);
  lcd.setSymbol(S_ST1);
  //lcd.setSymbol(S_CROSS);
  lcd.setSymbol(S_SW_SELECT);  
  lcd.setSymbol(S_PERCENT);  

  lcd.print14("Helo.");

  for (int i=-100; i <= 100 ; i++) {
    lcd.printNumber7(i, false, true, true);
  }

}

void loop()
{ 


}