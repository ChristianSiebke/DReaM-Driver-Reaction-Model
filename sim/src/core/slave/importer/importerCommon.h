#pragma once

#include "common/log.h"
#include "common/xmlParser.h"

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
