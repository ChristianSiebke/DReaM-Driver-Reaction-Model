/********************************************************************************
 * Copyright (c) 2019-2020 in-tech GmbH
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 ********************************************************************************/

#pragma once

#include <sstream>
#include <string>

namespace openpass::common {

//! Struct for defining the version of the simulator build
//! The version can either be given by using semantic versioning with three integers
//! or as some generic string
struct Version
{
    std::string tag;
    unsigned int major{0};
    unsigned int minor{0};
    unsigned int patch{0};

    //! Constructor using generic string
    explicit Version(const std::string& tag) :
        tag{tag}
    {
    }

    //! Constructor using semantic versioning
    explicit Version(unsigned int major, unsigned int minor, unsigned int patch) noexcept :
        major{major}, minor{minor}, patch{patch}
    {
    }

    //! Return the version as string
    std::string str() const noexcept
    {
        if (tag.empty())
        {
            std::ostringstream oss;
            oss << major << '.' << minor << '.' << patch;
            return oss.str();
        }
        else
        {
            return tag;
        }
    }
};

} // namespace openpass::common
