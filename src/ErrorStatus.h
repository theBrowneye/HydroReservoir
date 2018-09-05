#pragma once
/*
* ErrorStatus.h
*
* Created: 30/03/2014 3:24:49 PM
* Author: e210290
*/
#include "Arduino.h"

// TODO: include error handling in my DBX object.  Currenlty none of this stuff is reported back via modbus

#define LogError(...)    \
	Serial.print("[");   \
	Serial.print(now()); \
	Serial.print("] ");  \
	Serial.println(__VA_ARGS__);

class ErrorStatus
{
  public:
	ErrorStatus();

	//variables
  public:
	bool extrf : 1;		//  0 - external reset
	bool borf : 1;		//  1 - brownout reset
	bool wdrf : 1;		//  2 - wdt reset
	bool jtrf : 1;		//  3 - jtag reset
	bool HON_error : 1; //  5 - unable to access HON sensor
	bool porf : 1;		//  7 - power on restart
	uint16_t restarts;  // number of restarts

}; //ErrorStatus

extern ErrorStatus Error;
