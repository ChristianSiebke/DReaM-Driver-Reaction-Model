/******************************************************************************
* Copyright (c) 2017 ITK Engineering GmbH.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
******************************************************************************/

//-----------------------------------------------------------------------------
//! @file  pcm_object.h
//! @brief class to store a agent of pcm.
//!
//! This class is responsible to manage data of pcm objects which consists of
//! pcm lines.
//-----------------------------------------------------------------------------

#ifndef PCM_AGENT_H
#define PCM_AGENT_H

#include "pcm_lineContainer.h"

/*!
 * \brief The PCM_Agent class
 * This class is responsible to manage data of pcm agents which consists of
 * pcm lines.
 */
class PCM_Agent : public PCM_LineContainer
{
public:
    //-----------------------------------------------------------------------------
    //! Default Constructor
    //-----------------------------------------------------------------------------
    PCM_Agent() = default;
    //-----------------------------------------------------------------------------
    //! Constructor
    //!
    //! @param[in]     objectType     type of object
    //-----------------------------------------------------------------------------
    PCM_Agent(int id);

    // removing operators
    PCM_Agent(const PCM_Agent &) = default;
    PCM_Agent(PCM_Agent &&) = default;
    PCM_Agent &operator=(const PCM_Agent &) = delete;
    PCM_Agent &operator=(PCM_Agent &&) = delete;

    //-----------------------------------------------------------------------------
    //! Destructor
    //-----------------------------------------------------------------------------
    virtual ~PCM_Agent() = default;

    //-----------------------------------------------------------------------------
    //! Get the id.
    //!
    //! @return                 id
    //-----------------------------------------------------------------------------
    int GetId() const;

private:
    int id;
};

#endif // PCM_AGENT_H
