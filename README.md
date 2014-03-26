hallight 
==========

Cicruts and firmware for LED backlighting with ambient light sensor IC and passive infrared motion sensor.

Companents:
* Microcontroller ATTINY85.
* Passive infrared motion sensor (Panasonic PaPIRS EKMC1601111)
* Ambient light sensor IC (BH1772GLC-E2)
* IR sensor for configurations (TSOP34836)

Get, Compiling, Run
==========
1. Get halligth: 
   git clone git@github.com:BASM/hallight.git && cd hallight

2. Switch to pasha branch:
   git checkout pasha

3. Init submodules:
   git submodule init
   git submodule update

4. Update environments:
   source firmware/env.sh

5. Build libraries:  
   cd  firmware/lib/libir
   cmake .
   make
   cd -

   cd firmware/lib/libavr
   cmake .
   make
   cd -

6. Build project:
   cd firmware
   make

7. Flash main.hex to avr:
   make flash
   
