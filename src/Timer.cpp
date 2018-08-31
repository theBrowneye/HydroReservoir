/* 
* Timer.cpp
*
* Created: 18/10/2015 11:03:47 AM
* Author: gerry
*/

#include "Timer.h"

// default constructor
Timer::Timer()
{
	timedOut = true;
	interval = timeout = 0;
} //Timer

Timer::Timer(long t)
{
	startTimer(t);
}

void Timer::startTimer(long t)
{
	interval = t;
	timeout = millis() + t;
	timedOut = false;
}

bool Timer::timeOut()
{
	if (timedOut)
		return true;
	return timedOut = (((long)millis() - timeout) >= 0);
}

// set timer to next interval after now
void Timer::restartTimer()
{
	timedOut = false;
	while (timeOut())
		timeout += interval;
}