/*
 *  Aircore CS4192 gauge driver Arduino Library
 *  Alan Locatelli - 2015
 *  Licensed under the BSD2 license, see license.txt for details.
 *  contains portions of code from
 *  SwitecX25 Arduino Library
 *  Guy Carpenter, Clearwater Software - 2012
 *  Licensed under the BSD2 license, see license.txt for details.
 *
 *  All text above must be included in any redistribution.
 */

#include <Arduino.h>
#include <SPI.h>
#include "AircoreCS.h"


// This table defines the acceleration curve.
// 1st value is the speed step, 2nd value is delay in microseconds
// 1st value in each row must be > 1st value in subsequent row
// 1st value in last row should be == maxVel, must be <= maxVel
// This is unchanged from Switec X25 acceleration table. It is a bit slow for aircores, but OK for a test.
// Will need to tweak it.

static unsigned short defaultAccelTable[][2] = {
  {   20, 3000},
  {   50, 1500},
  {  100, 1000},
  {  150,  800},
  {  300,  600}
};

#define DEFAULT_ACCEL_TABLE_SIZE (sizeof(defaultAccelTable)/sizeof(*defaultAccelTable))

AircoreCS::AircoreCS(unsigned int cs)
{
  //CS4192 driver accepts a 10bit value
  this->steps = 1024;

  //this is CS (chip select) pin used for this instance of the aircore. Every instance has common SCK and MOSI pins.
  this->cs = cs;
  pinMode(this->cs, OUTPUT);

  //from CS4192 datasheet, contrary from most SPI devices, with this chip CS is normally LOW, and pulled HIGH to initiate a transfer
  digitalWrite(cs,LOW);
  
  dir = 0;
  vel = 0;
  stopped = true;
  currentStep = 0;
  targetStep = 0;

  accelTable = defaultAccelTable;
  maxVel = defaultAccelTable[DEFAULT_ACCEL_TABLE_SIZE-1][0]; // last value in table.
}


//according to CS4192 datasheet, in order to enable output of the chip the CS pin must be "pulsed".
//100 ms was a "random" value, but I found it to be working for me. YMMV
//For every motor this HAS to be called in void setup() AFTER SPI.begin()

void AircoreCS::pulse() {
        digitalWrite(cs,HIGH);
        delay(100);
        digitalWrite(cs,LOW);
}

void AircoreCS::writeIO()
{
     //from CS4192 datasheet, contrary from most SPI devices, with this chip CS is normally LOW, and pulled HIGH to initiate a transfer
     digitalWrite(cs, HIGH);

     //  send in the address and value via SPI:
     //CS4192 expects a 10 bit value, we're splitting it in 2 bytes and sending it.

      byte byte1 = (currentStep >> 8);
      byte byte0 = (currentStep & 0xFF); //0xFF = B11111111
      SPI.transfer(byte1);
      SPI.transfer(byte0);
      // take the CS pin low to de-select the chip:
      digitalWrite(cs, LOW);

}

void AircoreCS::stepUp()
{
  if (currentStep < steps) {
    currentStep++;

    writeIO();
  }
}

void AircoreCS::stepDown()
{ 
  if (currentStep > 0) {
    currentStep--;

    writeIO();
  }
}

void AircoreCS::zero()
{
  currentStep = steps - 1;
  for (unsigned int i=0;i<steps;i++) {
    stepDown();
    delayMicroseconds(RESET_STEP_MICROSEC);
  }
  currentStep = 0;
  targetStep = 0;
  vel = 0;
  dir = 0;
}

// This function determines the speed and accel
// characteristics of the motor.  Ultimately it 
// steps the motor once (up or down) and computes
// the delay until the next step.  Because it gets
// called once per step per motor, the calcuations
// here need to be as light-weight as possible, so
// we are avoiding floating-point arithmetic.
//
// To model acceleration we maintain vel, which indirectly represents
// velocity as the number of motor steps travelled under acceleration
// since starting.  This value is used to look up the corresponding
// delay in accelTable.  So from a standing start, vel is incremented
// once each step until it reaches maxVel.  Under deceleration 
// vel is decremented once each step until it reaches zero.

void AircoreCS::advance()
{
  time0 = micros();
  
  // detect stopped state
  if (currentStep==targetStep && vel==0) {
    stopped = true;
    dir = 0;
    return;
  }
  
  // if stopped, determine direction
  if (vel==0) {
    dir = currentStep<targetStep ? 1 : -1;
    // do not set to 0 or it could go negative in case 2 below
    vel = 1; 
  }
  
  if (dir>0) {
    stepUp();
  } else {
    stepDown();
  }
  
  // determine delta, number of steps in current direction to target.
  // may be negative if we are headed away from target
  int delta = dir>0 ? targetStep-currentStep : currentStep-targetStep;
  
  if (delta>0) {
    // case 1 : moving towards target (maybe under accel or decel)
    if (delta < vel) {
      // time to declerate
      vel--;
    } else if (vel < maxVel) {
      // accelerating
      vel++;
    } else {
      // at full speed - stay there
    }
  } else {
    // case 2 : at or moving away from target (slow down!)
    vel--;
  }
    
  // vel now defines delay
  unsigned char i = 0;
  // this is why vel must not be greater than the last vel in the table.
  while (accelTable[i][0]<vel) {
    i++;
  }
  microDelay = accelTable[i][1];
}

void AircoreCS::setPosition(unsigned int pos)
{
  // pos is unsigned so don't need to check for <0
  if (pos >= steps) pos = steps-1;
  targetStep = pos;
  if (stopped) {
    // reset the timer to avoid possible time overflow giving spurious deltas
    stopped = false;
    time0 = micros();
    microDelay = 0;
  }
}

void AircoreCS::update()
{
  if (!stopped) {
    unsigned long delta = micros() - time0;
    if (delta >= microDelay) {
      advance();
    }
  }
}


//This updateMethod is blocking, it will give you smoother movements, but your application will wait for it to finish
void AircoreCS::updateBlocking()
{
  while (!stopped) {
    unsigned long delta = micros() - time0;
    if (delta >= microDelay) {
      advance();
    }
  }
}

