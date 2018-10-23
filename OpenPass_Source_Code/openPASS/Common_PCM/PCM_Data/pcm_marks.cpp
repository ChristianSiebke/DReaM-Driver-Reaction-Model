/******************************************************************************
* Copyright (c) 2017 ITK Engineering GmbH.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
******************************************************************************/

#include "pcm_marks.h"

PCM_Marks::PCM_Marks()
{
    markType = MarkType::NONE;
}

PCM_Marks::PCM_Marks(MarkType markType):
    markType(markType)
{}

bool PCM_Marks::SetMarkType(MarkType markType)
{
    this->markType = markType;
    return true;
}

MarkType PCM_Marks::GetMarkType() const
{
    return markType;
}
