#pragma once

#include "Core.hpp"

namespace StringUtils
{
    static String ReplaceAll(const String& value, const String& from, const String& to)
    {
        String str = value;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }
}