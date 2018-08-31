#pragma once
/* 
* RunTime.h
*
* Created: 12/07/2014 12:08:55 PM
* Author: e210290
*/
#include "Measurement.h"
#include "time.h"

class RunTime : public Measurement
{
  public:
	RunTime();
	void tick();
	void begin();

  private:
	time_t runtime;
}; //RunTime
