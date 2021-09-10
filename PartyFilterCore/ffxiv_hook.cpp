#include "ffxiv_hook.h"

#include <Windows.h>
#include <memory>
#include <sstream>
#include <iomanip>

#include "party_packet.h"
#include "debug_log.h"

receive_listing_delegate receive_listing_original;

void write_to_hex(const uint8_t* data, const size_t len)
{
    std::wstringstream ss;
    for (size_t i = 0; i < len; ++i)
        ss << std::hex << std::setw(2) << std::setfill(L'0') << (int)data[i] << L" ";

    debug_log(ss.str().c_str());
}

void receive_listing_new(const void* manager, const void* data)
{
    constexpr size_t string_buff_length = 512;


    auto packet = (party_finder_packet*)((char*)data + 0x10);

    auto memory = std::make_unique<uint8_t[]>(sizeof(party_finder_packet));

    auto string_buff = std::make_unique<wchar_t[]>(string_buff_length);

    for (int i = 0; i < 4; i++)
    {
        auto listing = &packet->listing[i];

        auto isNotNull = false;
        for (int k = 0; k < 8 && !isNotNull; k++)
        {
            isNotNull = listing->Slots[k] != 0;
        }

        if (!isNotNull)
        {
            continue;
        }

        memset(string_buff.get(), 0, string_buff_length);

        auto desc_utf8 = (const char*)listing->Description;
        auto desc_len = std::strlen((const char*)listing->Description);

        auto cch = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, desc_utf8, desc_len, nullptr, 0);
        ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, desc_utf8, desc_len, string_buff.get(), cch);

        std::wstring desc_wstring(string_buff.get());

        debug_log(L"--------------------------------------------------");
        debug_log(L"Description");
        debug_log(desc_wstring.c_str());

        if (desc_wstring.find(L"2»ó") != std::wstring::npos)
        {
            memset(listing, 0, sizeof(party_finder_packet_listing));
        }
    }

    receive_listing_original(manager, data);
}