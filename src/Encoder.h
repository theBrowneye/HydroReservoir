#pragma once
/*
* Encoder.h
*
* Created: 29/03/2014 1:48:30 PM
* Author: e210290
*/

// TODO: find a better encoder class that handles the button press

class Encoder
{
  public:
    Encoder(int pinA, int pinB);
    void begin();
    void setMenuRange(int r);
    int getMenu();
    int getRotary();
    void setRotaryMode(bool m);
    bool inRotaryMode();

  private:
    int a, b, c;
};
