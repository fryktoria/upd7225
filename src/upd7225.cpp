#include "Arduino.h"
#include "upd7225.h"

// Requires the Wire library when an I2C extender is used
#include <Wire.h>
/*******************************
******** PUBLIC METHODS ********
*******************************/

// Constructor with direct connection of display pins to Arduino pins.
// If BUSY pin is not used, set parameter BUSY=0
UPD7225::UPD7225(uint8_t NSCK, uint8_t SI, uint8_t CD, uint8_t RESET, uint8_t CS, uint8_t BUSY)
{
	// Store parameters
  _SCK = NSCK;
  _SI = SI;
  _CD = CD;
  _RESET = RESET;
  _BUSY = BUSY;
  _CS = CS;
  
  //Set inputs and outputs
  pinMode(_SCK, OUTPUT);
  pinMode(_SI, OUTPUT);
  pinMode(_CD, OUTPUT);
  pinMode(_RESET, OUTPUT);
  pinMode(_BUSY, INPUT);
  pinMode(_CS, OUTPUT);
if (BUSY != 0) {
    pinMode(_BUSY, INPUT);    
  }  
  
  // Set the flag the remember that we operate without extender
  _withI2cExtender = false;                                                               
}


// Constructor with I2C
// If BUSY pin is not used, set parameter BUSY=0
UPD7225::UPD7225(uint8_t i2cAddress, uint8_t NSCK, uint8_t SI, uint8_t CD, uint8_t RESET, uint8_t CS, uint8_t BUSY)
{
  // Assign the pins numbers to the private pin variables
  // to operate similarly with and without extender
  _SCK = NSCK;
  _SI = SI;
  _CD = CD;
  _RESET = RESET;
  _BUSY = BUSY;
  _CS = CS;

  _I2C_Device_Address = i2cAddress;
  // Set the flag the remember that we operate with extender
  _withI2cExtender = true;
}                       


void UPD7225::begin(void)
{
  if (_withI2cExtender) {  
    Wire.begin();
    if (_BUSY != 0)
      setPin(_BUSY, HIGH); // Use as input, set to input pullup
  }                                                               
  setPin(_SCK, HIGH);
  setPin(_SI,  HIGH);
  setPin(_CD, HIGH);   
  setPin(_CS, HIGH);   // Deselect upd7225
  hardReset();

  // Setup command mode. See header for more info
  startSession();
  command(_Mode);
  command(_Sync);
  command(_NoBlink); 
  command(_NoDecode); 
  command(_ClearDsp);
  // Clears the Blinking Data Memory, so that only what we order will blink
  command(_ClearBlinkMemory); 
  command(_DispOn);
  endSession();
  
  // Set general blink flag, but only those which have 1 in blinking data memory will blink 
  setBlinkSpeed(blinkSlow);    
}


void UPD7225::hardReset(void)
{
  setPin(_RESET, LOW); // Reset LCD
  delay(1000);                 // Wait for the LCD to finish
  setPin(_RESET, HIGH);  
}


void UPD7225::startSession(void)
{
    setPin(_CS, LOW);    
}


void UPD7225::endSession(void)
{
    wait();
    setPin(_CS, HIGH);    
}


void UPD7225::command(uint8_t cmd)
{
  setPin(_CD, HIGH);
  wait();
  write(cmd);
  wait();

}


void UPD7225::setSymbol(uint16_t symbol) 
{
  startSession();
  if (symbol == S_SW_SELECT) {
    command(_LoadPtr + 2);
    command(_OrData + 0x08);
  } else if (symbol == S_PERCENT) {
    command(_LoadPtr + 0);
    command(_OrData + 0x08);
  } else {
    uint8_t location = symbol >> 8;
    uint8_t value = symbol & 0x0F;
    command(_LoadPtr + location);
    command(_OrData + value);
  }
  endSession();
}


void UPD7225::clearSymbol(uint16_t symbol) 
{
  startSession();
  if (symbol == S_SW_SELECT) {
    command(_LoadPtr + 2);
    command(_AndData + 0x7);
  } else if (symbol == S_PERCENT) {
    command(_LoadPtr + 0);
    command(_AndData + 0x7);
  } else {
    uint8_t location = symbol >> 8;
    uint8_t value = symbol & 0x0F;
    command(_LoadPtr + location);
    command(_AndData + (~value & 0x0F));
  }
  endSession();  
}

// The symbol must have been made visible with setSymbol() in order to see it blinking,
// because this command operates only at the blinking data memory and not at the data memory.
void UPD7225::blinkSymbol(uint16_t symbol) 
{
  uint8_t location = symbol >> 8;
  uint8_t value = symbol & 0x0F;
  startSession();
  command(_LoadPtr + location);
  command(_OrBlink + value);
  endSession();
}


