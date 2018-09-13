/*
 * Serial_Device.cpp
 *
 * Created: 27/03/2014 8:01:56 PM
 *  Author: e210290
 */

#include "Serial_Device.h"
#include "dbxMemoryMap.h"

const char *const PHCommandMessages[] = {"R","L1","L0","S","F","T"};

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
	uint16_t flags = getValueInt(2);
	command = PHCommands::Read;

	// check resetable flags
	if (flags & PHFlags::pDbgOn)
	{
		command = PHCommands::DebugOn;
		state = write;
	}
	else if (flags & PHFlags::pDbgOff)
	{
		command = PHCommands::DebugOff;
		state = write;
	}
	else if (flags & PHFlags::pPassOn)
	{
		state = passthrough;
	}
	else if (flags & PHFlags::pPassOff)
	{
		command = PHCommands::Read;
		state = write;
	}
	else if (flags & PHFlags::pCal7)
	{
		command = PHCommands::Cal7;
		state = write;
	}
	else if (flags & PHFlags::pCal4)
	{
		command = PHCommands::Cal4;
		state = write;
	}
	else if (flags & PHFlags::pCal10)
	{
		command = PHCommands::Cal10;
		state = write;
	}

	flags &= PHFlags::pDebug;	
	debug = (flags != 0);	
	setValueInt(flags, 2);
};

void PHSensor::tick()
{
	if (state == init)
	{
		device->begin(38400);
		device->write("L0\r");
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
			if (debug)
			{
				Serial.print("PH:");
				Serial.println(PHCommandMessages[command]);
			}
			command = Read;
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
			if (c != '\n') device->write(c);
			Serial.write(c);
		}
		while (device->available())
		{
			char c = device->read();
			Serial.write(c);
			if (c == '\r') Serial.write('\n');
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
			if (debug) Serial.print(c);
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

const char *const ECCommandMessages[] = {"R", "L1", "L0", "Z0", "Z30", "Z2"};

ECSensor::ECSensor(HardwareSerial *id) : device(id)
{
	sensor[0] = 0;
	sensor_string_complete = false;
	state = init;
	command = Read;
	debug = false;
}

void ECSensor::setMode()
{
	uint16_t flags = getValueInt(2);
	command = ECCommands::Read;

	// check resetable flags
	if (flags & ECFlags::eDbgOn)
	{
		command = ECCommands::DebugOn;
		state = write;
	}
	else if (flags & ECFlags::eDbgOff)
	{
		command = ECCommands::DebugOff;
		state = write;
	}
	else if (flags & ECFlags::ePassOn)
	{
		state = passthrough;
	}
	else if (flags & ECFlags::ePassOff)
	{
		command = ECCommands::Read;
		state = write;
	}
	else if (flags & ECFlags::eCalDry)
	{
		command = ECCommands::CalDry;
		state = write;
	}
	else if (flags & ECFlags::eCal3000)
	{
		command = ECCommands::Cal3000;
		state = write;
	}
	else if (flags & ECFlags::eCal220)
	{
		command = ECCommands::Cal220;
		state = write;
	}

	flags &= ECFlags::eDebug;	
	debug = (flags != 0);	
	setValueInt(flags, 2);
}

void ECSensor::tick()
{
	if (state == init)
	{
		device->begin(38400);
		device->write("P,1\r");
		device->write("L0\r");
		state = write;
	}

	// see if any commands received
	setMode();
	
	// get water temperature
	temperature = regmap.isBadValue(dbWatTemp) ? -1.0 : regmap.getValueFlt(dbWatTemp);

	if (state == write)
	{
		if (water_throttle.timeOut())
		{
			if (temperature > 0) device->print(temperature);
			water_throttle.startTimer(TempCompThrottle);
		}
		else
		{
			device->write(ECCommandMessages[command]);
			device->write("\r");
			if (debug)
			{
				Serial.print("EC:");
				Serial.println(ECCommandMessages[command]);
			}
			command = Read;
		}
		state = read;
		slen = 0;
		sensor_string_complete = false;
		taskTimer.startTimer(SerialTimeOut);
	}
	else if (state == passthrough)
	{
		while (Serial.available())
		{
			char c = Serial.read();
			if (c != '\n') device->write(c);
			Serial.write(c);
		}
		while (device->available())
		{
			char c = device->read();
			Serial.write(c);
			if (c == '\r') Serial.write('\n');
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
			if (debug) Serial.print(c);
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
