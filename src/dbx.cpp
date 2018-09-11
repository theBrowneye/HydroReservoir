#include "dbx.h"

dbxRegisters::dbxRegisters()
{
    for (uint16_t i = 0; i < dbxMemorySize; i++)
    {
        badValue[i] = false;    // default to good value
    }
}

float dbxRegisters::getValueFlt(uint16_t offset)
{
    float *p = reinterpret_cast<float *>(&reg[offset]);
    return *p;
} 

uint16_t dbxRegisters::getValueInt(uint16_t offset)
{
    return reg[offset];
}

void dbxRegisters::setValueFlt(uint16_t offset, float v)
{
    * (reinterpret_cast<float *>(&reg[offset])) = v;
}

uint16_t* dbxRegisters::getRegMap()
{
    return reg;
}

uint16_t& dbxRegisters::operator[] (uint16_t x)
{
    return reg[x];
}

void dbxRegisters::setValueInt(uint16_t offset, uint16_t v)
{
    reg[offset] = v;
}

bool dbxRegisters::isBadValue(uint16_t offset)
{
    return badValue[offset];
}

void dbxRegisters::setBadValue(uint16_t offset, bool b)
{
    badValue[offset] = b;
}

String dbxRegisters::asStringF(uint16_t offset, unsigned char spec)
{
    String v = isBadValue(offset) ? "????" : String(getValueFlt(offset), spec);
    return v;
}

String dbxRegisters::asStringI(uint16_t offset, unsigned char spec)
{
    String v = isBadValue(offset) ? "????" : String(getValueInt(offset), spec);
    return v;
}

dbxRegisters& regmap = dbxRegisters::getInstance();

dbxMenu::dbxMenu() 
{
     for (auto & s : screens) {
         s = nullptr;
     }  
     numSets = 0; 
     setPosition = 0;
 }

void dbxMenu::addScreenSet(dbxScreenSet * dbs)
{
    if (numSets >= MaxScreenSets) return;
    screens[numSets++] = dbs;
}

void dbxMenu::nextScreenSet() 
{
    if (++setPosition >= MaxScreenSets)
        setPosition = 0;
}

void dbxMenu::setMenuPosition(uint16_t p)
{
    menuPosition = p;
    while (menuPosition >= getMenuSize())
        menuPosition -= getMenuSize();
}

int dbxMenu::getMenuSize() 
{
    return screens[setPosition][0];
}

uint16_t h_i8toi16(uint8_t h, uint8_t l)
{
    uint16_t r = h;
    return r * 256 + l;
}