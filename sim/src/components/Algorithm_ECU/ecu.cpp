#include "ecu.h"

Ecu::Ecu()
{
    anyPrevActivity = false;
    index = -1;
}

int Ecu::Perform(bool coll, bool prio1, bool prio2, bool prio3) {

    if (coll)
    {
        index = 0;
    }
    else if (prio1)
    {
        index = 1;
        anyPrevActivity = true;
    }
    else if (prio2)
    {
        index = 2;
        anyPrevActivity = true;
    }
    else if (prio3)
    {
        index = 3;
        anyPrevActivity = true;
    }
    else if (anyPrevActivity)
    {
        index = 100;
    }

    return index;
}
