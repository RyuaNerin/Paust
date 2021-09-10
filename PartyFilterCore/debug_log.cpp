#include "debug_log.h"

#include <Windows.h>

#include <string>
#include <memory>

void debug_log(const wchar_t* fmt, ...)
{
    va_list	va;
    va_start(va, fmt);
    size_t len = std::wcslen(L"PartyFilterCore: ") + vswprintf(nullptr, 0, fmt, va) + 2;
    auto str = std::make_unique<wchar_t[]>(len);

    if (str == nullptr)
    {
        va_end(va);
        return;
    }

    len = wsprintfW(str.get(), L"PartyFilterCore: ");
    wvsprintfW(str.get() + len, fmt, va);

    va_end(va);

    OutputDebugStringW(str.get());
}