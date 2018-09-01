#pragma once
/*
* HON_Sensor.h
*
* Created: 29/03/2014 7:16:42 PM
* Author: e210290
*/
#include "Measurement.h"
#include "ConeShed.h"

class HONSensor : public Measurement
{
public:
  HONSensor(byte id);
  void tick();
  void begin();

private:
  byte honID;
  float humidity;
  float temperature;
  enum
  {
    hardFail = -2,
    softFail = -1,
    idle = 0,
    read = 1
  } state;
};