void UPD7225::noBlinkSymbol(uint16_t symbol)
{
  uint8_t location = symbol >> 8;
  uint8_t value = symbol & 0x0F;
  startSession();
  command(_LoadPtr + location);
  command(_AndBlink + (~value & 0x0F));
  endSession();

}


void UPD7225::clearDisplay(void)
{
  startSession();
  command(_ClearDsp);
  command(_ClearBlinkMemory); 
  endSession();
}

// The blink speed is one, either if the blinking refers to the entire display or individual symbols.
// It is not possible to blink sumbols at a different speed.
void UPD7225::setBlinkSpeed(int speed)
{	
  startSession();	
	if (speed == blinkFast) { // Blink fast
		command(_FBlink);
  } else if (speed == blinkSlow) {// Blink slow	
		command(_SBlink);
  } else {
    command(_NoBlink);
  }  
  endSession();
}

// This command operates only at the blinking data memory,
// by making any item in the data memory to be subject to blinking.
// The blinking speed must have been set beforehand with setBlinkSpeed() to fast or slow. 
void UPD7225::blinkDisplay(void)
{
  startSession();
  for (int i = 0; i < 32; i++) {   
    // Starting the session sets data pointer to 0
    // and each command increases by 1, therefore, I do not need to set manually
	  command(_Blink + 0x0F);
  }
  endSession();  
}


void UPD7225::blinkDisplayStop(void)
{
  startSession();
	command(_ClearBlinkMemory);
  endSession();  
}
    
    
void UPD7225::displayOn()
{	
  startSession();
	command(_DispOn);
  endSession();
}


void UPD7225::displayOff()
{	
  startSession();
	command(_DispOff);
  endSession();
}


// Writes one byte, 4bits at pointer location and 4bits at pointer location + 1
void UPD7225::writeDataMemory(uint8_t pointer, uint8_t code) 
{
  startSession();
  setPin(_CD, HIGH);
  write(_LoadPtr + pointer);
  setPin(_CD, LOW);    //Put in data mode  
  write(code);
  wait(); 
  endSession();

}

// Writes a character at cursor position of the 7-segment display area, using the 7-segment encodind
void UPD7225::writeChar7(uint8_t cursor, uint8_t character)
{
  // Only two characters support 7-segment printing
  if (cursor > 1)
    return;

  uint8_t memoryLocation = 2 - cursor * 2;

  startSession();
  for (uint8_t i = 0; i < numberOfCharacters7; i++) {
    if(character == charArray7[i]) {
      // OBSOLETE
      // Very simple, but does not preserve the status
      // of the SW_SELECT and % symbols
      //writeDataMemory(memoryLocation, sevenSegHex[i]);
      
      // This is more complex but preserves the symbols,
      // in case we wanted to change the number but not the symbol
      
      // First delete whatever is in the memory, except the "decimal point" (SW_SELECT and % symbols)      
      command(_LoadPtr + memoryLocation);
      command(_AndData + 0x08);   // Turn off low nimble except the decimal point
      command(_LoadPtr + memoryLocation + 1);
      command(_AndData + 0x00);    // Turn off high nimble
         
      //...and write with _OrData what we want, preserving the "decimal point"
      command(_LoadPtr + memoryLocation);
      command(_OrData + (sevenSegHex[i] & 0x0F));
      command(_LoadPtr + memoryLocation + 1);
      command(_OrData + (sevenSegHex[i] >> 4));     
    }
  }
  endSession();
}


// Prints a string of max two characters at the 7-segment display area
void UPD7225::print7(String str)
{
  // Delete whatever was written in both
  writeChar7(0, ' ');
  writeChar7(1, ' ');

  for (uint8_t cursor = 0; cursor < str.length() && cursor < 2; cursor++){
    writeChar7(cursor, str.charAt(cursor));
  }
}


// Writes a 14-segment encoded value at cursor position of the 14-segment display area
void UPD7225::writeCustomChar14(uint8_t cursor, uint16_t charCode, bool dp = false)
{
  // Only 4 characters support 14-segment printing
  if (cursor > 4)
    return;

  uint8_t memoryLocation = 18 - cursor * 4;

  startSession();
  setPin(_CD, HIGH);

  wait();
  write(_LoadPtr + memoryLocation);

  uint8_t bHigh = charCode >> 8;
  uint8_t bLow = charCode & 0xFF;

  if(dp) {
    bHigh |= 0b1000;
  }  

  setPin(_CD, LOW);    //Put in data mode
  wait(); // Wait for the LCD to finish
  write(bLow);
  wait();
  write(bHigh);
  wait();
  endSession();

}


