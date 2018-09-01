/*
* GccApplication1.cpp
*
* Created: 24/03/2014 3:11:57 PM
*  Author: Gerry
*
* 2015-10-04 Strip back to PH, EC, water temp
* 2015-10-04 Merged in latest Arduino release 1.6.5, UIP Ethernet, Fixed cxa_guard (new.cpp)
* 2015-10-16 Replace Ethernet & Mudbus with new RS482 modbus slave
* 2016-06-08 Strip out power, time and other modules not being used
* 2016-06-11 Add sensor calibration mode
* 2018-08-14 Copied to new project
*/

// defines for include files
#define U8X8_HAVE_HW_I2C

#include <Wire.h>
#include "ConeShed.h"
#include "avr/wdt.h"
#include "Arduino.h"
#include "One_Temperature.h"
#include "IO_Assignment.h"
#include "Serial_Device.h"
#include "Encoder.h"
#include "HON_Sensor.h"
#include "ErrorStatus.h"
#include "CycleTime.h"
#include "RunTime.h"
#include "U8x8lib.h"
#include "Sonar.h"
#include "dbxMemoryMap.h"
#include "Bounce2.h"
#include "ModbusESP8266AT.h"


// manage wdt
uint8_t mcusr_mirror __attribute__((section(".noinit")));
uint8_t avr_restarts __attribute__((section(".noinit")));

void get_mcusr(void) __attribute__((naked)) __attribute__((section(".init3")));

void get_mcusr(void)
{
	mcusr_mirror = MCUSR;
	MCUSR = 0;
	wdt_disable();
}

void parse_restart_flags()
{
	if (mcusr_mirror & _BV(PORF))
	{									 // test for power-on restart
		mcusr_mirror = avr_restarts = 0; // reset restart counter
		Serial.println("Restart reason: Power On");
	}
	else
	{
		Error.restarts = ++avr_restarts;
		Serial.print("Restart reason: [");
		if (mcusr_mirror & _BV(JTRF))
			Serial.print("JTRF ");
		if (mcusr_mirror & _BV(WDRF))
			Serial.print("WDRF ");
		if (mcusr_mirror & _BV(BORF))
			Serial.print("BORF ");
		if (mcusr_mirror & _BV(EXTRF))
			Serial.print("EXTRF ");
		Serial.print(mcusr_mirror, HEX);
		Serial.println("]");
	}

	Error.jtrf = mcusr_mirror & _BV(JTRF);
	Error.wdrf = mcusr_mirror & _BV(WDRF);
	Error.borf = mcusr_mirror & _BV(BORF);
	Error.extrf = mcusr_mirror & _BV(EXTRF);
}

// declarations
void Menu_Show();

// create objects
OneTemperature water_temperature(wtrTemp);
OneTemperature case_temperature(casTemp);
PHSensor ph_sensor(phSerial);
ECSensor ec_sensor(ecSerial);
Encoder enc(encPinA, encPinB);
Bounce encBtn;
HONSensor hon_sensor(honID);
CycleTime cycleTime;
RunTime runTime;
Timer menu_timer;
U8X8_SSD1306_128X64_ALT0_HW_I2C  u8x8(/* reset=*/U8X8_PIN_NONE); // oled on standard I2C pins
Sonar ms(dstTrig, dstEcho, dstMaxDistance);
ModbusDevice mb(mbSerial);

// the setup routine runs once when you press reset:
void setup()
{
	// initialize the digital pin as an output.
	Serial.begin(115200);
	LogError("Starting");

	// process restart
	parse_restart_flags();

	// TODO: change ms back to use watertemperature
	// set up instrument readings
	water_temperature.setValuePtr(dbWatTemp);
	case_temperature.setValuePtr(dbCaseTemp);
	ph_sensor.setValuePtr(dbPHSensor);
	ec_sensor.setValuePtr(dbECSensor);
	hon_sensor.setValuePtr(dbHonSensor);
	hon_sensor.begin();
	ms.setValuePtr(dbSonar);
	cycleTime.setValuePtr(dbCycleTime);
	runTime.setValuePtr(dbRunTime);
	encBtn.attach(encButton, INPUT_PULLUP);
	encBtn.interval(25);		// debounce interval
	u8x8.begin();

	// restore saved information
	// mm.readFromFlash();

	// set up UI objects
	enc.begin();
	enc.setMenuRange(15);		// TODO: remove this after testing
	u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
	u8x8.clear();

	// set up modbus slave
	mb.setValuePtr(dbModBus);
	mb.begin();

	// set up watchdog timer
	wdt_enable(WDTO_8S);
}

// the loop routine runs over and over again forever:
void loop()
{
	// reset watchdog
	wdt_reset();

	// tick each measurement
	case_temperature.tick();
	water_temperature.tick();
	ph_sensor.tick();
	ec_sensor.tick();
	hon_sensor.tick();
	cycleTime.tick();
	runTime.tick();
	ms.tick();
	encBtn.update();

	// display menus
	if (menu_timer.timeOut())
	{
		Menu_Show();
		menu_timer.startTimer(MenuTimeOut);
	}

	// process Modbus
	mb.tick();

	// save any changed variables
	// mm.saveToFlash();
}

int freeRam()
{
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

// process menu and displays
void Menu_Show()
{
	u8x8.setCursor(0, 0);
	u8x8.print(regmap.getValueInt(27));
	u8x8.print(",");
	u8x8.print(regmap.getValueInt(29));
	u8x8.print(",");
	u8x8.print(regmap.getValueInt(28));


	u8x8.setCursor(0, 1);
	u8x8.print("out t: ");
	u8x8.print(regmap.getValueFlt(9), 1);
	u8x8.print("  ");

	u8x8.setCursor(0, 2);
	u8x8.print("ram: ");
	u8x8.print(freeRam());
	u8x8.print("  ");

	u8x8.setCursor(0, 3);
	u8x8.print("level: ");
	u8x8.print(regmap.getValueFlt(3), 0);
	u8x8.print("  ");

	// Serial.print("sonar: ");
	// Serial.println(regmap.getValueFlt(3));
}
