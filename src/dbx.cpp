#include "dbx.h"

float dbxRegisters::getValueFlt(uint16_t offset)
{
    float *p = reinterpret_cast<float *>(hreg + offset);
    return *p;
} 

uint16_t dbxRegisters::getValueInt(uint16_t offset)
{
    return hreg[offset];
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