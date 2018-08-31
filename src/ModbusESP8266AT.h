#pragma once
// modbus tcp server
// using ESP8266 AT command set with ESP-01 modem
//
// modbus function codes
// FC3 - read multiple registers
// [id],[id],0,0,0,len,unit,FC=3,REF-H, REF-L, C-H, C-L
//
// FC16 - write multiple registers
// [id],[id],0,0,0,len,unit,FC=16,REF-H, REF-L, C-H, C-L, [byte count], val, val
//
// FC6 - write single register
// [id],[id],0,0,0,len,unit,FC=6, REF-H, REF-L, VAL-H, VAL-L
//
// TODO: Add checks for connection drop
// maybe a 5 minute diagnostic poll that restarts the link if necessary
// Add some counters for link health
 
#include "Measurement.h"
#include "ESP8266.h"
#include <string.h>

#define SSID "happycat"
#define PASSWORD "23Mort$treet"
const uint16_t bufferSize = 256;
const long mbTimeOut = 45000;

enum class ModState {
    t1, t2, t3, t4, 
    s1, s2
};

class ModbusDevice : public Measurement
{
  public:
    ModbusDevice(HardwareSerial &s);
    void begin();
    void tick();
    void reset();
    void restart();

  protected:
    HardwareSerial &sd;
    ESP8266 wifi;
    uint8_t rbuffer[bufferSize];
    uint8_t sbuffer[bufferSize];
    uint16_t sbufflen;
    uint16_t rbufflen;

    unsigned int len;
    ModState state;

    uint8_t mb_fc;   // modbus function code
    int16_t mb_ref;  // modbus reference
    uint16_t mb_cnt; // modbus size
    uint8_t mux;     // tcp stream mux

    int memstrn(char *needle);
    int memrem(int n);
    int memtoi(int n);

    uint16_t callsTotal;
    uint16_t callsError;
};

