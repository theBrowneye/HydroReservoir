#pragma once
/*
 * Serial_Device.h
 *
 * Created: 27/03/2014 7:57:10 PM
 *  Author: e210290
 * 2015-10-18 Moved temperature throttle to timer class
 */
#include "ConeShed.h"
#include "Measurement.h"

// memory map definition
// ! 5 ! 4 ! 3 ! 2 ! 1 ! 0 ! 9 ! 8 ! 7 ! 6 ! 5 ! 4 ! 3 ! 2 ! 1 ! 0 !
// !---------------------------------------------------------------!
// ! PV float word 1                                               !
// ! PV float word 2                                               !
// !---------------------------------------------------------------!
// ! PH control word (reset to zero after operation)               !
// !---------------------------------------------------------------!
// ! 0 - nop
// ! 1 - debug on
// ! 2 - debug off
// ! 3 - calibrate 7 (reset after operation)
// ! 4 - calibrate 4 (reset after operation)
// ! 5 - calibrate 10 (reset after operation)
// ! 6 - turn on debugging lights (reset after operation)
// ! 7 - turn off debugging lights (reset after operation)
// ! 8 - turn on passthrough (reset after operation)
// ! 9 - turn off passthrough (reset after operation)

// #define   SERIAL_DEBUG
enum PHFlags 
{
	pDebugOn = 1,
	pDebugOff = 2,
	pCal7 = 3,
	pCal4 = 4,
	pCal10 = 5,
	pDbgOn = 6,
	pDbgOff = 7,
	pPassOn = 8,
	pPassOff = 9
};

const int SerialTimeOut = 5000;
const int StringBufferSize = 30;
const long TempCompThrottle = 20l * 60l * 1000l; // 20 mins = 20 * 60 * 1000 = 1200000

class PHSensor : public Measurement
{
  public:
	enum PHCommands
	{
		Read,
		DebugOn,
		DebugOff,
		Cal7,
		Cal4,
		Cal10
	};
	PHSensor(HardwareSerial *id);
	void tick();
	void setMode();

  protected:
	float temperature;
	HardwareSerial *device;
	char sensor[StringBufferSize];
	bool sensor_string_complete;
	enum
	{
		hardFail = -2,
        softFail = -1,
        idle = 0,
		init,
		read,
		write,
		passthrough
	};
	enum PHCommands command;
	int target;
	int slen;
	Timer water_throttle;
	bool debug;
	// bool inCalibration;
};

// memory map definition
// ! 5 ! 4 ! 3 ! 2 ! 1 ! 0 ! 9 ! 8 ! 7 ! 6 ! 5 ! 4 ! 3 ! 2 ! 1 ! 0 !
// !---------------------------------------------------------------!
// ! PV float word 1                                               !
// ! PV float word 2                                               !
// !---------------------------------------------------------------!
// ! EC control word (reset to zero after operation)               !
// !---------------------------------------------------------------!
// ! 0 - NOP
// ! 1 - debug on
// ! 2 - debug off
// ! 3 - calibrate Z0 (reset after operation)
// ! 4 - calibrate Z2 (reset after operation)
// ! 5 - calibrate Z30 (reset after operation)
// ! 6 - turn on debugging lights (reset after operation)
// ! 7 - turn off debugging lights (reset after operation)
// ! 8 - turn on passthrough (reset after operation)
// ! 9 - turn off passthrough (reset after operation)

// #define   SERIAL_DEBUG
enum ECFlags 
{
	eDebugOn = 1,
	eDebugOff = 2,
	eCalDry = 3,
	eCal3000 = 4,
	eCal220 = 5,
	eDbgOn = 6,
	eDbgOff = 7,
	ePassOn = 8,
	ePassOff = 9
};

class ECSensor : public Measurement
{
  public:
	enum ECCommands
	{
		Read,
		DebugOn,
		DebugOff,
		CalDry,
		Cal3000,
		Cal220
	};
	ECSensor(HardwareSerial *id);
	void tick();
	void setMode();

  protected:
	float temperature;
	HardwareSerial *device;
	char sensor[StringBufferSize];
	bool sensor_string_complete;
	enum
	{
		hardFail = -2,
        softFail = -1,
        idle = 0,
		init,
		read,
		write,
		passthrough
	};
	enum ECCommands command;
	int target;
	int slen;
	Timer water_throttle;
	bool debug;
};
