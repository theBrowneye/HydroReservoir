#pragma once
/*
 * One_Temperature.h
 *
 * Created: 24/03/2014 7:01:16 PM
 *  Author: e210290
 *
 * Read a 1 wire temperature device
 */
#include "Measurement.h"
#include "OneWire.h"

class OneTemperature : public Measurement
{
  public:
    OneTemperature(byte id);
    void tick();

  private:
    OneWire ds;
    byte addr[8];
    byte data[12];
    byte type_s;
    enum
    {
        init,
        found,
        write,
        read
    } state; //  might need compiler flag to reduce size -fshort-enums
};
