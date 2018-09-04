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
uint8_t avr_restarts __attribute__((section(".noinit")));
uint8_t int_restarts __attribute__((section(".noinit")));

// install catchall isr
// send "!" to serial port
ISR(BADISR_vect)
{
	int_restarts++;
	for (;;)
		UDR0 = '!';
}

void parse_restart_flags()
{
	uint8_t mcusr_mirror = MCUSR;
	Serial.print("Restart reason: [");
	if (mcusr_mirror & _BV(PORF))
	{
		Serial.print("PORF ");
		avr_restarts = 0; // reset restart counter
	}
	Error.restarts = ++avr_restarts;
	if (mcusr_mirror & _BV(JTRF))
	{
		Serial.print("JTRF ");
		Error.jtrf = mcusr_mirror & _BV(JTRF);
	}
	if (mcusr_mirror & _BV(WDRF))
	{
		Serial.print("WDRF ");
		Error.wdrf = mcusr_mirror & _BV(WDRF);
	}
	if (mcusr_mirror & _BV(BORF))
	{
		Serial.print("BORF ");
		Error.borf = mcusr_mirror & _BV(BORF);
	}
	if (mcusr_mirror & _BV(EXTRF))
	{
		Serial.print("EXTRF ");
		Error.extrf = mcusr_mirror & _BV(EXTRF);
	}
	Serial.print(mcusr_mirror, HEX);
	Serial.println("]");
	Serial.print("starts: ");
	Serial.println(Error.restarts);
	MCUSR = 0;
}

// declarations
void Menu_Show(int, bool);

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
U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE); // oled on standard I2C pins
Sonar ms(dstTrig, dstEcho, dstMaxDistance);
ModbusDevice mb(mbSerial);

// the setup routine runs once when you press reset:
void setup()
{
	// initialize the digital pin as an output.
	wdt_disable();
	Serial.begin(115200);

	// process restart
	parse_restart_flags();
	wdt_disable();

	// TODO: impelement WDT counters
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
	encBtn.interval(25); // debounce interval
	u8x8.begin();

	// restore saved information
	// mm.readFromFlash();

	// set up UI objects
	enc.begin();
	enc.setMenuRange(15); // TODO: remove this after testing
	u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
	u8x8.clear();

	// set up modbus slave
 	mb.setValuePtr(dbModBus);
	mb.begin();

	// set up watchdog timer
	//wdt_enable(WDTO_8S);
}

// the loop routine runs over and over again forever:
void loop()
{
	// reset watchdog
	//wdt_reset();

	// tick each measurement
	case_temperature.tick();
	water_temperature.tick();
	// ph_sensor.tick();
	// ec_sensor.tick();
	hon_sensor.tick();
	cycleTime.tick();
	runTime.tick();
	ms.tick();
	encBtn.update();

	// display menus
	if (menu_timer.timeOut())
	{
		Menu_Show(enc.getMenu(), encBtn.read());
		menu_timer.startTimer(MenuTimeOut);
	}

	// process Modbus
	mb.tick();

	// save any changed variables
	// mm.saveToFlash();
}

// process menu and displays
void Menu_Show(int menuPos, bool b)
{
	bool action = !b;
	static int lastMenu = -1;
	if (lastMenu != menuPos)
	{
		lastMenu = menuPos;
		u8x8.clear();
	}

	switch (menuPos)
	{
	case 0:
		u8x8.setCursor(0, 0);
		u8x8.print("Water Quality");

		u8x8.setCursor(0, 1);
		u8x8.print("  ph:");
		u8x8.print(regmap.getValueFlt(dbPHSensor), 1);
		u8x8.print("  ");

		u8x8.setCursor(0, 2);
		u8x8.print("  ec:");
		u8x8.print(regmap.getValueFlt(dbECSensor), 0);
		u8x8.print("  ");

		u8x8.setCursor(0, 3);
		u8x8.print(" lvl:");
		u8x8.print(regmap.getValueFlt(dbSonar + 2), 0);
		u8x8.print("  ");
		break;

	case 1:
		u8x8.setCursor(0, 0);
		u8x8.print("Modem");

		u8x8.setCursor(0, 1);
		u8x8.print("call:");
		u8x8.print(regmap.getValueInt(dbModBus));
		u8x8.print("  ");

		u8x8.setCursor(0, 2);
		u8x8.print("cnct:");
		u8x8.print(regmap.getValueInt(dbModBus + 1));
		u8x8.print("  ");

		u8x8.setCursor(0, 3);
		u8x8.print("errs:");
		u8x8.print(regmap.getValueInt(dbModBus + 2));
		u8x8.print("  ");
		break;

	case 2:
		u8x8.setCursor(0, 0);
		u8x8.print("Environment");

		u8x8.setCursor(0, 1);
		u8x8.print("temp:");
		u8x8.print(regmap.getValueFlt(dbHonSensor), 1);
		u8x8.print("  ");

		u8x8.setCursor(0, 2);
		u8x8.print(" hum:");
		u8x8.print(regmap.getValueFlt(dbHonSensor + 2), 0);
		u8x8.print("  ");

		u8x8.setCursor(0, 3);
		u8x8.print("case:");
		u8x8.print(regmap.getValueFlt(dbCaseTemp), 1);
		u8x8.print("  ");
		break;

	case 3:
		u8x8.setCursor(0, 0);
		u8x8.print("Sonar");

		u8x8.setCursor(0, 1);
		u8x8.print("raw:");
		u8x8.print(regmap.getValueFlt(dbSonar), 0);
		u8x8.print("  ");

		u8x8.setCursor(0, 2);
		u8x8.print("span:");
		u8x8.print(regmap.getValueFlt(dbSonar + 4), 0);
		u8x8.print("  ");

		u8x8.setCursor(0, 3);
		u8x8.print(" off:");
		u8x8.print(regmap.getValueFlt(dbSonar + 6), 0);
		u8x8.print("  ");
		break;

		// case 4:
		// 	u8x8.setCursor(0, 0);
		// 	u8x8.print("System");

		// 	u8x8.setCursor(0, 1);
		// 	u8x8.print("flag:");
		// 	u8x8.print(regmap.getValueInt(23), HEX);
		// 	u8x8.print("  ");

		// 	u8x8.setCursor(0, 2);
		// 	u8x8.print("cycl:");
		// 	u8x8.print(regmap.getValueInt(24));
		// 	u8x8.print("  ");

		// 	u8x8.setCursor(0, 3);
		// 	u8x8.print(" run:");
		// 	u8x8.print(regmap.getValueInt(25));
		// 	u8x8.print("  ");
		// 	break;

	default:
		u8x8.setCursor(0, 0);
		u8x8.print("Unknown");
	}

	// u8x8.setCursor(0, 1);
	// u8x8.print("out t: ");
	// u8x8.print(regmap.getValueFlt(9), 1);
	// u8x8.print("  ");

	// Serial.print("sonar: ");
	// Serial.println(regmap.getValueFlt(3));
}
