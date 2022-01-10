/********************************************************************************
 * Copyright (c) 2017-2021 ITK Engineering GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

//-----------------------------------------------------------------------------
//! @file  pcm_lineSegment.h
//! @brief class to store a line segment which consists of two points
//!
//! This class is responsible to manage a pcm lineSegment.
//-----------------------------------------------------------------------------

#include "pcm_lineSegment.h"

#include "common/commonHelper.h"
#include "common/vector2d.h"

PCM_LineSegment::PCM_LineSegment(const PCM_Point &firstPoint, const PCM_Point &secondPoint) :
    firstPoint(firstPoint),
    secondPoint(secondPoint)
{
}

const PCM_Point PCM_LineSegment::GetFirstPoint() const
{
    return firstPoint;
}

const PCM_Point PCM_LineSegment::GetSecondPoint() const
{
    return secondPoint;
}

void PCM_LineSegment::SetFirstPoint(const PCM_Point point)
{
    firstPoint = point;
}

void PCM_LineSegment::SetSecondPoint(const PCM_Point point)
{
    secondPoint = point;
}

