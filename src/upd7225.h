/*
  This is a library specially designed to operate the display 
  of the ECLIPSE7 RC controller, after the extensive mod.
  The display was found to be in a working state, so I
  decided to re-use it.
  The libary is based partly on the following library
     KTM-S1201 Arduino library                                     
     Developed and maintanied by MCUdude
     https://github.com/MCUdude/KTMS1201

  which in turn was based on Tronixstruff's work    
  tronixstuff.com/2013/03/11/arduino-and-ktm-s1201-lcd-modules   

  The ECLIPSE7 display module has the following features:
    - Presents symbols
    - Has an area where it can display four 14-segment characters
    - Has an area where it can display two 7-segment characters

  The operation is controlled by the data memory of upd7225 chip
  which is composed of 32 4-bit locations.
  Symbols have a static association between the memory location and the value.
  For example, to turn on symbol DELAY, 0x01 needs to be written in 
  data memory location 22.
  The value in the memory location is assigned bit-by-bit to a symbol.
  For example, 0x01 turns on DELAY, 0x02 turns on INPUT_SEL etc.
  This way, by OR-ing the value writen on the register, you 
  can turn on (or off by AND-ing) symbols independently.

  Each of the four characters of the 14-segment area occupy 4 registers.
  Starting from the leftmost, it uses registers 18-21, second char 14-17,
  third char 10-13, fourth char 6-9.
  The first two registers contain the low byte of the 4-byte character encoding
  and the next two the high byte. For example, character '0' is encoded as 0x47E2.
  Registers 18 and 19 contain 0xE2, registers 20 and 21 contain 0x47.
  Each of those characters has a decimal point that can be activated by 
  adding 8 to the high byte.

  Each of the two characters of the 7-segment area occupies 2 registers.
  The leftmost occupies registers 2 and 3. The rightmost occupies 
  registers 0 and 1. Numeric and alphanumeric characters can be written
  using the mapping table.
  These characters do not have a decimal point. Actually,
  the decimal point of the leftmost character turns on the symbol 'S/W SELECT'
  and the decimal point of the rightmost character is the '%' sign.






*/

#include "Arduino.h"

#ifndef UPD7225_H
#define UPD7225_H

//      ---   0x01
//0x10 |   |  0x02
//0x20  ---  
//0x40 |   |  0x04
//0x80  --- o 0x08 

/* LCD characters */
static const uint8_t numberOfCharacters7 = 67;
static const uint8_t sevenSegHex[67] = {0xD7/*0*/, 0x06/*1*/, 0xE3/*2*/, 0xA7/*3*/, 0x36/*4*/, 0xB5/*5*/, 0xF4/*6*/, 0x07/*7*/, 0xF7/*8*/, 0x37/*9*/,  
0x00/*space*/, 0x20/*Dash*/, 0x80/*Underscore*/, 0x33/*Degree*/, 0xA0/*equal*/, 
0x77/*A*/, 0xF4/*B*/, 0xE0/*C*/, 0xE6/*D*/, 0xF1/*E*/, 0x71/*F*/, 0xD5/*G*/, 0x74/*H*/, 
0x04/*I*/, 0x86/*J*/, 0x70/*K*/, 0xD0/*L*/, 0x76/*M*/, 0x64/*N*/, 0xE4/*O*/, 0x73/*P*/, 
0xDF/*Q*/, 0x60/*R*/, 0xB5/*S*/, 0xF0/*T*/, 0xC4/*U*/, 0xD6/*V*/, 0xF6/*W*/, 0x56/*X*/, 
0xB6/*Y*/, 0xEB/*Z*/, 0x77/*a*/, 0xF4/*b*/, 0xE0/*c*/, 0xE6/*d*/, 0xF1/*e*/, 0x71/*f*/, 
0xD5/*g*/, 0x74/*h*/, 0x04/*i*/, 0x86/*j*/, 0x70/*k*/, 0xD0/*l*/, 0x76/*m*/, 0x64/*n*/, 
0xE4/*o*/, 0x73/*p*/, 0xDF/*q*/, 0x60/*r*/, 0xB5/*s*/, 0xF0/*t*/, 0xC4/*u*/, 0xD6/*v*/, 
0xF6/*w*/, 0x56/*x*/, 0xB6/*y*/, 0xEB/*z*/};
static const uint8_t charArray7[67] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  
' ', '-', '_', '*', '=', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 
'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 
'x', 'y', 'z'};

static const uint8_t numberOfCharacters14 = 48;
static const uint8_t charArray14[48] = {' ', '`','(',')','+','-','/',
'0','1','2','3','4','5','6','7','8','9','<','=','>',
'@','A','B','C','D','E','F','G','H','I','J','K',
'L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','\\'
};

