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
// ! 0 - debug mode (send all traffic to console)
// ! 1 - calibrate 7 (reset after operation)
// ! 2 - calibrate 4 (reset after operation)
// ! 3 - calibrate 10 (reset after operation)
// ! 4 - turn on debugging lights (reset after operation)
// ! 5 - turn off debugging lights (reset after operation)
// ! 6 - turn on passthrough (reset after operation)
// ! 7 - turn off passthrough (reset after operation)

// #define   SERIAL_DEBUG
enum PHFlags 
{
	pDebug = 1 << 0,
	pCal7 = 1 << 1,
	pCal4 = 1 << 2,
	pCal10 = 1 << 3,
	pDbgOn = 1 << 4,
	pDbgOff = 1 << 5,
	pPassOn = 1 << 6,
	pPassOff = 1 << 7
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
// ! 0 - debug mode (send all traffic to console)
// ! 1 - calibrate Z0 (reset after operation)
// ! 2 - calibrate Z2 (reset after operation)
// ! 3 - calibrate Z30 (reset after operation)
// ! 4 - turn on debugging lights (reset after operation)
// ! 5 - turn off debugging lights (reset after operation)
// ! 6 - turn on passthrough (reset after operation)
// ! 7 - turn off passthrough (reset after operation)

// #define   SERIAL_DEBUG
enum ECFlags 
{
	eDebug = 1 << 0,
	eCalDry = 1 << 1,
	eCal3000 = 1 << 2,
	eCal220 = 1 << 3,
	eDbgOn = 1 << 4,
	eDbgOff = 1 << 5,
	ePassOn = 1 << 6,
	ePassOff = 1 << 7
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
