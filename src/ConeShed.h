#pragma once
/*
 * ConeShed.h
 *
 * Created: 5/04/2014 10:45:59 AM
 *  Author: e210290
 */

#include "Time.h"

time_t getRunTimeSecs();       // run time in seconds
float getRunTimeDays();        // run time in days
const long MenuTimeOut = 250; // time between menu updates

// debug levels
#define DEBUG_PH1 false
#define DEBUG_PH2 true
// 2 - ec sensor
#define DEBUG_EC1 false
#define DEBUG_EC2 true
// 3 - Hon sensor
// #define DEBUG_HON (mm.debugLevel.value == 5)
