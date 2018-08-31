/*
* Encoder.cpp
*
* Created: 29/03/2014 1:48:30 PM
* Author: e210290
*/

#include "Arduino.h"
#include "Encoder.h"

int menuRange = 0;
int menuValue = 0;
volatile bool menuButton = false;
volatile int rotaryValue = 0;
bool rotaryMode = false;

// default constructor
Encoder::Encoder(int pinA, int pinB)
{
    a = pinA;
    b = pinB;
}

void Encoder::begin()
{
    pinMode(a, INPUT);
    pinMode(b, INPUT);

    // enable pin change interrupts
    PCMSK2 |= 0x07;                          // enable pin mask
    PCICR |= (1 << digitalPinToPCICRbit(a)); // enable pin change interrupts
}

void Encoder::setMenuRange(int r)
{
    menuRange = r;
}

void Encoder::setRotaryMode(bool m)
{
    rotaryMode = m;
    if (m)
        rotaryValue = 0;
}

bool Encoder::inRotaryMode()
{
    return rotaryMode;
}

int Encoder::getRotary()
{
    int i = rotaryValue;
    rotaryValue = 0;
    return i;
}

int Encoder::getMenu()
{
    return menuValue;
}

ISR(PCINT2_vect)
{
    uint8_t encport; //encoder port copy
    uint8_t btnport; // port copy
    uint8_t direction;
    static uint8_t old_AB = 3;                                                                       //lookup table index and initial state
    static const int8_t enc_states[] PROGMEM = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; //encoder lookup table

    old_AB <<= 2;          //remember previous state
    encport = PINK & 0x03; //read encoder bottom 3 bits
    btnport = PINK & 0x04; // read button

    if (!btnport)
    {
        menuButton = true;
    }
    else
    {
        old_AB |= encport;                                         //create index
        direction = pgm_read_byte(&(enc_states[(old_AB & 0x0f)])); //get direction

        if (direction && (encport == 3))
        { //check if at detent and transition is valid
            /* post "Navigation forward/reverse" event */
            if (direction == 1)
            { // forward
                if (!rotaryMode)
                    menuValue++;
                rotaryValue++;
            }
            else
            { // reverse
                if (!rotaryMode)
                    menuValue--;
                rotaryValue--;
            }
            menuValue = min(menuValue, menuRange);
            menuValue = max(menuValue, 0);
        }
    }
}