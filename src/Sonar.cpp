#include "Sonar.h"

Sonar::Sonar(byte trigger, byte pulse, int maxDst) : m_s(trigger, pulse, maxDst)
{
    wt = 25.0; // assign a default value for water temperature

    // assign default values for compensator
    span = 1;
    offset = 0;
}

void Sonar::tick()
{
    if (!timeOut())
        return;
        
    // TODO: implement water temperature readings
    // if (!pValue2->bad)
    //     wt = pValue2->value;

    // calculate temperature compensated distance
    float vs = 331.4 + wt * 0.6;
    vs = vs * m_s.ping() / 1000 * 0.5;

    // accelerate filter convergence
    if (filter < 0.75 * vs)
        filter = vs;

    // apply exponential filter
    filter = (1 - alpha) * filter;
    filter = alpha * vs + filter;

    // compensate
    filter = filter * span + offset;

    // update database
    setValueflt(filter, 2);

    // reschedule timer
    startTimer(ReadDelay);
}