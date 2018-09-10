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
	"S",
	"F",
	"T",
};

PHSensor::PHSensor(HardwareSerial *id) : device(id)
{
	sensor[0] = 0;
	sensor_string_complete = false;
	state = init;
	command = Read;
	debug = false;
}

void PHSensor::setMode()
{
	static uint16_t lastFlags = 0xffff;
	uint16_t flags = getValueInt(2);
	command = PHCommands::Read;

	// check resetable flags
	if (flags & PHFlags::DbgOn)
	{
		command = PHCommands::DebugOn;
	}
	else if (flags & PHFlags::DbgOff)
	{
		command = PHCommands::DebugOff;
	}
	else if (flags & PHFlags::PassOn)
	{
		state = passthrough;
	}
	else if (flags & PHFlags::PassOff)
	{
		command = PHCommands::Read;
	}
	else if (flags & PHFlags::Cal7)
	{
		command = PHCommands::Cal7;
	}
	else if (flags & PHFlags::Cal4)
	{
		command = PHCommands::Cal4;
	}
	else if (flags & PHFlags::Cal10)
	{
		command = PHCommands::Cal10;
	}

	// check for debug mode and clear any resetable flags at the same time
	if (flags != lastFlags)
	{
		Serial.print("flags:");
		Serial.print(flags, HEX);
		lastFlags = flags;
	}
	debug = true;
	// debug = (flags &= PHFlags::Debug);
	
	// if (debug)
	// {
	// 	Serial.print("PH command: ");
	// 	Serial.println(command);
	// }
};

void PHSensor::tick()
{
	return;
	if (state == init)
	{
		// device->begin(38400);
		device->begin(1200);
		device->write("I\r");
		state = write;
	}

	// see if any commands received
	setMode();

	// get water temperature
	temperature = regmap.isBadValue(dbWatTemp) ? -1.0 : regmap.getValueFlt(dbWatTemp);

	// in normal mode, write a command and wait for a response (default is read)
	if (state == write)
	{
		if (water_throttle.timeOut())
		{
			if (temperature > 0) device->print(temperature);
			water_throttle.startTimer(TempCompThrottle);
		}
		else
		{
			device->write(PHCommandMessages[command]);
			device->write("\r");
			command = Read;
			// if (debug)
			// {
			// 	Serial.print("PH:");
			// 	Serial.println(PHCommandMessages[command]);
			// }
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
			if (!isprint(c)) c='.';
			// if (debug) Serial.print(c);
		}

		if (sensor_string_complete)
		{
			float v = atof(sensor);
			if (v > 1)
			{
				setValueflt(v, 0);
				setBadValue(false, 0);
			}
			state = write;
		}
		// if timeout on getting a complete string
		if (taskTimer.timeOut())
		{
			setBadValue(true, 0);
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
	// FIXME: get rid of this
	return;

	if (state == init)
	{
		device->begin(38400);
		device->write("P,1\r");
		state = write;
	}

	bool debug_msg = 9; //DEBUG_EC1 || (DEBUG_EC2 && (command != Read));

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
