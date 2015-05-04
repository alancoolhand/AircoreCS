# AircoreCS
Arduno Library to drive CS4192 aircore gauge driver IC via Arduino

# Preface
Much of this library is directly derived from Guy Carpenter's excellent SwitecX25 library (https://github.com/clearwater/SwitecX25).
CS4192 is a now out of production aircore driver IC (Datasheet: http://www.onsemi.com/pub_link/Collateral/CS4192-D.PDF). It is commoly found in automotive dashboards in order to drive aircore motors. The communications is done via  SPI (see note below). CS4192 takes a 10bit digital value and converts it into sin+-,cos+- to drive

# Arduino wiring
Tested on a UNO, other boards might require relevand edits:<br>
-PIN 13 (SCK) -> CS4192 pin 9 (SCLK)<br>
-PIN 11 (MOSI) -> CS4192 pin 6 (SI)<br>
-PIN xx (CS) -> CS4192 pin 10 (CS) pick any output pin, standard is 10, but you can have multiple devices.<br>
-GND -> CS4192 pins 4,5,12,13<br>
-5V -> CS4192 pin 7,8 (they are tied so OE is always on)<br>

# CS4192 wiring
pin 1,2 to SIN- and SIN+ of aircore motor<br>
pin 15,16 to COS- and COS+ of aircore motor<br>
Supply +12V to pin 3 (nominal is 13.5, max is 16), make sure you are on common gnd with arduino if supplying from external power source

# Usage:
1. Import SPI.h and ArduinoCS.h
2. Into `void loop()` call constructor by passing CS pin number BEFORE SPI.begin():
`AircoreCS myAC(10);`
3. Init and begin SPI.<br /> 
  `SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  SPI.setBitOrder(MSBFIRST);` <br />
  Note that max SPI Mhz for CS4192 is 2 Mhz, hence SPI_CLOCK_DIV8. If you're having issues, try lowering speed to DIV16, or    shorten cables whenever possible.<br />
4. Pulse the motor by calling pulse() method: `myAC.pulse();`. This will enable output on CS4192.
5. Call `myAC.setPosition(x);` where x is an int from 0 to 1023, that translates on a needle position from 0 to 360 degrees.Note that this only sets the target position, but DOES NOT move the motor.
6. Call `myAC.update();` as frequently as possible. Note that this is a non-blocking function that moves the pointer a single step in the direction of the target set with setPosition(). This is a mymic of the original SwitecX25 library. For a full explanation see here: https://github.com/clearwater/SwitecX25


# Non-standard SPI note
CS4192 does use SPI_MODE_0 (which is most common), but what it does in non standard way is that CS is normally LOW, and pulled HIGH to select the chip and transfer data. The sin, cos output will then happen when CS is pulled LOW.
What this means is that you WILL run into trouble if you're using other SPI devices in the same application.Beware!

#Acceleration table
The logic behind the acceleration table is best explained here:
http://guy.carpenter.id.au/gaugette/2012/01/26/simplified-acceleration-model/

As i tested it the standard one which is provided in the code is quite slow for aircores. Play with it if you understand it. For my application it was more or less ok.



