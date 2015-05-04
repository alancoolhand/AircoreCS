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

#ifndef AircoreCS_h
#define AircoreCS_h
#include <Arduino.h>
#include <SPI.h>

#define RESET_STEP_MICROSEC 3000
class AircoreCS
{
 public:

   unsigned int cs;
   unsigned int currentStep;      // step we are currently at
   unsigned int targetStep;       // target we are moving to
   unsigned int steps;            // total steps available
   unsigned long time0;           // time when we entered this state
   unsigned int microDelay;       // microsecs until next state
   unsigned short (*accelTable)[2]; // accel table can be modified.
   unsigned int maxVel;           // fastest vel allowed
   unsigned int vel;              // steps travelled under acceleration
   char dir;                      // direction -1,0,1  
   boolean stopped;               // true if stopped
   
   AircoreCS(unsigned int cs);
   void pulse();
   void stepUp();
   void stepDown();
   void zero();
   void update();
   void updateBlocking();
   void setPosition(unsigned int pos);

 private:
   void advance();
   void writeIO();
};


#endif