static const uint16_t fourteenSegHex[48] = {0x0000, 0x0002,0x000A,0x5000,0xA005,0x2004,0x4002,
0x47E2,0x0600,0x23C4,0x2784,0x2624,0x25A4,0x25E4,0x0700,0x27E4,0x27A4,0x4082,0x2084,0x1088,
0xA7C0,0x2764,0x8785,0x01E0,0x8781,0x21E4,0x2164,0x05E4,0x2664,0x8181,0x06C0,0x206A,
0x00E0,0x1662,0x1668,0x07E0,0x2364,0x07E8,0x236C,0x1584,0x8101,0x06E0,0x4062,0x4668,0x500A,0x9002,0x4182,0x1008
};

/* LCD commands */
const uint8_t _Mode = 0x40;     // LCD mode. See uPD7225 datasheet for more infp
const uint8_t _Sync = 0x31;     // Synchronized transfer
const uint8_t _USync = 0x30;    // Unsynchronized transfer
const uint8_t _FBlink = 0x1b;   // Blink fast (~2 Hz)

const uint8_t _SBlink = 0x1a;   // Blink slow (~1 Hz) 
const uint8_t _NoBlink = 0x18;  // No blink
const uint8_t _DispOn = 0x11;   // Display on
const uint8_t _DispOff = 0x10;  // Display off
const uint8_t _Decode = 0x15;   // Turn on segment decoding
const uint8_t _NoDecode = 0x14; // Turn off segment decoding
const uint8_t _LoadPtr = 0xE0;  // Load data pointer The data memory location (32 x4) is added to 0xE0, e.g to write to memory location 2, send command 0xE2
const uint8_t _ClearDsp = 0x20; // Clear data memory

const uint8_t _AndData = 0x90;  // Performs an AND of the data at current pointer location
const uint8_t _OrData = 0xB0;   // Performs an OR of the data at current pointer location

const uint8_t _ClearBlinkMemory = 0x00; // Clear blinking memory
const uint8_t _Blink = 0xC0;    // stores immediate data D3-D0 to the blinking data memory addressed by the data pointer
const uint8_t _OrBlink = 0xA0;   // OR blinking memory
const uint8_t _AndBlink = 0x80;   // OR blinking memory


const int blinkNone = 0;
const int blinkFast = 1;
const int blinkSlow = 2;

const uint8_t clockHalfPulseWidth = 10; // The half period of the clock signal in micro seconds
const uint8_t busyDelay = 2000; // Wait time in micro seconds, in case the BUSY signal of the display is not read.



/*
  Display ECLIPSE7 symbols
  Encoded in two bytes.
  The first byte is the register location
  The second byte is the register value associated with the symbol
*/

#define S_DELAY     0x1601
#define S_INPUT_SEL 0x1602
#define S_ON        0x1604
#define S_OFF       0x1608
#define S_SLV       0x1701
#define S_MAS       0x1702
#define S_MODEL     0x1704
#define S_NAME      0x1708
#define S_LOCK      0x1801
#define S_CH        0x1802
#define S_R         0x1804
#define S_N         0x1808
#define S_1         0x1901
#define S_2         0x1902
#define S_3         0x1904
#define S_4         0x1908
#define S_5         0x1A01
#define S_6         0x1A02
#define S_7         0x1A04
#define S_8         0x1A08
#define S_ARROW_OVER_1 0x1B01
#define S_ARROW_OVER_2 0x1B02 
#define S_ARROW_OVER_3 0x1B04
#define S_ARROW_OVER_4 0x1B08
#define S_ARROW_OVER_5 0x1C01
#define S_ARROW_OVER_6 0x1C02
#define S_ARROW_OVER_7 0x1C04
#define S_ARROW_OVER_8 0x1C08
#define S_ARROW_UNDER_1 0x1D01
#define S_ARROW_UNDER_2 0x1D02
#define S_ARROW_UNDER_3 0x1D04
#define S_ARROW_UNDER_4 0x1D08
#define S_ARROW_UNDER_5 0x1E01
#define S_ARROW_UNDER_6 0x1E02
#define S_ARROW_UNDER_7 0x1E04
#define S_ARROW_UNDER_8 0x1E08
#define S_SWASH         0x1F01
#define S_WING          0x1F02
#define S_LU            0x1F04
#define S_RD            0x1F08
#define S_COLON         0x0401 // Between the 14-segment characters 2 and 3
#define S_HOR_CROSS     0x0402 // The two horizontal bars making the cross
#define S_VERT_CROSS    0x0404 // The two vertical bars making the cross
#define S_I             0x0408 // The symbol before the two 7-segments, interpreted as I or 1
#define S_NOR           0x0501 
#define S_ST3           0x0502
#define S_ST2           0x0504
#define S_ST1           0x0508
#define S_CROSS         0x0406 // Creates a cross by combining S_HOR_CROSS and S_VERT_CROSS

