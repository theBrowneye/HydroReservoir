#pragma once
/*
 * Measurement.h
 *
 * Created: 24/03/2014 6:47:45 PM
 *  Author: e210290
 * 2015-10-18 Use timer class
 */

// TODO: implement bad value handling

#include "Arduino.h"
#include "Timer.h"
#include "dbx.h"

class Measurement
{
  public:
    Measurement()
    {
        state = Measurement::idle;
    };
    void tick();
    void setValuePtr(uint16_t b)
    {
        base = b;
    };
    void setValueflt(float f, uint16_t offset)
    {
        regmap.setValueFlt(base + offset, f);
    };
    void setValueInt(uint16_t f, uint16_t offset)
    {
        regmap.setValueInt(base + offset, f);
    };
    void setBadValue(bool b, uint16_t offset)
    {
        regmap.setBadValue(base + offset, b);
    }
    bool isBadValue(uint16_t offset)
    {
        return regmap.isBadValue(base + offset);
    }
    bool isFailed() 
    {
        return state < 0;
    };
    bool isIdle()
    {
        return state == 0;
    }
    bool isBusy()
    {
        return state > 0;
    }

  protected:
    uint16_t base;  // register offset for this object
    Timer taskTimer;
    Timer diagTimer;
    enum MeasurementStates
    {
        hardFail = -2,
        softFail = -1,
        idle = 0,
        busy = 1
    };
    int state;
};
