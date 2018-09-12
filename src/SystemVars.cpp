#include "SystemVars.h"
#include "ErrorStatus.h"
#include "dbx.h"

extern uint8_t avr_restarts;
extern uint8_t int_restarts;

// default constructor
SystemVars::SystemVars()
{
} 

const long UpdateRate = 1000;

void SystemVars::tick()
{
	if (!taskTimer.timeOut())
		return;

	taskTimer.startTimer(UpdateRate);

 	setValueInt(avr_restarts, 0);
    setValueInt(int_restarts, 1);
    setValueInt(Error.flags, 2);

	// check for retained operations
	uint16_t retCmd = getValueInt(3);
	if (retCmd == 1)
	{
		regmap.saveRetainedValues();
		setValueInt(0, 3);  // reset flag
		Serial.println("Save retained");
	}
	else if (retCmd == 2)
	{
		regmap.loadRetainedValues();
		setValueInt(0, 3);  // reset flag
		Serial.println("Load retained");
	}
}
