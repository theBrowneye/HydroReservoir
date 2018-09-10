#include "ModbusESP8266AT.h"

ModbusDevice::ModbusDevice(HardwareSerial &s) : sd(s), wifi(s, 9600)
{
}

void ModbusDevice::begin()
{
    barometer = callsConnect = callsError = callsTotal = 0;
    setValueInt(callsTotal, 0);
    setValueInt(callsError, 1);
    setValueInt(callsConnect, 2);
    setValueInt(barometer, 3);
    state = ModbusDevice::d1;   // force a diagnostic on start
    barometer = BarometerFail;  // preset barometer to force a hard restart if required
}

int ModbusDevice::getState()
{
    return state;
}

// TODO: implement error checking
// establish communications to access point
// TODO: implement connect and other long routines with timer controls (expect object??)
// TODO: Add sensor to show if we are connected, and perhaps if there are any problems that need to be fixed
// TODO: implement state machine for connections to prevent app from hanging on connect
// TODO: implement better diagnostics to see what's happening during connect issues
// TODO: don't reconnect unnecessarily - check connect status in diag call isntead of forcing new connection

// if connectStrength == true then force a device reset
bool ModbusDevice::connect(bool connectStrength)
{
    bool retval = true;
    Serial.println("ModbusDevice::begin");

    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());

    if (connectStrength)
    {
        Serial.println("restart ESP-01");
        wifi.restart();
    }

    if (wifi.setOprToStationSoftAP())
    {
        Serial.print("to station + softap ok\r\n");
    }
    else
    {
        Serial.print("to station + softap err\r\n");
        retval = false;
    }

    if (wifi.joinAP(SSID, PASSWORD))
    {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");
        Serial.println(wifi.getLocalIP().c_str());
    }
    else
    {
        Serial.print("Join AP failure\r\n");
        retval = false;
    }

    if (wifi.enableMUX())
    {
        Serial.print("multiple ok\r\n");
    }
    else
    {
        Serial.print("multiple err\r\n");
        retval = false;
    }

    if (wifi.startTCPServer(502))
    {
        Serial.print("start tcp server ok\r\n");
    }
    else
    {
        Serial.print("start tcp server err\r\n");
        retval = false;
    }

    if (wifi.setTCPServerTimeout(60))
    {
        Serial.print("set tcp server timout 60 seconds\r\n");
    }
    else
    {
        Serial.print("set tcp server timout err\r\n");
        retval = false;
    }
    return retval;
}

void ModbusDevice::reset()
{
    setValueInt(++callsError, 1);
    Serial.println("reset()");
    restart();
}

void ModbusDevice::restart()
{
    rbufflen = 0;
    state = ModbusDevice::idle;
    taskTimer.stopTimer();                  // turn off task timer until we get a start segment
}

