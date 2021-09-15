#include <iostream>
#include <cstdint>

#include <array>
#include <string>

#include <Windows.h>

#include <duktape.h>

#include "..\PartyFilterCore\defer.h"
#include "..\PartyFilterCore\ffxiv_data.h"
#include "..\PartyFilterCore\javascript_engine.h"

#include "duk_print_alert.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    std::wstring script_w = L"part.test";

    auto len = WideCharToMultiByte(CP_UTF8, 0, script_w.c_str(), static_cast<int>(script_w.size()), nullptr, 0, nullptr, nullptr);
    std::string script;
    script.resize(len);
    WideCharToMultiByte(CP_UTF8, 0, script_w.c_str(), static_cast<int>(script_w.size()), script.data(), static_cast<int>(len), nullptr, nullptr);

    auto js = std::make_unique<javascript_engine>();

    bool succeed;
    std::string error_message;
    auto v = js->eval(party_finder_packet_item_sample, script, &succeed, &error_message);
    
    std::wcout << std::flush;
    std::wcout << v << std::endl;
    std::wcout << "succeed :" << succeed << std::endl;
    std::cout << "error_message :" << error_message << std::endl;
    std::wcout << v << std::endl;

    std::wcin.get();
}
