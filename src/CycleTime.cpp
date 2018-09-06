/* 
* CycleTime.cpp
*
* Created: 8/04/2014 10:08:27 PM
* Author: e210290
*/

#include "CycleTime.h"

// default constructor
CycleTime::CycleTime()
{
	v = 500;
} //CycleTime

const float TimeConstant = 0.001;
unsigned long last = micros();

void CycleTime::tick()
{
	unsigned long m = micros();
	t = m - last;
	last = m;
	if (t > 0)
		v = TimeConstant * (float)t + (1 - TimeConstant) * v;

	if (!taskTimer.timeOut())
		return;

	taskTimer.startTimer(1000);

 	setValueflt(v, 0);

	 // TODO: Add restart counter
}
