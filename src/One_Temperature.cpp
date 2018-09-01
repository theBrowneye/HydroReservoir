/*
 * Water_Temperature.cpp
 *
 * Created: 24/03/2014 4:41:51 PM
 *  Author: e210290
 */

#include "One_Temperature.h"

const int CommandDelay = 250;
const int ReadDelay = 1000;
const long FailedDelay = 60000;
const float MaxLow = 5.0;
const float MaxHigh = 45.0;

OneTemperature::OneTemperature(uint8_t id) : ds(id)
{
    state = init;
}

void OneTemperature::tick()
{

    if (!taskTimer.timeOut())
        return;

    if (state == init)
    {

        if (!ds.search(addr))
        {
            ds.reset_search();
            delay(CommandDelay);
            // pValue->updateBad();
            taskTimer.startTimer(FailedDelay);
            return;
        }

        if (OneWire::crc8(addr, 7) != addr[7])
        {
            // pValue->updateBad();
            taskTimer.startTimer(FailedDelay);
            return;
        }

        switch (addr[0])
        {
        case 0x10:
            type_s = 1;
            break;
        case 0x28:
        case 0x22:
            type_s = 0;
            break;
        default:
            // pValue->updateBad();
            return;
        }
        state = found;
    }

    if (state == found)
    {
        ds.reset();
        ds.select(addr);
        ds.write(0x44, 1); // start conversion, with parasite power on at the end

        taskTimer.startTimer(ReadDelay); // maybe 750ms is enough, maybe not
        // we might do a ds.depower() here, but the reset will take care of it.
        state = write;
        return;
    }

    if (state == write)
    {
        ds.reset();
        ds.select(addr);
        ds.write(0xBE); // Read Scratchpad

        for (byte i = 0; i < 9; i++)
        { // we need 9 bytes
            data[i] = ds.read();
        }

        int16_t raw = (data[1] << 8) | data[0];
        if (type_s)
        {
            raw = raw << 3; // 9 bit resolution default
            if (data[7] == 0x10)
            {
                // "count remain" gives full 12 bit resolution
                raw = (raw & 0xFFF0) + 12 - data[6];
            }
        }
        else
        {
            byte cfg = (data[4] & 0x60);
            // at lower res, the low bits are undefined, so let's zero them
            if (cfg == 0x00)
                raw = raw & ~7; // 9 bit resolution, 93.75 ms
            else if (cfg == 0x20)
                raw = raw & ~3; // 10 bit res, 187.5 ms
            else if (cfg == 0x40)
                raw = raw & ~1; // 11 bit res, 375 ms
            //// default is 12 bit resolution, 750 ms conversion time
        }

        float v = (float)raw / 16.0;
        if (v > MaxLow && v < MaxHigh)
        {
            setValueflt((float)raw / 16.0, 0);
            taskTimer.startTimer(CommandDelay);
        }
        else
        {
            // pValue->updateBad();
            taskTimer.startTimer(FailedDelay);
        }
        state = read;
        return;
    }

    if (state == read)
    {
        state = found;
        return;
    }
}
