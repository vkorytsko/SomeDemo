#pragma once

#include <string>
#include <stdlib.h>


namespace SD {

inline std::wstring AToWstring(const std::string& str)
{
    WCHAR buffer[1024];
    mbstowcs_s(nullptr, buffer, str.c_str(), _TRUNCATE);
    return buffer;
}

inline std::string WToAstring(const std::wstring& str)
{
    CHAR buffer[1024];
    wcstombs_s(nullptr, buffer, str.c_str(), _TRUNCATE);
    return buffer;
}

}  // end namespace SD
