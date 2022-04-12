//*********************User Display Code for Perculator Arduino Drum Machine**********************/
// OLED display and LED interface code by Jupertronic aka Janis Wilson Hughes aka J Dub aka vitalWho aka Evolution Stoneware (youtube)
// OLED shows what pattern you're playing with pattern number and pattern name as selected by the pattern potentiometer.
// Pins for OLED are dependent on type of Arduino used. This is set up for Nano. If you're using Pro Micro SDA = D2, SCL = D3. Check your SDA & SCL and adjust accordingly.
// This is designed for Mark Dammer's Wee O3 version of Jan Ostman's O2 Mini Pops project revived by Bloghoskins. Sounds emulate Korg Mini Pops drum machine.
// See my Perculator code for extensive changes to the pattern beats corresponding to the pattern names here and minor additions for LEDs.
// LEDs are run off this board to remove potential sound interference on main Perculator board since we have this board available to handle the OLED screen.
// You will need to install the ss_oled library to run the display.
// Change the pattern names as desired in the code. These are promarily 4 on the floor 80s dance beats for Perculator. Boot n' pants is beat box slang. 

#include <ss_oled.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

// Standard Arduino Pins

#define digitalPinToPortReg(P) \
  (((P) >= 0 && (P) <= 7) ? &PORTD : (((P) >= 8 && (P) <= 13) ? &PORTB : &PORTC))
#define digitalPinToDDRReg(P) \
  (((P) >= 0 && (P) <= 7) ? &DDRD : (((P) >= 8 && (P) <= 13) ? &DDRB : &DDRC))
#define digitalPinToPINReg(P) \
  (((P) >= 0 && (P) <= 7) ? &PIND : (((P) >= 8 && (P) <= 13) ? &PINB : &PINC))
#define digitalPinToBit(P) \
  (((P) >= 0 && (P) <= 7) ? (P) : (((P) >= 8 && (P) <= 13) ? (P) - 8 : (P) - 14))


#define digitalReadFast(P) bitRead(*digitalPinToPINReg(P), digitalPinToBit(P))

#define digitalWriteFast(P, V) bitWrite(*digitalPinToPortReg(P), digitalPinToBit(P), (V))

// if your system doesn't have enough RAM for a back buffer, comment out
// this line (e.g. ATtiny85)
#define USE_BACKBUFFER

#ifdef USE_BACKBUFFER
static uint8_t ucBackBuffer[1024];
#else
static uint8_t *ucBackBuffer = NULL;
#endif

// Use -1 for the Wire library default pins
// or specify the pin numbers to use with the Wire library or bit banging on any GPIO pins
// These are the pin numbers for the M5Stack Atom default I2C
#define SDA_PIN 18
#define SCL_PIN 19
// Set this to -1 to disable or the GPIO pin number connected to the reset
// line of your display if it requires an external reset
#define RESET_PIN -1
// let ss_oled figure out the display address
#define OLED_ADDR -1
// don't rotate the display
#define FLIP180 0
// don't invert the display
#define INVERT 0
// Bit-Bang the I2C bus
#define USE_HW_I2C 0

// Change these if you're using a different OLED display
#define MY_OLED OLED_128x64
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
//#define MY_OLED OLED_64x32
//#define OLED_WIDTH 64
//#define OLED_HEIGHT 32

SSOLED ssoled;

int count = 0;

// Output Pins
#define patternStartLED 7
#define stepLED 6
#define patternEndLED 5


//Input Pins
#define patternPot A0
#define patternStartPulse 4
#define stepPulse 3
#define patternEndPulse 2



void setup() {
  //  Serial.begin(9600);
  pinMode(patternStartLED, OUTPUT);
  pinMode(patternEndLED, OUTPUT);
  pinMode(stepLED, OUTPUT);
  pinMode(patternStartPulse, INPUT);
  pinMode(patternEndPulse, INPUT);
  pinMode(stepPulse, INPUT);
  pinMode(patternPot, INPUT);

  //Initialize OLED display
  int rc;
  // The I2C SDA/SCL pins set to -1 means to use the default Wire library
  // If pins were specified, they would be bit-banged in software
  // This isn't inferior to hw I2C and in fact allows you to go faster on certain CPUs
  // The reset pin is optional and I've only seen it needed on larger OLEDs (2.4")
  //    that can be configured as either SPI or I2C
  //
  // oledInit(SSOLED *, type, oled_addr, rotate180, invert, bWire, SDA_PIN, SCL_PIN, RESET_PIN, speed)

  rc = oledInit(&ssoled, MY_OLED, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, 400000L); // use standard I2C bus at 400Khz


  oledFill(&ssoled, 1, 1);
  oledFill(&ssoled, 0, 1);
  //  oledDumpBuffer(&ssoled, NULL);

  oledWriteString(&ssoled, 0, 0, 1, (char *)"   Jupertronic       ", FONT_NORMAL, 1, 1);
  oledWriteString(&ssoled, 0, 0, 3, (char *)"Perc-", FONT_STRETCHED, 0, 1);
  oledWriteString(&ssoled, 0, 0, 6, (char *)" ulator", FONT_STRETCHED, 0, 1);

  delay(1000);

  oledFill(&ssoled, 0, 1);
  oledDumpBuffer(&ssoled, NULL);

  oledWriteString(&ssoled, 0, 0, 0, (char *)"   Jupertronic       ", FONT_NORMAL, 1, 1);
  oledWriteString(&ssoled, 0, 5, 7, (char *)"   PERCULATOR   ", FONT_NORMAL, 1, 1);
  //  oledWriteString(&ssoled, 0, 40, 4, (char *)"BPM  ", FONT_STRETCHED, 0, 1);
}


