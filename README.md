# AircoreCS
Arduno Library to drive CS4192 aircore gauge driver IC via Arduino

# Preface
Much of this library is directly derived from Guy Carpenter's excellent SwitecX25 library (https://github.com/clearwater/SwitecX25).
CS4192 is a now out of production aircore driver IC (Datasheet: http://www.onsemi.com/pub_link/Collateral/CS4192-D.PDF). It is commoly found in automotive dashboards in order to drive aircore motors. The communications is done via "standard" SPI (see below)

# Arduino wiring
Tested on a UNO, other boards might require relevand edits:
-PIN 13 (SCK) -> CS4192 pin 9 (SCLK)
-PIN 11 (MOSI) -> CS4192 pin 6 (SI)
-PIN xx (CS) -> CS4192 pin 10 (CS) pick any output pin, standard is 10, but you can have multiple devices.
-GND -> CS4192 pins 4,5,12,13
-5V -> CS4192 pin 7,8 (they are tied so OE is always on)

# CS4192 wiring
pin 1,2 to SIN- and SIN+ of aircore motor
pin 15,16 to COS- and COS+ of aircore motor

# Usage:
Coming soon


