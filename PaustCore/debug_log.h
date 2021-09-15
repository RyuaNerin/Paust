#pragma once

#ifdef _DEBUG

#include <string>

void debug_log(const std::wstring& str);
void debug_log(const wchar_t* fmt, ...);

#else

#define debug_log

#endif