String string_Pattern;
// Convert pattern number into a string
char string[3];  // Create a character array of 10 characters
// Convert float to a string:
//dtostrf(round_BPM, 3, 0, string);  // (<variable>,<amount of digits we are going to use>,<amount of decimal digits>,<string name>)


void loop() {
  update_LEDs();
  update_OLED();
}
/*********************************************************************/
void update_LEDs() {
  int patternEnd = digitalReadFast(patternEndPulse);
  int stepAdvance = digitalReadFast(stepPulse);
  int patternStart = digitalReadFast(patternStartPulse);
  if (patternStart == 1) {
    digitalWriteFast(patternStartLED, HIGH);
  } else {
    digitalWriteFast(patternStartLED, LOW);
  }
  if (patternEnd == 1) {
    digitalWriteFast(patternEndLED, HIGH);
  } else {
    digitalWriteFast(patternEndLED, LOW);
  }
  if (stepAdvance == 1) {
    digitalWriteFast(stepLED, HIGH);
  } else {
    digitalWriteFast(stepLED, LOW);
  }
}
/*********************************************************************/
void update_OLED () {
// Read pattern pot, depending on the value update the display with the current pattern selection
  int pattern = analogRead(patternPot);
  switch (pattern)
  {
    case 0 ... 63:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 01 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"  Boots n Pants    ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 00, 5, (char *)"       Hi          ", FONT_NORMAL, 0, 1);
      break;
    case 64 ... 127:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 02 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"  Boots n Pants   ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"      Low      ", FONT_NORMAL, 0, 1);
      break;
    case 128 ... 191:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 03 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"   Sizzle My   ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"      Pants     ", FONT_NORMAL, 0, 1);
      break;
    case 192 ... 255:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 04 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"     Send Me     ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"      Pants     ", FONT_NORMAL, 0, 1);
      break;
    case 256 ... 319:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 05 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"  I Wear Pants     ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"    at Night     ", FONT_NORMAL, 0, 1);
      break;
    case 320 ... 383:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 06 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"  Pants n Pants     ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"   Turn Around     ", FONT_NORMAL, 0, 1);
      break;
    case 384 ... 447:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 07 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"    Obsessed     ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"   with Pants     ", FONT_NORMAL, 0, 1);
      break;
    case 448 ... 511:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 08 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"   Straight Up     ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"    My Pants     ", FONT_NORMAL, 0, 1);
      break;
    case 512 ... 575:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 09 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"   Stand Back     ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"  Stevie Pants     ", FONT_NORMAL, 0, 1);      break;
    case 576 ... 639:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 10 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"  I Can't Pants    ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"      Fast       ", FONT_NORMAL, 0, 1);
      break;
    case 640 ... 703:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 11 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"  I Can't Pants   ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"      Slow       ", FONT_NORMAL, 0, 1);      
      break;
    case 704 ... 767:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 12 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"   Acid Washed     ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"      Pants     ", FONT_NORMAL, 0, 1);
      break;
    case 768 ... 831:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 13 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"  Stone Washed     ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"      Pants     ", FONT_NORMAL, 0, 1);
      break;
    case 832 ... 895:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 14 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"  Reggae Pants    ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"       Mon    ", FONT_NORMAL, 0, 1);
      break;
    case 896 ... 959:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 15 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"    New Wave     ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 0, 5, (char *)"      Pants     ", FONT_NORMAL, 0, 1); 
     break;
    case 960 ... 1024:
      oledWriteString(&ssoled, 0, 30, 2, (char *)" 16 ", FONT_STRETCHED, 0, 1);
      oledWriteString(&ssoled, 0, 0, 4, (char *)"     Dancie      ", FONT_NORMAL, 0, 1);
      oledWriteString(&ssoled, 0, 20, 5, (char *)"Fancy Pants     ", FONT_NORMAL, 0, 1);
      break;

  }

}
