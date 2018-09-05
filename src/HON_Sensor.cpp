/*
* HON_Sensor.cpp
*
* Created: 29/03/2014 7:16:42 PM
* Author: e210290
*/

#include "HON_Sensor.h"
#include "Wire.h"
#include "ErrorStatus.h"

const int CommandDelay = 250;
const int ErrorDelay = 250;
const int UpdateDelay = 1000; // throttle update rate so LCD doesn't jitter

// default constructor
HONSensor::HONSensor(byte id)
{
    honID = id;
    state = idle;
}

void HONSensor::begin()
{
    Wire.begin(); // initiate wire library
}

void HONSensor::tick()
{
    if (!taskTimer.timeOut())
        return;

    if (state == idle)
    {

        Wire.beginTransmission(honID);
        Wire.endTransmission();
        taskTimer.startTimer(CommandDelay);
        state = read;
        return;
    }

    if (state == read)
    {
        Wire.requestFrom(honID, (byte)4);
        if (Wire.available() == 4)
        {
            int a, b, c, d;
            a = Wire.read();
            b = Wire.read();
            c = Wire.read();
            d = Wire.read();

            humidity = (((a & 0x3f) << 8) + b);
            humidity = humidity / 16382 * 100;
            temperature = ((c * 64) + (d >> 2));
            temperature = temperature / 16382 * 165 - 40;
            setValueflt(temperature, 0);
            setValueflt(humidity, 2);
            taskTimer.startTimer(UpdateDelay);
            Error.HON_error = false;
        }
        else
        {
            // pValue->updateBad(temperature);
            // pValue2->updateBad(humidity);
            taskTimer.startTimer(ErrorDelay);
            Error.HON_error = true;
            // if (DEBUG_HON)
            //     Serial.println("HON:bad");
        }
        state = idle;
    }
}
