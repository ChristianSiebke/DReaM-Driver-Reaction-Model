/*******************************************************************************
* Copyright (c) 2019 in-tech GmbH
*
* This program and the accompanying materials are made
* available under the terms of the Eclipse Public License 2.0
* which is available at https://www.eclipse.org/legal/epl-2.0/
*
* SPDX-License-Identifier: EPL-2.0
*******************************************************************************/

#pragma once

#include <string>
#include <sstream>

namespace openpass::common {

struct Version
{
    unsigned int major;
    unsigned int minor;
    unsigned int patch;

    constexpr explicit Version (unsigned int major, unsigned int minor, unsigned int patch) noexcept : 
    major{major}, minor{minor}, patch{patch}
    {}
    
    std::string str() const noexcept
    {
        std::ostringstream oss;
        oss << major << '.' << minor << '.' << patch;
        return oss.str();
    }
};

//std::ostream& operator<<(std::ostream& os, const Version& version)
//{
//    return (os << version.major << '.' << version.minor << '.' << version.patch);
//}


//bool operator==(const Version &lhs, const Version &rhs) noexcept
//{
//    return rhs.patch == lhs.patch &&
//           rhs.minor == lhs.minor &&
//           rhs.major == lhs.major;
//}

//bool operator!=(const Version &lhs, const Version &rhs) noexcept
//{
//    return !(rhs == lhs);
//}
    
//bool operator==(const Version &lhs, const std::string &rhs) noexcept
//{
//    std::stringstream ss;
//    ss << lhs.major << '.' << lhs.minor << '.' << lhs.patch;
//    return ss.str() == rhs;
//}

//bool operator!=(const Version &lhs, const std::string &rhs) noexcept
//{
//    return !(lhs == rhs);
//}

//bool operator==(const std::string &lhs, const Version &rhs) noexcept
//{
//    return rhs == lhs;
//}

//bool operator!=(const std::string &lhs, const Version &rhs) noexcept
//{
//    return !(rhs == lhs);
//}

//bool operator<(const Version& lhs, const Version& rhs) noexcept
//{
//    return lhs.major < rhs.major &&
//           lhs.minor < rhs.minor &&
//           lhs.patch < rhs.patch;
//}

//bool operator>(const Version& lhs, const Version& rhs) noexcept
//{
//    return !(lhs < rhs);
//}

//bool operator<=(const Version& lhs, const Version& rhs) noexcept
//{
//    return (lhs == rhs) || (lhs < rhs);
//}

//bool operator>=(const Version& lhs, const Version& rhs) noexcept
//{
//    return !(lhs <= rhs);
//}

} // namespace openpass::common
