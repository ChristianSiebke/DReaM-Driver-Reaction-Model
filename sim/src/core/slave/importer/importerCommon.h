/*******************************************************************************
* Copyright (c) 2021 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include "common/log.h"
#include "common/xmlParser.h"

//! If the first parameter is false writes a message into the log including the line and column number of the erronous xml element
//!
//! \param success      writes message if success is false
//! \param element      erronous xml element
//! \param message      message describing error
//! \param logFunction  function to use for logging
[[maybe_unused]] static void ThrowIfFalse(bool success, const QDomElement element, const std::string &message)
{
    if (!success)
    {
        LogErrorAndThrow("Could not import element " + element.tagName().toStdString() +
                         " (line " + std::to_string(element.lineNumber()) +
                         ", column " + std::to_string(element.columnNumber()) + "): " +
                         message);
    }
}
