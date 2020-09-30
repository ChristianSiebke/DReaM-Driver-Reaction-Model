#ifndef ECU_H
#define ECU_H


class Ecu
{
public:
    Ecu();

    int Perform(bool coll, bool prio1, bool prio2, bool prio3);
private:
    bool anyPrevActivity;
    int index;
};

#endif // ECU_H
