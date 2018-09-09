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
// TODO: what value to set for mbTimeOut??  Currently it's based on the host poll rate but that doesn't appear right.

#include "Measurement.h"
#include "ESP8266.h"
#include <string.h>

#define SSID "happycat"
#define PASSWORD "23Mort$treet"
const int16_t bufferSize = 256;
const long msgTimeOut = 5l * 1000l;         // 5 seconds to complete a message cycle
const long diagTimeOut = 1l * 60l * 1000l; // 1 minutes to check connection and restart if necessary
const int16_t BarometerFail = 5;         // fail device after 10 bad connects or status

class ModbusDevice : public Measurement
{
public:
  ModbusDevice(HardwareSerial &s);
  void begin();
  void tick();
  void reset();
  void restart();
  bool connect(bool connectStrength = false);
  int  getState();

protected:
  HardwareSerial &sd;
  ESP8266 wifi;
  uint8_t rbuffer[bufferSize];
  uint8_t sbuffer[bufferSize];
  int16_t sbufflen;
  int16_t rbufflen;
  int16_t len;
  enum MeasurementStates
  {
    notConnected = -1,
    idle = 0,
    t1, t2, t3,     // receive buffer
    s1, s2,         // send buffer
    d1, d2, d3     // diagnostic health checks
  };

  int16_t barometer;

  uint8_t mb_fc;  // modbus function code
  int16_t mb_ref; // modbus reference
  int16_t mb_cnt; // modbus size
  int8_t mux;     // tcp stream mux

  int memstrn(const char *needle);
  int memrem(int n);
  int memtoi(int n);

  uint16_t callsTotal;
  uint16_t callsError;
  uint16_t callsConnect;
};
