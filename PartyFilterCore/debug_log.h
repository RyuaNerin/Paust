#pragma once

#ifdef _DEBUG
void debug_log(const wchar_t* fmt, ...);
#else
#define debug_log
#endif
