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
	stopped = false;
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
	stopped = timedOut = false;
}

bool Timer::timeOut()
{
	if (stopped)
		return false;
	if (timedOut)
		return true;
	return timedOut = (((long)millis() - timeout) >= 0);
}

long Timer::timeLeft()
{
	return (long)millis() - timeout;
}

// set timer to next interval after now
void Timer::restartTimer()
{
	stopped = timedOut = false;
	while (timeOut())
		timeout += interval;
}

// stop the timer
void Timer::stopTimer()
{
	stopped = true;
	timedOut = false;
}