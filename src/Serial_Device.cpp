/*
 * Serial_Device.cpp
 *
 * Created: 27/03/2014 8:01:56 PM
 *  Author: e210290
 */

#include "Serial_Device.h"
#include "dbxMemoryMap.h"

const char *const PHCommandMessages[] = {
	"R",
	"L1",
	"L0",
	"CalStart",
	"S",
	"F",
	"T",
};
const char *const PHCommandNames[] = {
	"Read    ",
	"DebugOn ",
	"DebugOff",
	"CalStart",
	"Cal7.0  ",
	"Cal4.0  ",
	"Cal10.0 "
};

PHSensor::PHSensor(HardwareSerial *id) : device(id)
{
	sensor[0] = 0;
	sensor_string_complete = false;
	state = init;
	command = Read;
	inCalibration = false;
}

const char *PHSensor::getModeString(int s)
{
	return PHCommandNames[s];
}

int PHSensor::getMode()
{
	return command;
}

void PHSensor::setMode()
{
	switch (target)
	{
	case 0:
		command = Read;
		break;
	case 1:
		command = DebugOn;
		state = passthrough;
		break;
	case 2:
		command = DebugOff;
		state = write;
		command = Read;
		break;
	case 3:
		command = CalStart;
		break;
	case 4:
		command = Cal7;
		break;
	case 5:
		command = Cal4;
		break;
	case 6:
		command = Cal10;
		break;
	default:
		command = Read;
		break;
	}
	if (DEBUG_PH1 || (DEBUG_PH2 && (command != Read)))
	{
		Serial.print("PH command: ");
		Serial.println(PHCommandMessages[command]);
	}
};

int PHSensor::setTarget(int t)
{
	target += t;
	target = max(Read, target);
	target = min(Cal10, target);
	return target;
}

void PHSensor::tick()
{

	if (state == init)
	{
		device->begin(38400);
		state = write;
	}

	bool debug_msg = DEBUG_PH1 || (DEBUG_PH2 && (command != Read));

	// get water temperature
	temperature = regmap.getValueFlt(dbWatTemp);

	// in normal mode, write a command and wait for a response (default is read)
	if (state == write)
	{
		if (water_throttle.timeOut() || command == CalStart)
		{
			if (!inCalibration)
			{
				if (temperature > 0)
					device->print(temperature);
				if (command == CalStart)
				{
					command = Read;
					inCalibration = true;
				}
			}
			water_throttle.startTimer(TempCompThrottle);
		}
		else
		{
			device->write(PHCommandMessages[command]);
			device->write("\r");
			command = Read;
			if (debug_msg)
			{
				Serial.print("PH:");
				Serial.println(PHCommandMessages[command]);
			}
			if (command == Cal10)
				inCalibration = false;
		}
		state = read;
		slen = 0;
		sensor_string_complete = false;
		taskTimer.startTimer(SerialTimeOut);

		// in passthrough mode, just send anything received from serial in
	}
	else if (state == passthrough)
	{
		while (Serial.available())
		{
			char c = Serial.read();
			device->write(c);
		}
		while (device->available())
		{
			char c = device->read();
			Serial.write(c);
			if (c == '\r')
				Serial.write('\n');
		}
	}

	// try to read anything we've received
	if (state == read)
	{
		while (device->available())
		{
			char c = device->read();
			sensor[slen++] = c;
			sensor[slen] = 0;
			if (c == '\r')
			{
				sensor_string_complete = true;
			}
		}

		if (sensor_string_complete)
		{
			float v = atof(sensor);
			if (v > 1)
				setValueflt(v, 0);
			if (debug_msg)
			{
				Serial.print("PH[");
				Serial.print(sensor);
				Serial.print("] ");
				Serial.println(v);
			}
			state = write;
		}
		// if timeout on getting a complete string
		if (taskTimer.timeOut())
		{
			// pValue->updateBad();
			state = write;
		}
	}
}

//============================================================================================

const char *const ECCommandMessages[] = {"R", "L1", "L0", "CalStart", "Z0", "Z30", "Z2"};
const char *const ECCommandNames[] = {"Read    ", "DebugOn  ", "DebugOff", "CalStart", "CalDry  ", "Cal3000 ", "Cal220  "};

ECSensor::ECSensor(HardwareSerial *id) : device(id)
{
	sensor[0] = 0;
	sensor_string_complete = false;
	state = init;
	command = Read;
	inCalibration = false;
}

const char *ECSensor::getModeString(int s)
{
	return ECCommandNames[s];
}

int ECSensor::getMode()
{
	return command;
}

void ECSensor::setMode()
{
	switch (target)
	{
	case 0:
		command = Read;
		break;
	case 1:
		command = DebugOn;
		state = passthrough;
		break;
	case 2:
		command = DebugOff;
		state = write;
		command = Read;
		break;
	case 3:
		command = CalStart;
		break;
	case 4:
		command = CalDry;
		break;
	case 5:
		command = Cal3000;
		break;
	case 6:
		command = Cal220;
		break;
	default:
		command = Read;
		break;
	}
	if (DEBUG_EC1 || (DEBUG_EC2 && (command != Read)))
	{
		Serial.print("EC command: ");
		Serial.println(command);
	}
};

int ECSensor::setTarget(int t)
{
	target += t;
	target = max(Read, target);
	target = min(Cal220, target);
	return target;
}

void ECSensor::tick()
{

	if (state == init)
	{
		device->begin(38400);
		device->write("P,1\r");
		state = write;
	}

	bool debug_msg = DEBUG_EC1 || (DEBUG_EC2 && (command != Read));

	// get water temperature
	temperature = regmap.getValueFlt(dbWatTemp);

	if (state == write)
	{
		if (water_throttle.timeOut() || command == CalStart)
		{
			if (!inCalibration)
			{
				if (temperature > 0)
					device->print(temperature);
				if (command == CalStart)
				{
					command = Read;
					inCalibration = true;
				}
			}
			water_throttle.startTimer(TempCompThrottle);
		}
		else
		{
			device->write(ECCommandMessages[command]);
			device->write("\r");
			command = Read;
			if (debug_msg)
			{
				Serial.print("EC:");
				Serial.println(PHCommandMessages[command]);
			}
			if (command == Cal220)
				inCalibration = false;
		}
		state = read;
		slen = 0;
		sensor_string_complete = false;
		taskTimer.startTimer(SerialTimeOut);
	}
	else if (state == passthrough)
	{
		while (device->available())
		{
			char c = device->read();
			Serial.write(c);
			if (c == '\r')
				Serial.write('\n');
		}
		while (Serial.available())
		{
			char c = Serial.read();
			device->write(c);
		}
	}

	if (state == read)
	{
		while (device->available())
		{
			char c = device->read();
			sensor[slen++] = c;
			sensor[slen] = 0;
			if (c == '\r')
			{
				sensor_string_complete = true;
			}
		}

		if (sensor_string_complete)
		{
			float v = atof(sensor);
			if (v > 1)
				setValueflt(v, 0);
			if (debug_msg)
			{
				Serial.print("EC[");
				Serial.print(sensor);
				Serial.print("]");
				Serial.println(v);
			}
			state = write;
		}

		// if timeout on getting a complete string
		if (taskTimer.timeOut())
		{
			// pValue->updateBad();
			state = write;
		}
	}
}
