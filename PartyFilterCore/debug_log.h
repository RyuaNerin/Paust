#pragma once

#define USE_DEBUG_LOG

#ifdef USE_DEBUG_LOG

#include <string>

void debug_log(const std::wstring& str);
void debug_log(const wchar_t* fmt, ...);

#else

#define debug_log

#endif
