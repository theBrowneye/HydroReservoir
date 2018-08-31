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
  bool isBusy();

private:
  byte honID;
  float humidity;
  float temperature;
  enum
  {
    command,
    read
  } state;
};
