#pragma once
//
// ultrasonic sensor
//
#include "Measurement.h"
#include "ConeShed.h"
#include "NewPing.h"

// TODO: add code to skip eratic changes

class Sonar : public Measurement
{
private:
  const int ReadDelay = 250;
  const float alpha = 0.05;

public:
  Sonar(byte trigger, byte pulse, int maxDst);
  void tick();

private:
  NewPing m_s;
  float wt;
  float filter;
  float span, offset;
};
