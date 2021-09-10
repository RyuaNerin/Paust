#include "minhook-1.3.3/include/MinHook.h"

#include "memory_signature.h"
#include "debug_log.h"
#include "ffxiv_hook.h"

static const memory_signature receive_listing_sig("405341574883EC28488BD9");

void* receive_listing_ptr = nullptr;

BOOL g_hooked = FALSE;
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        debug_log(L"DLL_PROCESS_ATTACH");

        receive_listing_ptr = receive_listing_sig.scan();
        if (receive_listing_ptr != nullptr && MH_Initialize() == MH_OK)
        {
            g_hooked = TRUE;

            debug_log(L"Initialized");
            MH_CreateHook(receive_listing_ptr, &receive_listing_new, (LPVOID*)&receive_listing_original);
            MH_EnableHook(receive_listing_ptr);
        }

        break;

    case DLL_PROCESS_DETACH:
        debug_log(L"DLL_PROCESS_DETACH");

        if (g_hooked)
        {
            MH_DisableHook(receive_listing_ptr);
            MH_RemoveHook(receive_listing_ptr);
            MH_Uninitialize();
        }

        break;
    }

    return TRUE;
}

