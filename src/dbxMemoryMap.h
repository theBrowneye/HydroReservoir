#pragma once
// define memory map for dbx
// TODO: Implement EEPROM saves

#include "dbx.h"

const uint16_t dbSonar = 1;
const uint16_t dbHonSensor = 9;
const uint16_t dbWatTemp = 13;
const uint16_t dbPHSensor = 15;
const uint16_t dbECSensor = 19;
const uint16_t dbCaseTemp = 23;
const uint16_t dbCycleTime = 25;
const uint16_t dbRunTime = 27;
const uint16_t dbModBus = 29;
const uint16_t dbSystemFlags = 32;

// dbxMemMap mem[] = {
//     /* empty */ 
//     /* 40001 */ {dbSonar + 0, dbxMemMap::tflt0, "tank level - raw"},
//     /* 40003 */ {dbSonar + 2, dbxMemMap::tflt0, "tank level - filt"},
//     /* 40005 */ {dbSonar + 4, dbxMemMap::tflt2, "sonar span"},
//     /* 40007 */ {dbSonar + 6, dbxMemMap::tflt2, "sonar offset"},
//     /* 40009 */ {dbHonSensor + 0, dbxMemMap::tflt1, "out temperature"},
//     /* 40011 */ {dbHonSensor + 2, dbxMemMap::tflt0, "out humidity"},
//     /* 40013 */ {dbWatTemp + 0, dbxMemMap::tflt1, "water temperature"},
//     /* 40015 */ {dbPHSensor + 0, dbxMemMap::tflt1, "water pH"},
//     /* 40017 */ {dbPHSensor + 2, dbxMemMap::t16, "pH calibrate"},
//     /* 40019 */ {dbECSensor + 0, dbxMemMap::tflt0, "water EC"},
//     /* 40021 */ {dbECSensor + 2, dbxMemMap::t16, "EC calibrate"},
//     /* 40023 */ {dbCaseTemp + 0, dbxMemMap::tflt1, "case temperature"},
//     /* 40025 */ {dbCycleTime, dbxMemMap::tflt2, "cycle time"},
//     /* 40027 */ {dbRunTime, dbxMemMap::tflt1, "run time"},
//     /* 40029 */ {dbModBus + 0, dbxMemMap::t16, "modbus calls"},
//     /* 40030 */ {dbModBus + 1, dbxMemMap::t16, "modbus errors"},
//     /* 40031 */ {dbModBus + 2, dbxMemMap::t16, "num connects"},
//     /* 40032 */ {dbSystemFlags + 0, dbxMemMap::t16, "Power on restarts"}
//     /* 40033 */ {dbSystemFlags + 1, dbxMemMap::t16, "Interrupt restarts"}
//     /* 40034 */ {dbSystemFlags + 2, dbxMemMap::t16, "System flags"}
//     /* 40035 */ 
// };