// Writes a character at cursor position of the 14-segment display area
void UPD7225::writeChar14(uint8_t cursor, uint8_t character, bool dp = false)
{
  for (uint8_t i=0; i < numberOfCharacters14; i++) {
    if(character == charArray14[i]) {
      writeCustomChar14(cursor, fourteenSegHex[i], dp);  
    }
  }
}


// Prints a string of max 4 characters at the 14-segment display area. Any decimal
// point is not counted in the 4 char limit, as the decimal point is integrated in the 
// 14-segment encoding. For example, "A.B.C.D." is a valid 4 character string
void UPD7225::print14(String str)
{
  str.toUpperCase();
  int index = 0;
  uint8_t cursor; // I define it outside for() because I want to use its value after the for()
  for (cursor = 0; index < str.length() && cursor < 4; cursor++) {
    if (cursor == 0 && str.charAt(0) == '.') {
      writeCustomChar14(index, 0x0000, true);
      index++;
    }  
    else {
      if ((index + 1) < str.length() && str.charAt(index + 1) == '.')  {
        writeChar14(cursor, str.charAt(index), true);
        index++;
        index++;
      }  
      else {
        writeChar14(cursor, str.charAt(index), false);
        index++;
      }  
    }
  }
  // Complete the remaining characters with spaces, to delete any previously written character
  for(;cursor < 4; cursor++) {
    writeCustomChar14(cursor, 0x0000, false); 
  }
}


/*
  Prints a number between -100 to +100 to the 7-seg digits
  plus the sign, and the 1. It can also add the percentage sign.
  It does not preserve neither an existing percent sign,
  nor a SW_SELECT symbol. Although the % sign is manipulated 
  by this function, the SW_SELECT symbol must be set explicitly by 
  setSymbol() by the user after the call to printNumber7()
*/
void UPD7225::printNumber7(int value, bool withPercent, bool leadingZero, bool plusOnPositive)
{
  clearSymbol(S_I);
  clearSymbol(S_VERT_CROSS);
  clearSymbol(S_HOR_CROSS);

  if (value < -100 || value > 100)
    return;
  if (value == -100 || value == 100)
    setSymbol(S_I);

  if (value < 0) {
    setSymbol(S_HOR_CROSS);  
    value = -value; // keep only two digits
  } else {
    plusOnPositive ? setSymbol(S_CROSS) : clearSymbol(S_CROSS);    
  } 
  if (value == 100) {
    writeChar7(0, '0');
    writeChar7(1, '0');
  } else {
    if (value <= 9) {
      leadingZero ? writeChar7(0, '0') : writeChar7(0, ' ');
      writeChar7(1, charArray7[value]);
    } else {
      writeChar7(0, charArray7[value/10])  ;
      writeChar7(1, charArray7[value - value / 10 * 10]);
    }
  }
  withPercent ? setSymbol(S_PERCENT) : clearSymbol(S_PERCENT);
}


void UPD7225::write(uint8_t character)
{
  // The SCK should be set to high before serial data is input and after the data has been input (after eight clocks are input to SCK).
  setPin(_SCK, HIGH);
  delayMicroseconds(clockHalfPulseWidth);
  for(uint8_t i = 0; i < 8; i++) 
  {
    /*
    The double exclamation marks !! convert 
    the result of the bitwise AND operation to a boolean value. 
    Essentially, it ensures the result is either 0 (false) or 1 (true). 
    If the bit at position (7-i) is set (i.e., 1), the result is 1. Otherwise, it's 0.
    */

    // Characters are input to upd7225 MSB first
    setPin(_SI, !!(character & (1 << (7-i))));
    delayMicroseconds(clockHalfPulseWidth);
    setPin(_SCK,LOW);
    delayMicroseconds(clockHalfPulseWidth);
    setPin(_SCK, HIGH);    
  }
  wait();
  
}


void UPD7225::wait()
{
	if(_BUSY == 255)
		delayMicroseconds(busyDelay);
	else
		while(readPin(_BUSY) == 0);
}


void UPD7225::setPin(uint8_t pin, uint8_t newState)
{
  if (_withI2cExtender) {
    Wire.beginTransmission((int)_I2C_Device_Address);  
    if (newState == LOW) {
      _pcf8574OutputPinState &= ~pin;      
    }  else {
      _pcf8574OutputPinState |= pin; 
    } 
    Wire.write(_pcf8574OutputPinState);
    Wire.endTransmission();  
  } else {
    digitalWrite(pin, newState);
  }
}


int UPD7225::readPin(uint8_t pin)
{ 
  if (_withI2cExtender) {
    Wire.requestFrom((int)_I2C_Device_Address, 1);
    if (Wire.available()) {
      return ( !!(Wire.read() & pin));
    } else {
      return HIGH;
    }
  } else { 
    return(digitalRead(pin));
  }
}