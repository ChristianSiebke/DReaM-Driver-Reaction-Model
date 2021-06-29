/*********************************************************************
* Copyright (c) 2020, 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

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
