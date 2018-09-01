#pragma once
/* 
* Timer.h
*
* Created: 18/10/2015 11:03:47 AM
* Author: gerry
*/
#include "Arduino.h"

class Timer
{
	//variables
	public:
	protected:
		long interval;
		long timeout;
		bool timedOut;
		bool stopped;
	private:
		//functions
	public:
		Timer();
		Timer(long t);
		bool timeOut();
		void startTimer(long t);
		void restartTimer();
		void stopTimer();
		long timeLeft();
}; //Timer
