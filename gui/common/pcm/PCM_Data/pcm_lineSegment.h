/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

#ifndef PCM_LINESEGMENT_H
#define PCM_LINESEGMENT_H

#include "pcm_point.h"
#include "pcm_helper.h"

/*!
 * \brief The PCM_LineSegment class
 * This class is responsible to manage a line segment which consists of exactly two points.
 */
class PCM_LineSegment
{
public:
    //-----------------------------------------------------------------------------
    //! Constructor
    //-----------------------------------------------------------------------------
    PCM_LineSegment() = default;
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     firstPoint     first pcm point
    //! @param[in]     secondPoint    second pcm point
    //-----------------------------------------------------------------------------
    PCM_LineSegment(const PCM_Point &firstPoint, const PCM_Point &secondPoint);

    // removing operators
    PCM_LineSegment(const PCM_LineSegment &) = default;
    PCM_LineSegment(PCM_LineSegment &&) = default;
    PCM_LineSegment &operator=(const PCM_LineSegment &) = default;
    PCM_LineSegment &operator=(PCM_LineSegment &&) = default;

    //-----------------------------------------------------------------------------
    //! Destructor
    //-----------------------------------------------------------------------------
    virtual ~PCM_LineSegment() = default;

    //-----------------------------------------------------------------------------
    //! Retrieve the pointer to the first point of the line segment.
    //!
    //! @return                     first point of line segment
    //-----------------------------------------------------------------------------
    const PCM_Point GetFirstPoint() const;

    //-----------------------------------------------------------------------------
    //! Retrieve the pointer to the second point of the line segment.
    //!
    //! @return                     second point of line segment
    //-----------------------------------------------------------------------------
    const PCM_Point GetSecondPoint() const;

    //-----------------------------------------------------------------------------
    //! Set the pointer of the first point of the line segment.
    //!
    //! @return                     first point of line segment
    //-----------------------------------------------------------------------------
    void SetFirstPoint(const PCM_Point point);

    //-----------------------------------------------------------------------------
    //! Set the pointer of the second point of the line segment.
    //!
    //! @return                     second point of line segment
    //-----------------------------------------------------------------------------
    void SetSecondPoint(const PCM_Point point);

private:
    PCM_Point firstPoint;
    PCM_Point secondPoint;
};

#endif // PCM_LINESEGMENT_H
