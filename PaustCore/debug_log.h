#pragma once

#ifdef _DEBUG

#include <string>

void debug_log_wcs(const std::wstring& str);
void debug_log(const wchar_t* fmt, ...);

#else

#define debug_log
#define debug_log_wcs

#endif
