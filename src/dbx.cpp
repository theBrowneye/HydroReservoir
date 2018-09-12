#include "dbx.h"
#include "EEPROM.h"

dbxRegisters::dbxRegisters()
{
    for (uint16_t i = 0; i < dbxMemorySize; i++)
    {
        badValue[i] = false;    // default to good value
        retained[i] = false;
    }
}

bool dbxRegisters::loadRetainedValues()
{
    for (uint16_t i = 0; i < dbxMemorySize; i++)    
    {
        if (isRetained(i))
        {
            uint8_t h = EEPROM.read(i*2 + 0);
            uint8_t l = EEPROM.read(i*2 + 1);
            reg[i] = h_i8toi16(h, l);
        }
    }
    // TODO:  implement error handling and return values
    return true;
}

bool dbxRegisters::saveRetainedValues()
{
    for (uint16_t i = 0; i < dbxMemorySize; i++)    
    {
        if (isRetained(i))
        {
            uint8_t h = reg[i] / 256;
            uint8_t l = reg[i] % 256;
            EEPROM.write(i*2 + 0, h);
            EEPROM.write(i*2 + 1, l);
        }
    }
    // TODO:  implement error handling and return values
    return true;
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

bool dbxRegisters::isRetained(uint16_t offset)
{
    return retained[offset];
}

void dbxRegisters::setRetained(uint16_t offset, bool b)
{
    retained[offset] = b;
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