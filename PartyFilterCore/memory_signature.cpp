#include "memory_signature.h"

#include <vector>
#include <cstdint>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include "debug_log.h"

inline uint8_t hex2dec(char h0, char h1)
{
    uint8_t val = 0;

    if      (h0 >= '0' && h0 <= '9') val = (h0 - '0'     ) << 4;
    else if (h0 >= 'a' && h0 <= 'f') val = (h0 - 'a' + 10) << 4;
    else if (h0 >= 'A' && h0 <= 'F') val = (h0 - 'A' + 10) << 4;

    if      (h1 >= '0' && h1 <= '9') val |= (h1 - '0'     );
    else if (h1 >= 'a' && h1 <= 'f') val |= (h1 - 'a' + 10);
    else if (h1 >= 'A' && h1 <= 'F') val |= (h1 - 'A' + 10);

    return val;
}

void memory_signature::gen_bch()
{
    size_t last = this->mask.size() - 1;
    size_t idx;
    for (idx = last; idx > 0 && !this->mask[idx]; --idx)
    {
    }

    debug_log(L"this->mask size : %d", this->mask.size());

    auto diff = last - idx;
    if (diff == 0) diff = 1;

    for (idx = 0; idx <= 255; ++idx)
        this->bch[idx] = diff;
    for (idx = last - diff; idx < last; ++idx)
        this->bch[this->value[idx]] = last - idx;

    debug_log(L"gen_bch : ok");
}

const uint8_t* memory_signature::find_signature(const uint8_t* baseAddr, const size_t baseSize) const
{
    if (this->size > baseSize)
    {
        return nullptr;
    }

    const size_t sigLast = this->size - 1;

    size_t offset = 0;
    const size_t offsetMax = baseSize - this->size;
    while (offset <= offsetMax)
    {
        size_t position;
        for (position = sigLast; this->mask[position] || this->value[position] == baseAddr[offset + position]; position--)
        {
            if (position == 0)
                return baseAddr + offset;
        }
        offset += this->bch[baseAddr[offset + sigLast]];
    }

    return nullptr;
}

bool get_ffxiv_module(DWORD pid, uint8_t** modBaseAddr, size_t* modBaseSize)
{
    bool res = false;

    MODULEENTRY32W snapEntry = { 0, };
    snapEntry.dwSize = sizeof(MODULEENTRY32W);

    const auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hSnapshot)
    {
        if (Module32FirstW(hSnapshot, &snapEntry))
        {
            do
            {
                if (std::wcscmp(snapEntry.szModule, L"ffxiv_dx11.exe") == 0)
                {
                    *modBaseAddr = snapEntry.modBaseAddr;
                    *modBaseSize = static_cast<size_t>(snapEntry.modBaseSize);
                    res = true;
                    break;
                }
            } while (Module32Next(hSnapshot, &snapEntry));
        }
        CloseHandle(hSnapshot);
    }
    return res;
}

memory_signature::memory_signature(const std::string& str)
{
    size_t index = 0;
    while (index < str.size())
    {
        if (str[index] == ' ')
        {
            index++;
            continue;
        }

        if (str[index] == '?')
        {
            this->value.push_back(0);
            this->mask.push_back(true);
        }
        else
        {
            this->value.push_back(hex2dec(str[index], str[index + 1]));
            this->mask.push_back(false);
        }

        index += 2;
    }


    debug_log(L"size");
    this->size = this->value.size();

    debug_log(L"gen_bch");
    this->gen_bch();
}

void* memory_signature::scan() const
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // 파이널 판타지 메인 모듈 주소 얻어오기
    auto pid = GetCurrentProcessId();
    debug_log(L"pid %d", pid);

    uint8_t* baseAddr = nullptr;
    size_t baseSize = 0;

    if (!get_ffxiv_module(pid, &baseAddr, &baseSize))
    {
        debug_log(L"Module not found");
        return nullptr;
    }
    debug_log(L"Module found : %p / %p", baseAddr, baseSize);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // 위치 계산
    auto ptr = (LPVOID)this->find_signature(baseAddr, baseSize);
    if (ptr == nullptr)
    {
        debug_log(L"SigChat not found");
        return nullptr;
    }

    debug_log(L"OnMessageAddr : %p", ptr);

    return ptr;
}