// The SW_SELECT and the PERCENT sign are the decimal points
// of the two 7-segment digits and are not handled like the other symbols
#define S_SW_SELECT     0xFFFE  
#define S_PERCENT       0xFFFF                               

//The assignment of PCF8574 remote extender pins,
// if this extender is used. This is defined by the 
// wiring of the board. This specific order was decided because of the 
// order at the existing connector of the display module of ECLIPSE7
// Class contrsuctor may override these values
#define _EXT_RESET 0x01 // Output
#define _EXT_CD    0x02 // Output
#define _EXT_BUSY  0x04 // Input
#define _EXT_CS    0x08 // Output
#define _EXT_SI    0x10 // Output
#define _EXT_SCK   0x20 // Output
#define _EXT_UNUSED1 0x40 // Input
#define _EXT_UNUSED2 0x80 // Input

class UPD7225
{
  public:
    /* Public methods */
    UPD7225(uint8_t NSCK, uint8_t SI, uint8_t CD, uint8_t RESET, uint8_t CS, uint8_t BUSY);
    // When a remote extender PCF8574 is used.
    // In that case, pin assignment is wired but can be changed by the user.
    UPD7225(uint8_t i2cAddress, uint8_t NSCK, uint8_t SI, uint8_t CD, uint8_t RESET, uint8_t CS, uint8_t BUSY);

    void begin(void);
    void hardReset(void); // Hardware reset the LCD using the RESET pin
    void startSession(void);
    void endSession(void);
    void command(uint8_t); //Send a command within a session
    void customChar(uint8_t);
    void setSymbol(uint16_t symbol); // Turns on a symbol from the symbol library
    void clearSymbol(uint16_t symbol); // Turns off a symbol
    void blinkSymbol(uint16_t symbol); // Blinks an individual symbol, without interfering with other symbols. setBlinkSpeed() should have been set to blinkFast or blinkSlow for the blinking to take effect
    void noBlinkSymbol(uint16_t symbol); // Stops the blinking of an individual symbol

    void clearDisplay(void); // Clears the entire display
    void setBlinkSpeed(int speed = blinkSlow); // Sets the blinking speed to slow, fast or no blink.
    void blinkDisplay(void); // Blinks the entire display (whatever has been written and whatever will be written until blinking is turned off)
    void blinkDisplayStop(void); // Stops blinking the entire display. There is no memory kept of symbols that were blinking individually
                             // If you want to stop blinking preserving what was already blinking, just call setBlinkSpeed(blinkNone). 
                             // You will be able to activate blinking againg by calling setBlinkSpeed(blinkSlow) or setBlinkSpeed(blinkFast)

    void displayOn(void); // Makes the display visible
    void displayOff(void); // Makes the display non visible

    // Writes an 8-bit code to data memory position of pointer and pointer+1
    void writeDataMemory(uint8_t pointer, uint8_t code); 
    // Write to the 7-segment characters (cursor can be 0 or 1)   
    void writeChar7(uint8_t cursor, uint8_t character);
    // Prints a string to the 7-segment characters
    void print7(String);
    // Write to the 14-segment characters a character code, based on the encoding of the chip - see upd7225 manual (cursor can be 0 to 3)
    void writeCustomChar14(uint8_t cursor, uint16_t charCode, bool dp = false);
    // Write a character at position of cursor, with optional decimal point
    void writeChar14(uint8_t cursor, uint8_t character, bool dp = false);
    // Prints a string to the 14-segment characters. Decimal points can be included in the string for each of the characters.
    // Characters are printed as uppercase.
    void print14(String);
    // Prints a number between -100 to +100 to the 7-seg digits
    // plus the sign, and the 1. It can also add the percentage sign.
    void printNumber7(int value, bool withPercent = false, bool leadingZero = false, bool plusOnPositive = true);

 
  private: 
    void write(uint8_t);   
    void wait(void);
    // digitalRead() and digitalWrite() re-implemented to allow 
    // future implementation using a port extender
    void setPin(uint8_t pin, uint8_t state);
    int readPin(uint8_t pin);

    /* Private variables */
    uint8_t _cursorPos14 = 0;
    uint8_t _cursorPos7 = 0;

    /* Digital pins */
    uint8_t _SCK = 0;
    uint8_t _SI = 0;
    uint8_t _CD = 0;
    uint8_t _RESET = 0;
    uint8_t _BUSY = 255;
    uint8_t _CS = 0;
    
    // A variable to control how setPin() and readPin() works in this class instance, either with extender or not
    bool _withI2cExtender = false;
    // A variable to remember what has been written to the PCF8574 extender,
    // when we use an extender via I2C to control the display
    uint8_t _pcf8574OutputPinState = 0;
    // The I2C address of the PCF8574
    uint8_t _I2C_Device_Address = 0;    
    
    
};

#endif    