/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#include "pcm_lineContainer.h"

PCM_LineContainer::~PCM_LineContainer()
{
    for (std::pair<int, PCM_Line *> pcmLinePair : lineMap)
    {
        delete pcmLinePair.second;
    }
    lineMap.clear();
}

bool PCM_LineContainer::AddPCM_Line(PCM_Line *line)
{
    bool success = lineMap.emplace(std::make_pair(line->GetId(), line)).second;
    return success;
}

const std::map<int, PCM_Line *> *PCM_LineContainer::GetLineMap() const
{
    return &lineMap;
}

bool PCM_LineContainer::IsLinePresent(int id) const
{
    return lineMap.find(id) != lineMap.end();
}

PCM_Line *PCM_LineContainer::GetLineById(int id) const
{
    std::map<int, PCM_Line *>::const_iterator lineIterator = lineMap.find(id);

    if (lineIterator != lineMap.end())
    {
        return lineIterator->second;
    }
    else
    {
        return nullptr;
    }
}
