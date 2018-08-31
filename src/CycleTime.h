#pragma once
/* 
* CycleTime.h
*
* Created: 8/04/2014 10:08:27 PM
* Author: e210290
*/

#include "Measurement.h"

class CycleTime : public Measurement
{
  public:
	CycleTime();
	void tick();

  private:
	unsigned long t;
	float v;
};
