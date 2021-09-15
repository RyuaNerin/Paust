#include "utils.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <sstream>
#include <iomanip>
#include <exception>

#include "debug_log.h"

std::string to_hex_string(const uint8_t* data, const size_t len)
{
    std::stringstream ss;
    for (size_t i = 0; i < len; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i] << " ";

    return ss.str();
}

std::wstring to_hex_wstring(const uint8_t* data, const size_t len)
{
    std::wstringstream ss;
    for (size_t i = 0; i < len; ++i)
        ss << std::hex << std::setw(2) << std::setfill(L'0') << (int)data[i] << L" ";

    return ss.str();
}

std::wstring utf8_to_wstring(const std::string& str)
{
    std::wstring wcs;

    // MB_ERR_INVALID_CHARS
    const auto len = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), nullptr, 0);
    if (len == 0)
    {
        debug_log(L"utf8_to_wstring error : %ws %d", to_wstring(str).c_str(), GetLastError());

        wcs.resize(str.size());
        wcs.assign(str.begin(), str.end());
        return wcs;
    }

    wcs.resize(len);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), wcs.data(), len);

    return wcs;
}
std::string wstring_to_utf8(const std::wstring& wcs)
{
    std::string str;

    const auto len = WideCharToMultiByte(CP_UTF8, 0, wcs.data(), static_cast<int>(wcs.size()), nullptr, 0, NULL, NULL);
    if (len == 0)
    {
        debug_log(L"wstring_to_utf8 error : %ws %d", wcs.c_str(), GetLastError());

        str.resize(wcs.size());
        str.assign(wcs.begin(), wcs.end());
        return str;
    }

    str.resize(len);
    WideCharToMultiByte(CP_UTF8, 0, wcs.data(), static_cast<int>(wcs.size()), str.data(), len, NULL, NULL);

    return str;
}

std::wstring to_wstring(const std::string& str)
{
    std::wstring wstr;
    wstr.resize(str.size());
    wstr.assign(str.begin(), str.end());

    return wstr;
}
