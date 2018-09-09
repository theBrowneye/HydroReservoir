#pragma once
// assign IO pins
#include <Arduino.h>

// Encoder
const byte encPinA = A8;    // right
const byte encPinB = A9;    // left
const byte encButton = A10; // push button

// LCD Module
const byte oledID = 0x3C; // i2c address of oled module
const byte oledSDA = 20;  // standard I2C SDA, shared with oled
const byte oledSDL = 21;  // standard I2C SDL

// ultrasonic distance sensor
const byte dstTrig = 7;
const byte dstEcho = 6;
const unsigned int dstMaxDistance = 500;

// Water measurement
const byte phRX = 18;
const byte phTX = 19;
const byte ecRX = 16;
const byte ecTX = 17;
const byte wtrTemp = 2;
const byte casTemp = 26;
HardwareSerial *phSerial = &Serial1;
HardwareSerial *ecSerial = &Serial2;

// Honeywell sensor
const byte honSDA = 20;  // standard I2C SDA, shared with oled
const byte honSDL = 21;  // standard I2C SDL
const byte honID = 0x27; // i2c address of honeywell sensor

// MODBUS server
HardwareSerial& mbSerial = Serial3;
