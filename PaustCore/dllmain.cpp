#include <minhook.h>

#include "memory_signature.h"
#include "debug_log.h"
#include "ffxiv_data.h"
#include "filter_core.h"
#include "utils.h"

static std::unique_ptr<filter_core> core_instance;

/*
ffxiv_dx11.exe+E8100 - 40 53                 - push rbx
ffxiv_dx11.exe+E8102 - 41 57                 - push r15
ffxiv_dx11.exe+E8104 - 48 83 EC 28           - sub rsp,28 { 40 }
ffxiv_dx11.exe+E8108 - 48 8B D9              - mov rbx,rcx
ffxiv_dx11.exe+E810B - 4C 8B FA              - mov r15,rdx
ffxiv_dx11.exe+E810E - 48 8B 0D 431ECA01     - mov rcx,[ffxiv_dx11.exe+1D89F58] { (1CC62864260) }
ffxiv_dx11.exe+E8115 - E8 7692FAFF           - call ffxiv_dx11.exe+91390
ffxiv_dx11.exe+E811A - 41 81 7F 10 00000080  - cmp [r15+10],80000000 { -2147483648 }
ffxiv_dx11.exe+E8122 - 75 0F                 - jne ffxiv_dx11.exe+E8133
ffxiv_dx11.exe+E8124 - C6 83 95030000 00     - mov byte ptr [rbx+00000395],00 { 0 }
ffxiv_dx11.exe+E812B - 48 83 C4 28           - add rsp,28 { 40 }
ffxiv_dx11.exe+E812F - 41 5F                 - pop r15
ffxiv_dx11.exe+E8131 - 5B                    - pop rbx
ffxiv_dx11.exe+E8132 - C3                    - ret
ffxiv_dx11.exe+E8133 - 48 8B 4B 08           - mov rcx,[rbx+08]
*/
typedef void           (*party_list_delegate)(const void* manager, const void* data);
const memory_signature   party_list_sig ("4053 4157 4883EC?? 488BD9 4C8BFA");
void*                    party_list_ptr = nullptr;
party_list_delegate      party_list_original;

void party_list_new(const void* manager, const void* data)
{
    core_instance->modify_packet((party_finder_packet*)data);

    party_list_original(manager, data);
}

bool g_hooked = false;
__declspec(dllexport) BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    OutputDebugStringW(L"ATTACHED");

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        debug_log(L"DllMain : DLL_PROCESS_ATTACH /// hModule: %x / lpReserved: %x", hModule, lpReserved);

        party_list_ptr = party_list_sig.scan();
        debug_log(L"party_list_ptr : %x", party_list_ptr);
        if (party_list_ptr == nullptr)
        {
            debug_log_wcs(L"scan failed");
            return FALSE;
        }

        core_instance = std::make_unique<filter_core>();

        {
            auto mh = MH_Initialize();
            debug_log(L"MH_Initialize : %d", mh);
            if (mh == MH_OK)
            {
                g_hooked = true;

                MH_CreateHook(party_list_ptr, &party_list_new, (LPVOID*)&party_list_original);

                MH_EnableHook(party_list_ptr);
            }
        }

        debug_log_wcs(L"attached");
        break;

    case DLL_PROCESS_DETACH:
        debug_log(L"DllMain : DLL_PROCESS_DETACH /// hModule: %x / lpReserved: %x", hModule, lpReserved);

        if (g_hooked)
        {
            MH_DisableHook(party_list_ptr);


            MH_RemoveHook(party_list_ptr);

            MH_Uninitialize();
        }

        core_instance.reset();

        debug_log_wcs(L"detached");
        break;
        
    case DLL_THREAD_ATTACH:
        //debug_log(L"DllMain : DLL_THREAD_ATTACH /// hModule: %x / lpReserved: %x", hModule, lpReserved);
        break;

    case DLL_THREAD_DETACH:
        //debug_log(L"DllMain : DLL_THREAD_DETACH /// hModule: %x / lpReserved: %x", hModule, lpReserved);
        break;

    default:
        debug_log(L"DllMain : %d /// hModule: %x / lpReserved: %x", ul_reason_for_call, hModule, lpReserved);
        break;
    }

    return TRUE;
}
