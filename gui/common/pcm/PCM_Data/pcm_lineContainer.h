/*********************************************************************
* Copyright (c) 2021 ITK Engineering GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
**********************************************************************/

//-----------------------------------------------------------------------------
//! @file  pcm_lineContainer.h
//! @brief class to store a line which consists of pcm points.
//!
//! This class is responsible to manage data of pcm lines.
//-----------------------------------------------------------------------------

#ifndef PCM_LINECONTAINER_H
#define PCM_LINECONTAINER_H

#include <QString>
#include "pcm_line.h"

/*!
 * \brief The PCM_LineContainer class
 * This class is responsible to manage data of a pcm lines.
 */
class PCM_LineContainer
{
public:
    //-----------------------------------------------------------------------------
    //! Default Constructor
    //-----------------------------------------------------------------------------
    PCM_LineContainer() = default;

    // removing operators
    PCM_LineContainer(const PCM_LineContainer &) = default;
    PCM_LineContainer(PCM_LineContainer &&) = default;
    PCM_LineContainer &operator=(const PCM_LineContainer &) = delete;
    PCM_LineContainer &operator=(PCM_LineContainer &&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor
    //-----------------------------------------------------------------------------
    ~PCM_LineContainer();

    //-----------------------------------------------------------------------------
    //! Function to add a line.
    //!
    //! @param[in]     line     PCM_Line to add
    //! @return                 true on success
    //-----------------------------------------------------------------------------
    bool AddPCM_Line(PCM_Line *line);

    //-----------------------------------------------------------------------------
    //! Get the vector of lines.
    //!
    //! @return                 vector of points
    //-----------------------------------------------------------------------------
    const std::map<int, PCM_Line *> *GetLineMap() const;

    //-----------------------------------------------------------------------------
    //! Check if a line with this id is already present.
    //!
    //! @param[in]     id       id of line
    //! @return                 true if line already exists,
    //!                         false else
    //-----------------------------------------------------------------------------
    bool IsLinePresent(int id) const;

    //-----------------------------------------------------------------------------
    //! Get a line by the id
    //!
    //! @param[in]     id       id of line
    //! @return                 pointer the the PCM_Line with this id,
    //!                         nullptr if line does not exist
    //-----------------------------------------------------------------------------
    PCM_Line *GetLineById(int id) const;

private:
    std::map<int, PCM_Line *> lineMap;    //!< map of lines
};

#endif // PCM_LINECONTAINER_H
