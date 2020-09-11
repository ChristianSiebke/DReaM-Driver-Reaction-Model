#pragma once

namespace testing{

template <typename T>
inline T &DontCare()
{
    static T x{};
    return x;
}

} // namespace Testing



