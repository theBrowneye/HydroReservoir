#include "ModbusESP8266AT.h"

ModbusDevice::ModbusDevice(HardwareSerial &s) : sd(s), wifi(s, 9600)
{
    reset();
}

// establish communications to access point
void ModbusDevice::begin()
{
    Serial.println("ModbusDevice::begin start");

    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());

    if (wifi.setOprToStationSoftAP())
    {
        Serial.print("to station + softap ok\r\n");
    }
    else
    {
        Serial.print("to station + softap err\r\n");
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
    }

    if (wifi.enableMUX())
    {
        Serial.print("multiple ok\r\n");
    }
    else
    {
        Serial.print("multiple err\r\n");
    }

    if (wifi.startTCPServer(502))
    {
        Serial.print("start tcp server ok\r\n");
    }
    else
    {
        Serial.print("start tcp server err\r\n");
    }

    if (wifi.setTCPServerTimeout(60))
    {
        Serial.print("set tcp server timout 60 seconds\r\n");
    }
    else
    {
        Serial.print("set tcp server timout err\r\n");
    }
}

void ModbusDevice::reset()
{
    len = rbufflen = sbufflen = 0;
    state = ModState::t1;
    startTimer(mbTimeOut);
    Serial.println("reset()");
    callsTotal = callsError = 0;
}

void ModbusDevice::restart()
{
    rbufflen = 0;
    state = ModState::t1;
    startTimer(mbTimeOut);
    setValueInt(++callsTotal, 0);
    // Serial.println("restart()");
}

// process modem messages
void ModbusDevice::tick()
{
    int rf, rc;

    if (timeOut()) reset();

    // prevent buffer from growing too large
    if (rbufflen >= bufferSize) reset(); 

    // read any incoming data
    bool b = false;
    while (sd.available())
    {
        char c = sd.read();
        rbuffer[rbufflen++] = c;
        b = true;
    }
    if (!b) return;

    // +IPD,id,len:data
    // look for a response header
    int l = rbufflen;
    if (state == ModState::t1 && rbufflen >= 5)
    {
        l = memstrn("+IPD,");
        if (l > 0)
        {
            memrem(l+5); // strip leading portion
            state = ModState::t2;
        }
    } 

    if (state == ModState::t2 && rbufflen >= 2)
    {
        l = memstrn(",");
        if (l > 0)
        {
            mux = memtoi(l);
            memrem(l+1);
            state = ModState::t3;
        }
    }

    if (state == ModState::t3 && rbufflen > 2)
    {
        l = memstrn(":");
        if (l > 0)
        {
            len = memtoi(l);
            memrem(l+1);
            state = ModState::t4;
        }
    }

    if (state == ModState::t4 && rbufflen >= len)
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
                rc = mb_cnt;
                rf = mb_ref;
                for (int i = 0; i < 12; i++)
                    sbuffer[i] = rbuffer[i];
                sbuffer[5] = 3 + mb_cnt * 2; // length
                sbuffer[8] = mb_cnt * 2;     // number of value bytes
                sbufflen = 9 + mb_cnt * 2;       // length of out buffer

                union {
                    uint16_t i;
                    uint8_t  b[2];
                } uu;
                for (int i = 0; i < mb_cnt; i++)
                {
                    uu.i = bPtr[i + mb_ref];
                    sbuffer[9 + i * 2] = uu.b[0];
                    sbuffer[10 + i* 2] = uu.b[1];
                }

                rsend = true;

                Serial.print("MODBUS:"); Serial.print(mb_fc); Serial.print(",R:"); Serial.print(rf); Serial.print(",C:"); Serial.println(rc);
                break;

            // FC16 - write multiple registers
            // len-in = 12 + num * 2
            // len-out = 12
            // map ! 00 ! 01 ! 02 ! 03 ! 04 ! 05 ! 06 ! 07 ! 08 ! 09 ! 10 ! 11 ! 12 ! 13 ! 14 ! 15
            // in  ! id ! id ! 00 ! 00 ! 00 ! len! un ! 16 ! R-H! R-L! C-H! C-L! VAL! VAL! ...
            // out ! id ! id ! 00 ! 00 ! 00 ! len! un ! 16 ! R-H! R-L! C-H! C-L!
            case 16:
                // mb_cnt = static_cast<uint8_t>(buffer[10]) * 256 + static_cast<uint8_t>(buffer[11]);
                // buffer[5] = 6; // length
                // while (mb_cnt-- > 0)
                // {
                // }
                break;

            // FC6 - write single registers
            // len-in = 12
            // len-out = 12
            // map ! 00 ! 01 ! 02 ! 03 ! 04 ! 05 ! 06 ! 07 ! 08 ! 09 ! 10 ! 11 ! 12 ! 13 ! 14 ! 15
            // in  ! id ! id ! 00 ! 00 ! 00 ! len! un !  6 ! R-H! R-L! VAL! VAL!
            // out ! id ! id ! 00 ! 00 ! 00 ! len! un !  6 ! R-H! R-L! VAL! VAL!
            case 6:
                // mb_cnt = 1;
                // buffer[5] = 6;
                // hreg[mb_ref] = buffer[10] * 256 + buffer[11];
                break;
            }
            // for (uint32_t i = 0; i < len; i++)
            // {
            //     Serial.print(buffer[i], HEX);
            //     Serial.print(",");
            // }
            // Serial.println("done");

            // send response
            // bool ESP8266::send(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
        }
        if (rsend)  {
            state = ModState::s1;
        } else {
            reset();
        }
    }
    
    if (state == ModState::s1)
    {
        sd.print("AT+CIPSEND=");
        sd.print(mux);
        sd.print(",");
        sd.println(sbufflen);
        rbufflen = 0;
        state = ModState::s2;
    }

    if (state == ModState::s2 && rbufflen > 22)
    {
        l = memstrn(">");
        if (l > 0)
        {
            sd.write(sbuffer, sbufflen);
            restart();
        }
    }
}

int ModbusDevice::memstrn(char *needle)
{
	uint8_t *p;
	uint16_t needlesize = strlen(needle);

	for (p = rbuffer; p <= (rbuffer-needlesize+rbufflen); p++)
	{
		if (memcmp(p, needle, needlesize) == 0)
			return p - rbuffer; /* found */
	}
	return -1;
}

int ModbusDevice::memrem(int n)
{
    if (n <= 0) return 0;
    n = min(rbufflen, n);

    for (int i = 0; i < rbufflen - n; i++)
        rbuffer[i] = rbuffer[i+n];
    rbufflen -= n;
    return n;
}

int ModbusDevice::memtoi(int n)
{
    int retval = 0;
    for (int i = 0; i < n; i++) 
        retval = retval * 10 + rbuffer[i]-'0';
    return retval;
}