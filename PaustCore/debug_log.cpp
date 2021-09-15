#include "debug_log.h"

#ifdef _DEBUG

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void debug_log(const wchar_t* fmt, ...)
{
    va_list	va;
    va_start(va, fmt);

    size_t len = std::wcslen(L"PaustCore: ") + vswprintf(nullptr, 0, fmt, va) + 2;
    std::wstring wcs;
    wcs.resize(len);

    len = wsprintfW(wcs.data(), L"PaustCore: ");
    wvsprintfW(wcs.data() + len, fmt, va);

    va_end(va);

    OutputDebugStringW(wcs.c_str());
}

void debug_log(const std::wstring& str)
{
    std::wstring wcs = L"PaustCore: " + str;
    OutputDebugStringW(wcs.c_str());
}

#endif
