#pragma once
/*
* Encoder.h
*
* Created: 29/03/2014 1:48:30 PM
* Author: e210290
*/

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
