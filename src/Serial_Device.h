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

// #define   SERIAL_DEBUG

const int SerialTimeOut = 5000;
const int StringBufferSize = 30;
const long TempCompThrottle = 1200000; // 20 mins = 20 * 60 * 1000 = 1200000

class PHSensor : public Measurement
{
  public:
	enum PHCommands
	{
		Read,
		DebugOn,
		DebugOff,
		CalStart,
		Cal7,
		Cal4,
		Cal10
	};
	PHSensor(HardwareSerial *id);
	void tick();
	void setMode();
	int getMode();
	const char *getModeString(int s);
	int setTarget(int t);

  protected:
	float temperature;
	HardwareSerial *device;
	char sensor[StringBufferSize];
	bool sensor_string_complete;
	enum
	{
		init,
		read,
		write,
		passthrough
	} state;
	enum PHCommands command;
	int target;
	int slen;
	Timer water_throttle;
	bool inCalibration;
};

class ECSensor : public Measurement
{
  public:
	enum ECCommands
	{
		Read,
		DebugOn,
		DebugOff,
		CalStart,
		CalDry,
		Cal3000,
		Cal220
	};
	ECSensor(HardwareSerial *id);
	void tick();
	void setMode();
	int getMode();
	const char *getModeString(int s);
	int setTarget(int t);

  protected:
	float temperature;
	HardwareSerial *device;
	char sensor[StringBufferSize];
	bool sensor_string_complete;
	enum
	{
		init,
		read,
		write,
		passthrough
	} state;
	enum ECCommands command;
	int target;
	int slen;
	Timer water_throttle;
	bool inCalibration;
};
