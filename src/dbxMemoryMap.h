#pragma once
// define memory map for dbx

#include "dbx.h"

const uint16_t dbSonar = 1;
const uint16_t dbHonSensor = 9;
const uint16_t dbWatTemp = 13;
const uint16_t dbPHSensor = 15;
const uint16_t dbECSensor = 17;
const uint16_t dbCaseTemp = 21;
const uint16_t dbCycleTime = 24;
const uint16_t dbRunTime = 25;
const uint16_t dbModBus = 27;

dbxMemMap mem[] = {
    /* 40001 */ {dbSonar + 0, dbxMemMap::tflt0, "tank level - raw"},
    /* 40003 */ {dbSonar + 2, dbxMemMap::tflt0, "tank level - filt"},
    /* 40005 */ {dbSonar + 3, dbxMemMap::tflt2, "sonar span"},
    /* 40007 */ {dbSonar + 4, dbxMemMap::tflt2, "sonar offset"},
    /* 40009 */ {dbHonSensor + 0, dbxMemMap::tflt1, "out temperature"},
    /* 40011 */ {dbHonSensor + 2, dbxMemMap::tflt0, "out humidity"},
    /* 40013 */ {dbWatTemp + 0, dbxMemMap::tflt1, "water temperature"},
    /* 40015 */ {dbPHSensor + 0, dbxMemMap::tflt1, "water pH"},
    /* 40017 */ {dbPHSensor + 2, dbxMemMap::t16, "pH calibrate"},
    /* 40018 */ {dbECSensor + 0, dbxMemMap::tflt0, "water EC"},
    /* 40020 */ {dbECSensor + 2, dbxMemMap::t16, "EC calibrate"},
    /* 40021 */ {dbCaseTemp + 0, dbxMemMap::tflt1, "case temperature"},
    /* 40023 */ {23, dbxMemMap::t16, "System flags"},
    /* 40024 */ {24, dbxMemMap::t16, "cycle time"},
    /* 40025 */ {25, dbxMemMap::t16, "run time"},
    /* 40026 */ {26, dbxMemMap::t8, "testy"},
    /* 40027 */ {dbModBus + 0, dbxMemMap::t16, "modbus calls"},
    /* 40028 */ {dbModBus + 1, dbxMemMap::t16, "modbus errors"}
};

// screensets
/// first number is the count of screen set items (not including the count)
dbxScreenSet ss1[] = {5, 15, 17, 3, 9, 11};

// const int dbSonar = 0;
// sonar.reg_memory(dbSonar + 0, dbxMemMap::tflt0, "tank level - raw");
// sonar.reg_memory(dbSonar + 2, dbxMemMap::tflt2, "tank level - filt");