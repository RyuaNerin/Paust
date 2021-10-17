#include "debug_log.h"

#ifdef _DEBUG

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void debug_log(const wchar_t* fmt, ...)
{
    va_list	va;
    va_start(va, fmt);

    size_t len = vswprintf(nullptr, 0, fmt, va);
    std::wstring wcs;
    wcs.resize(len);

    wvsprintfW(wcs.data(), fmt, va);

    va_end(va);

    debug_log_wcs(wcs);
}

void debug_log_wcs(const std::wstring& str)
{
    std::wstring wcs = L"PaustCore: " + str + L"\n";
    OutputDebugStringW(wcs.c_str());
}

#endif
