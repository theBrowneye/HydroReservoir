#pragma once
/*
* ErrorStatus.h
*
* Created: 30/03/2014 3:24:49 PM
* Author: e210290
*/
#include "Arduino.h"

#define LogError(...)    \
	Serial.print("[");   \
	Serial.print(now()); \
	Serial.print("] ");  \
	Serial.println(__VA_ARGS__);

class ErrorStatus
{
	//variables
  public:
	union {
		struct
		{
			uint8_t cycle_time : 4;   //  0 - cycle time (ms)
			uint8_t modbus_good : 4;  //  4 - modbus counter
			uint8_t restarts : 4;	 //  8 - reset counter
			uint8_t modbus_bad : 4;   // 12 - modbus errors
			bool extrf : 1;			  //  0 - external reset
			bool borf : 1;			  //  1 - brownout reset
			bool wdrf : 1;			  //  2 - wdt reset
			bool jtrf : 1;			  //  3 - jtag reset
			bool RTC_Error : 1;		  //  4 - unable to access RTC
			bool HON_error : 1;		  //  5 - unable to access HON sensor
			bool RTC_consistency : 1; //  6 - true if RTC is consistent
		};
		uint32_t l;
		uint16_t w[2];
	};
	ErrorStatus();

}; //ErrorStatus

extern ErrorStatus Error;
