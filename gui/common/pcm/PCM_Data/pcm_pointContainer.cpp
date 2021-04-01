/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "pcm_pointContainer.h"

PCM_PointContainer::~PCM_PointContainer()
{
    for (std::pair<int, const PCM_Point *> pcmPointPair : pointMap)
    {
        delete pcmPointPair.second;
    }

    pointMap.clear();
}

bool PCM_PointContainer::AddPCM_Point(const PCM_Point *point)
{
    bool success = pointMap.emplace(std::make_pair(point->GetId(), point)).second;
    return success;
}

const std::map<int, const PCM_Point *> *PCM_PointContainer::GetPointMap() const
{
    return &pointMap;
}

bool PCM_PointContainer::IsPointPresent(int id) const
{
    return pointMap.find(id) != pointMap.end();
}

const PCM_Point *PCM_PointContainer::GetPointById(int id) const
{
    std::map<int, const PCM_Point *>::const_iterator pointIterator = pointMap.find(id);

    if (pointIterator != pointMap.end())
    {
        return pointIterator->second;
    }
    else
    {
        return nullptr;
    }
}
