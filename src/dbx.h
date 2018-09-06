#pragma once
// database executive
// simple operating system for Arduino devices

#include "Arduino.h"
#include "string.h"

// TODO: makes sure all types are behind typedefs
// TODO: Work out how to automatically update this item from the memory map configuration
const uint16_t dbxMemorySize = 35; // make sure this is correct

// http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
class BitVector
{
  public:
	BitVector(uint16_t n)
	{
		bv = new uint8_t(n / 8 + 1);
	}
	~BitVector() { delete[] bv; }
	void set(uint16_t n)
	{
		bv[n / 8] |= (1 << (n % 8));
	}
	void clr(uint16_t n)
	{
		bv[n / 8] &= ~(1 << (n % 8));
	}
	bool get(uint16_t n)
	{
		return bv[n / 8] & (1 << (n % 8));
	}

  private:
	uint8_t *bv;
};

// define the register structure that holds the data
class dbxRegisters
{
  public:
	dbxRegisters();
	uint16_t reg[dbxMemorySize];
	bool badValue[dbxMemorySize];

  public:
	float getValueFlt(uint16_t offset);
	uint16_t getValueInt(uint16_t offset);
	bool isBadValue(uint16_t offset);
	void setBadValue(uint16_t offset, bool b = true);
	static dbxRegisters &getInstance()
	{
		static dbxRegisters instance;
		return instance;
	};
};

typedef uint16_t dbxScreenSet;
typedef uint16_t *dbxMemPtr;

extern dbxRegisters &regmap;
extern dbxMemPtr hreg;

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
	dbxScreenSet *screens[MaxScreenSets];
};