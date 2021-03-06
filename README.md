# Jupertronic_Perculator_Mini_Pops
## Video Demo
Video Demo: https://www.youtube.com/watch?v=Q6Sxdc9tYrI

![Display](https://github.com/JuperTronic/Jupertronic_Perculator_Mini_Pops/blob/main/Perculator%20OLED%20Screen3.jpg)

## About
Perculator is essentially an OLED display addition to the Arduino based MiniPops drum machine originally designed by Jan Ostman. 
There are also _significant_ changes to the drum sequences to emulate 80s electro pop and dance beats.

![Perculator](https://github.com/JuperTronic/Jupertronic_Perculator_Mini_Pops/blob/main/Perculator.gif)
Lots of fabrication info available on my blog here: https://noisymaker.blogspot.com/2022/04/perculator-arduino-mini-pops-drum.html

*This project runs on 2 Arduinos connected through a common ground, the pattern pot, and a few pins*

Board 1 runs a sound generating drum machine with sounds emulating the Korg Mini Pops. This code, Perculator, is a modified version of The Wee O3 (Mark Dammer) expansion of O2 Mini Pops (Jan Ostman) code which has been popularized by Bloghoskins.
  - Minor changes made for sending pulsed outputs for Perculator's 3 LED design.
  - Extensive changes made to the beat patterns to produce 80s 4 on the floor dance beats
  - Switched to 8ths (1 and 2 and 3 and 4 and) instead of 16ths to work with timing input from my Skeeter sequence synth project

Board 2 runs original code operating an OLED screen showing the number and name of the drum pattern selected by the pattern pot and 3 LEDs which are userful for live play for visual cues showing the beats so you can syncronize your changes with the first/last beat of a sequence.
  - LED 1 blinks on the first note of the beat sequence
  - LED 2 blinks on each beat of the sequence
  - LED 3 blinks on the last note of the beat sequence

## Installation
Install Jupertronic_Perculator_O3Minipops.ino on board 1. *Include Perculator_data.h in the same sketch folder when you upload.

Install Jupertronic_Perculator_Display.ino on board 2. You will need to install the ss_oled.h library for the OLED.


## Wiring
Wire board 1 according to the schematics detailed by Mark Dammer for The Wee O3 https://github.com/mark-orion/The-Wee-O3 with changes shown in Perculator_Display_schem.pdf and Perculator_Display_diagram.jpg.
Also see Bloghoskins' detailed drawings for the original O2 Mini Pops. http://bloghoskins.blogspot.com/2016/11/korg-mini-pops-diy-drum-machine.html

Wire board 2 according to the Fritzing schematic Perculator_Display_schem.pdf and drawing Perculator_Display_diagram.jpg.
  - Connect grounds from board 1 and board 2 to a common ground point.
  - Run 2 wires from the middle signal lug of the pattern pot so that it can be connected as an input to both Arduino boards. 
  - Be sure the ground lug of pattern pot is connected to the common ground. 
  - Power the pattern pot only from board 1. 
  - Connect the other pins as shown in the schematic for inputs to board 2 from board 1 for timing pulses and the LEDs.
  - LEDs can be run off board 1 as in The Wee O3. They are split off to the extra capacity of board 2 here to remove possible audio coloration on board 1.
  - Suggest green LED for first beat, blue LED for each beat, and red LED for last beat. 
  - Suggest a smaller resistor on the green LED because the red and blue are brighter. This evens it out with my parts. Value determined by trial and error. 
  - My LEDs are very bright in the dark but good in a light room. Use larger resistors to dim the LEDs.
  - You could put diodes on the connections between board 1 and 2 if desired.

## Pattern Names
Change the pattern names as desired in the display code. These Perculator beats are promarily 4 on the floor 80s dance beat sequences. 'Boots n' pants' is slang for beat box sounds. ;) 

## General

OLED display and LED interface code by Jupertronic aka Janis Wilson Hughes aka J Dub aka vitalWho aka Evolution Stoneware (youtube).
OLED shows what pattern you're playing with pattern number and pattern name so you can tell what the heck you've selected on the pattern potentiometer.
Pins for OLED are dependent on type of Arduino used. This is set up for Nano. If you're using Pro Micro SDA = D2, SCL = D3. Check your SDA & SCL and adjust accordingly.

## Parts Links
My Actual Parts:
OLED https://amzn.to/3MrvjiG
Arduino Nanos https://amzn.to/36Gc3iu
Latching Buttons https://amzn.to/3k3Vd08
Wire https://amzn.to/3rN78ng
Potentiometers with knobs https://amzn.to/3k3wOYp
Project box https://amzn.to/3vBEt5E
Solderable breadboard https://amzn.to/3rOkVK7
Lots of solder https://amzn.to/3MskkG6
A little solder https://amzn.to/3L42MzA
3.5mm audio jacks https://amzn.to/3K1a0TK
5.5mm power jack https://amzn.to/3Oun4o6
Step bit drill set https://amzn.to/3rKd3sW

## Disclamer
I'm neither a coding expert nor a Fritzing wiz, but I hope this helps!

## Thank you!
Big thanks to Bloghoskins, Jan Ostman, and Mark Dammer.

Wishing wellness and wellbeing to all.
- Janis
