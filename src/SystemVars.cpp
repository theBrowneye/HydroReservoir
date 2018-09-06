#include "SystemVars.h"
#include "ErrorStatus.h"

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
}
