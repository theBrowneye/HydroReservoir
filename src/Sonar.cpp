#include "Sonar.h"
#include "dbxMemoryMap.h"

Sonar::Sonar(byte trigger, byte pulse, int maxDst) : m_s(trigger, pulse, maxDst)
{
    wt = 25.0; // assign a default value for water temperature

    // TODO: implement span and offset in memory map and EEPROM saving
    // TODO: put cro on sonar output and see how much jitter we are really getting
    // assign default values for compensator
    span = 1;
    offset = 0;
}

void Sonar::tick()
{
    if (!taskTimer.timeOut())
        return;

    // TODO: add checking for bad water temperature    
    wt = regmap.getValueFlt(dbWatTemp);

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
    setValueflt(vs, 0);

    // reschedule timer
    taskTimer.startTimer(ReadDelay);
}