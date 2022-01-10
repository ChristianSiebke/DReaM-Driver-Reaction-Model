/********************************************************************************
 * Copyright (c) 2020-2021 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

/*!
 * 
 * \file  writeInterface.h
 * \brief This file contains the interface of the observation modules to
 *        interact with the framework
 */

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "common/logEntryBase.h"
#include "include/dataBufferInterface.h"

/**
 * @brief Interface which has to be provided by observation modules
 */
class PublisherInterface
{
public:
    PublisherInterface(DataBufferWriteInterface *const dataBuffer) :
        dataBuffer(dataBuffer)
    {
    }

    PublisherInterface(const PublisherInterface &) = delete;
    PublisherInterface &operator=(const PublisherInterface &) = delete;

    PublisherInterface(PublisherInterface &&) = default;
    PublisherInterface &operator=(PublisherInterface &&) = default;

    virtual ~PublisherInterface() = default;

    /*!
     * \brief Writes information into a data buffer backend
     *
     * \param key[in]     Unique topic identification
     * \param value[in]   Value to be written
     */
    virtual void Publish(const openpass::databuffer::Key &key, const openpass::databuffer::Value &value)
    {
    }

    /*!
     * \brief Writes acyclic information into a data buffer backend
     *
     * \param value[in]   The acyclic event
     */
    virtual void Publish(const openpass::databuffer::Key &key, const openpass::publisher::LogEntryBase &event)
    {
    }

    /*!
     * \brief Writes acyclic information into a data buffer backend
     *
     * \param value[in]   The acyclic event
     */
    virtual void Publish(const openpass::databuffer::Key &key, const openpass::databuffer::ComponentEvent &event)
    {
    }

protected:
    DataBufferWriteInterface *const dataBuffer; //!< References the dataBuffer backend
};
