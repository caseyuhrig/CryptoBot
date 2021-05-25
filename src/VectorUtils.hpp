#pragma once

#include "Core.hpp"


namespace VectorUtils
{
    template <typename T>
    static bool Contains(const Vector<T>& vector, const T& value)
    {
        if (vector.empty()) return false;
        return std::any_of(vector.begin(), vector.end(), [&](const String& element) { return element == value; });
    }
}