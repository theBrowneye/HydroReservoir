#include "dbx.h"

dbxRegisters::dbxRegisters()
{
    for (uint16_t i = 0; i < dbxMemorySize; i++)
    {
        badValue[i] = true;
    }
}

float dbxRegisters::getValueFlt(uint16_t offset)
{
    float *p = reinterpret_cast<float *>(hreg + offset);
    return *p;
} 

uint16_t dbxRegisters::getValueInt(uint16_t offset)
{
    return hreg[offset];
}

bool dbxRegisters::isBadValue(uint16_t offset)
{
    return badValue[offset];
}

void dbxRegisters::setBadValue(uint16_t offset, bool b)
{
    badValue[offset] = b;
}

dbxRegisters& regmap = dbxRegisters::getInstance();
dbxMemPtr hreg = regmap.reg;

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