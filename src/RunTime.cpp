/* 
* RunTime.cpp
*
* Created: 12/07/2014 12:08:55 PM
* Author: e210290
*/

#include "RunTime.h"

// default constructor
RunTime::RunTime()
{
} //RunTime

void RunTime::begin()
{
	runtime = now();
}
void RunTime::tick()
{

	// throttle update
	if (!taskTimer.timeOut())
		return;

	// calculate runtime in days
	taskTimer.startTimer(60000); // update every minute
	unsigned long v = now() - runtime;

	setValueflt((float)v / (float)SECS_PER_DAY, 0);
}