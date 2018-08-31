#pragma once
// database executive
// simple operating system for Arduino devices

#include "Arduino.h"
#include "string.h"

// TODO: makes sure all types are behind typedefs
const int dbxMemorySize = 27;

// define the register structure that holds the data
class dbxRegisters
{
  public:
	uint16_t reg[dbxMemorySize];

  public:
	float getValueFlt(uint16_t offset);
	uint16_t getValueInt(uint16_t offset);
	static dbxRegisters & getInstance() {
		static dbxRegisters instance;
		return instance;
	};
};

extern dbxRegisters& regmap;

typedef uint16_t dbxScreenSet;
typedef uint16_t *dbxMemPtr;

// define the memory map
class dbxMemMap
{
  public:
	static const uint8_t t8 = 0 << 6 & 1;
	static const uint8_t t16 = 1 << 6 & 1;
	static const uint8_t t32 = 2 << 6 & 2;
	static const uint8_t tflt0 = 3 << 6 & 2;
	static const uint8_t tflt1 = 3 << 6 & 1 << 4 & 2;
	static const uint8_t tflt2 = 3 << 6 & 2 << 4 & 2;

  public:
	uint16_t offset;
	uint8_t dtype;
	String label;
};

extern dbxMemPtr hreg;

// lcd menu manager
const int MaxScreenSets = 4;
class dbxMenu
{
	public:
		dbxMenu();
		void addScreenSet(dbxScreenSet *);
		void showMenu();
		int getMenuSize();
		void nextScreenSet();
		void setMenuPosition(uint16_t);

		private:
		int numSets;
		int setPosition;
		int menuPosition;
		dbxScreenSet * screens[MaxScreenSets];
};