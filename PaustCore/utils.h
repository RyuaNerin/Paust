#pragma once

#include <string>
#include <cstdint>

std::string to_hex_string(const uint8_t* data, const size_t len);
std::wstring to_hex_wstring(const uint8_t* data, const size_t len);

std::wstring utf8_to_wstring(const std::string& str);
std::string wstring_to_utf8(const std::wstring& wcs);

std::wstring to_wstring(const std::string& str);
