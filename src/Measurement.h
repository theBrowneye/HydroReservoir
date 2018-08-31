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

class Measurement : public Timer
{
  public:
    Measurement() 
    {
        bPtr = hreg;
    };
    void tick(){};
    void setValuePtr(uint16_t b)
    {
        base = b;
    };

  protected:
    void setValueflt(float f, uint16_t offset) 
    {
        float * p = reinterpret_cast<float *>(hreg + base + offset);
        *p = f;
    };
    void setValueInt(uint16_t f, uint16_t offset)
    {
        *(hreg + base + offset) = f;
    };
    uint16_t base;
    uint16_t *bPtr;
};
