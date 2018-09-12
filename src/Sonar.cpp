#include "Sonar.h"
#include "dbxMemoryMap.h"

Sonar::Sonar(byte trigger, byte pulse, int maxDst) : m_s(trigger, pulse, maxDst)
{
    wt = 25.0; // assign a default value for water temperature
}

void Sonar::tick()
{
    if (!taskTimer.timeOut())
        return;

    // get water temperature if available
    if (!isBadValue(dbWatTemp))
        wt = regmap.getValueFlt(dbWatTemp);

    // calculate temperature compensated distance
    medianFilter.addValue(m_s.ping());
    float vs = 331.4 + wt * 0.6;
    vs = vs * medianFilter.getMedian() / 1000 * 0.5;

    // accelerate filter convergence
    if (filter < 0.75 * vs)
        filter = vs;

    // apply exponential filter
    // filter = (1 - alpha) * filter;
    // filter = alpha * vs + filter;

    // compensate
    span = getValueFlt(4);
    offset = getValueFlt(6);
    filter = filter * span + offset;

    // update database
    setValueflt(filter, 2);
    setValueflt(vs, 0);

    // reschedule timer
    taskTimer.startTimer(ReadDelay);
}