

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
  Serial.begin(115200);
  // Set up the LCD
  lcd.begin();
  lcd.clearDisplay();
  lcd.print14("Helo");

}

void loop()
{ 
  // Prints a number to the 7-segment area
  Serial.println("Input a number between -100 and +100");
  while (!Serial.available()) {}
  if (Serial.available() > 0) { 
    String inputString = Serial.readStringUntil('\n'); // Read the input string until newline character
    int number = inputString.toInt(); // Convert the string to an integer
    lcd.printNumber7(number);
  }

  //Prints a string to the 14-segment area
  Serial.println("Input a string of characters");
  while (!Serial.available()) {}
  if (Serial.available() > 0) { 
    String inputString = Serial.readStringUntil('\n'); // Read the input string until newline character
    lcd.print14(inputString);
  }

}