// process modem messages
void ModbusDevice::tick()
{
    if (isFailed())                         
    {
        if (!connect(true))  // try to connect (hard connect if device failed)
        {
            Serial.println("connect failed");
            barometer = BarometerFail;
        }
        setValueInt(++callsConnect, 2);
        setValueInt(barometer = 0, 3);
        restart();
    }

    // issue a diagnostic if not in the middle of a message cycle
    if (!isBusy() && diagTimer.timeOut())
    {
        diagTimer.startTimer(diagTimeOut);      // (re)set diagnostic timer
        taskTimer.startTimer(msgTimeOut);        // set message timeout
        state = ModbusDevice::d1;
    }

    // check for timeout while waiting for something.  This means somethign broke
    if (isBusy() && taskTimer.timeOut())
    {
        // check if diagnostic has timed out
        if (state == ModbusDevice::d1 || state == ModbusDevice::d2 || state == ModbusDevice::d3) 
        {
            // set barometer high so we do a hard reset next time.
            Serial.println("diagnostic timeout");
            barometer = 100;
        }
        else
        {
            Serial.println("read timeout");
        }
        reset();
    }

    // prevent buffer from growing too large
    if (rbufflen >= bufferSize)
        reset();

    // read any incoming data
    while (sd.available())
    {
        char c = sd.read();
        rbuffer[rbufflen++] = c;
        // if (isprint(c))
        //     Serial.print(c);
        // else
        //     Serial.print(c, HEX);
    }

    // +IPD,id,len:data
    // look for a response header
    int l = rbufflen;
    if (isIdle() && rbufflen >= 5)
    {
        l = memstrn("+IPD,");
        if (l > 0)
        {
            memrem(l + 5); // strip leading portion
            taskTimer.startTimer(msgTimeOut);        // set message timeout
            state = ModbusDevice::t1;
        }
    }

    if (state == ModbusDevice::t1 && rbufflen >= 2)
    {
        l = memstrn(",");
        if (l > 0)
        {
            mux = memtoi(l);
            memrem(l + 1);
            state = ModbusDevice::t2;
        }
    }

    if (state == ModbusDevice::t2 && rbufflen > 2)
    {
        l = memstrn(":");
        if (l > 0)
        {
            len = memtoi(l);
            memrem(l + 1);
            state = ModbusDevice::t3;
        }
    }

    if (state == ModbusDevice::t3 && rbufflen >= len)
    {
        bool rsend = false;

        if (len >= 12)
        {
            mb_fc = static_cast<uint8_t>(rbuffer[7]);
            mb_ref = static_cast<uint8_t>(rbuffer[8]) * 256 + static_cast<uint8_t>(rbuffer[9]);

            switch (mb_fc)
            {
            // FC3 - read multiple registers
            // len-in = 12
            // len-out = 9 + num * 2
            // map ! 00 ! 01 ! 02 ! 03 ! 04 ! 05 ! 06 ! 07 ! 08 ! 09 ! 10 ! 11 ! 12 ! 13 ! 14 ! 15
            // in  ! id ! id ! 00 ! 00 ! 00 ! len! un ! 03 ! R-H! R-L! C-H! C-L!
            // out ! id ! id ! 00 ! 00 ! 00 ! len! un ! 03 ! byt! val! val!
            case 3:
                mb_cnt = static_cast<uint8_t>(rbuffer[10]) * 256 + static_cast<uint8_t>(rbuffer[11]);
                for (int i = 0; i < 12; i++)
                    sbuffer[i] = rbuffer[i];
                sbuffer[5] = 3 + mb_cnt * 2; // length
                sbuffer[8] = mb_cnt * 2;     // number of value bytes
                sbufflen = 9 + mb_cnt * 2;   // length of out buffer

                union {
                    uint16_t i;
                    uint8_t b[2];
                } uu;

                for (auto i = 0; i < mb_cnt; i++)
                {
                    uu.i = regmap[i + mb_ref];
                    sbuffer[9 + i * 2] = uu.b[0];
                    sbuffer[10 + i * 2] = uu.b[1];
                }

                rsend = true;

                // Serial.print("fc3 ");
                break;

            // FC16 - write multiple registers
            // len-in = 12 + num * 2
            // len-out = 12
            // TODO: test FC3 and FC16
            // map ! 00 ! 01 ! 02 ! 03 ! 04 ! 05 ! 06 ! 07 ! 08 ! 09 ! 10 ! 11 ! 12 ! 13 ! 14 ! 15
            // in  ! id ! id ! 00 ! 00 ! 00 ! len! un ! 16 ! R-H! R-L! C-H! C-L! VAL! VAL! ...
            // out ! id ! id ! 00 ! 00 ! 00 ! len! un ! 16 ! R-H! R-L! C-H! C-L!
            case 16:
                mb_cnt = static_cast<uint8_t>(rbuffer[10]) * 256 + static_cast<uint8_t>(rbuffer[11]);
                for (int i = 0; i < 12; i++)
                    sbuffer[i] = rbuffer[i];
                sbuffer[5] = 6; // length
                for (int i = 0; i < mb_cnt; i++) {
                    regmap[mb_ref+i] = sbuffer[12+i*2] * 256 + sbuffer[13+i*2];
                    Serial.print("bPtr[");
                    Serial.print(mb_ref+i);
                    Serial.print("]=");
                    Serial.println(regmap[mb_ref+i],HEX);
                }
                rsend = true;
                break;

            // FC6 - write single registers
            // len-in = 12 
            // len-out = 12
            // map ! 00 ! 01 ! 02 ! 03 ! 04 ! 05 ! 06 ! 07 ! 08 ! 09 ! 10 ! 11 ! 12 ! 13 ! 14 ! 15
            // in  ! id ! id ! 00 ! 00 ! 00 ! len! un !  6 ! R-H! R-L! VAL! VAL!
            // out ! id ! id ! 00 ! 00 ! 00 ! len! un !  6 ! R-H! R-L! VAL! VAL!
            case 6:
                for (int i = 0; i < 12; i++)
                    sbuffer[i] = rbuffer[i];
                sbuffer[5] = 6;             // length
                sbufflen = 12;              // length of out buffer
                regmap[mb_ref] = sbuffer[10] * 256 + sbuffer[11];
                Serial.print("bPtr[");
                Serial.print(mb_ref);
                Serial.print("]=");
                Serial.println(regmap[mb_ref],HEX);
                rsend = true;
                break;
            }
        }
        if (rsend)
        {
            state = ModbusDevice::s1;
        }
        else
        {
            reset();
        }
    }

    if (state == ModbusDevice::s1)
    {
        sd.print("AT+CIPSEND=");
        sd.print(mux);
        sd.print(",");
        sd.println(sbufflen);
        rbufflen = 0;
        state = ModbusDevice::s2;
    }

    if (state == ModbusDevice::s2 && rbufflen > 22)
    {
        l = memstrn(">");
        if (l > 0)
        {
            sd.write(sbuffer, sbufflen);
            setValueInt(callsTotal++, 0);
            restart();
        }
    }

    if (state == ModbusDevice::d1)
    {
        sd.println("AT+CIPSTATUS");
        state = ModbusDevice::d2;
    }

    // response could be "busy" or "ERROR"
    if (state == ModbusDevice::d2 && rbufflen >= 7)
    {
        l = memstrn("STATUS:");
        if (l > 0)
        {
            memrem(l + 7);
            state = ModbusDevice::d3;
        }
    }

    // looking for "STATUS:3" to show connected OK
    if (state == ModbusDevice::d3 && rbufflen >= 1)
    {
        l = memstrn("\r\n");
        if (l > 0)
        {
            int status = memtoi(l);
            memrem(l + 2);
            // any other status than 3 means it's not fully connected
            if (status != 3) 
                barometer++;
            else 
                barometer--;
            state = ModbusDevice::idle;
            if (barometer >= BarometerFail) state = ModbusDevice::notConnected;
            if (barometer < 0) barometer = 0;
            setValueInt(barometer, 3);
            Serial.print("diag:");
            Serial.print(status);
            Serial.print(",");
            Serial.println(barometer);
        }
    }
}

int ModbusDevice::memstrn(const char *needle)
{
    uint8_t *p;
    uint16_t needlesize = strlen(needle);

    for (p = rbuffer; p <= (rbuffer - needlesize + rbufflen); p++)
    {
        if (memcmp(p, needle, needlesize) == 0)
            return p - rbuffer; /* found */
    }
    return -1;
}

int ModbusDevice::memrem(int n)
{
    if (n <= 0)
        return 0;
    n = min(rbufflen, n);

    for (int i = 0; i < rbufflen - n; i++)
        rbuffer[i] = rbuffer[i + n];
    rbufflen -= n;
    return n;
}

int ModbusDevice::memtoi(int n)
{
    int retval = 0;
    for (int i = 0; i < n; i++)
        retval = retval * 10 + rbuffer[i] - '0';
    return retval;
}