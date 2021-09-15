#include <windows.h>
#include <tlhelp32.h>

#include <memory>
#include <filesystem>

BOOL isInjected(DWORD pid, LPCWSTR moduleName, PBYTE* pDllBaseAddress);
BOOL injectDll(DWORD pid, LPCWSTR path);
BOOL uninjectDll(DWORD pid, PBYTE pDllBaseAddress);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int cmdShow)
{
    HWND hFFXIV;
    DWORD pid;
    HANDLE hProc;

    HMODULE hModule;
    PBYTE pDllBaseAddress;

    auto pathStr = std::make_unique<wchar_t[]>(4096);

    hFFXIV = FindWindowW(L"FFXIVGAME", NULL);
    if (hFFXIV == NULL)
    {
        MessageBoxW(NULL, L"No permission OR Final Fantasy XIV is not running", L"PartyFilterInjector", 0);
        return 1;
    }

    if (GetWindowThreadProcessId(hFFXIV, &pid) == 0 || pid == 0)
    {
        MessageBoxW(NULL, L"No permission OR Final Fantasy XIV is not running", L"PartyFilterInjector", 0);
        return 1;
    }

    hProc = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProc == NULL)
    {
        hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (hProc == NULL)
        {
            MessageBoxW(NULL, L"No permission OR Final Fantasy XIV is not running", L"PartyFilterInjector", 0);
            return 1;
        }
    }

    CloseHandle(hProc);

    hModule = GetModuleHandleW(NULL);
    if (hModule == NULL)
    {
        MessageBoxW(NULL, L"No permission OR Final Fantasy XIV is not running", L"PartyFilterInjector", 0);
        return 1;
    }

    if (isInjected(pid, L"PartyFilterCore.dll", &pDllBaseAddress))
    {
        if (!uninjectDll(pid, pDllBaseAddress))
            MessageBoxW(NULL, L"un-injection failed.", L"PartyFilterInjector", 0);
        else
            MessageBoxW(NULL, L"un-injected", L"PartyFilterInjector", 0);

    }
    else
    {
        GetModuleFileNameW(hModule, pathStr.get(), 4096);

        std::filesystem::path path(pathStr.get());
        path = path.parent_path().append(L"PartyFilterCore.dll");

        if (!injectDll(pid, path.c_str()))
            MessageBoxW(NULL, L"injection failed.", L"PartyFilterInjector", 0);
        else
            MessageBoxW(NULL, L"injected", L"PartyFilterInjector", 0);
    }

    return 0;
}

BOOL isInjected(DWORD pid, LPCWSTR moduleName, PBYTE* pDllBaseAddress)
{
    BOOL res = FALSE;

    MODULEENTRY32W snapEntry = { 0 };
    HANDLE hSnapshot;

    snapEntry.dwSize = sizeof(MODULEENTRY32W);
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hSnapshot == NULL) return FALSE;

    if (Module32FirstW(hSnapshot, &snapEntry))
    {
        do
        {
            if (!lstrcmp(snapEntry.szModule, moduleName))
            {
                *pDllBaseAddress = snapEntry.modBaseAddr;
                res = TRUE;
                break;
            }
        } while (Module32NextW(hSnapshot, &snapEntry));
    }
    CloseHandle(hSnapshot);

    return res;
}

BOOL injectDll(DWORD pid, LPCWSTR path)
{
    BOOL result = FALSE;

    HMODULE hKernel32;
    LPTHREAD_START_ROUTINE lpLoadLibrary;

    LPVOID pBuff;
    DWORD pBuffSize;

    HANDLE hProcess;
    HANDLE hThread;

    DWORD exitCode;

    hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (hKernel32 == NULL)
        return FALSE;

    lpLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
    if (lpLoadLibrary == NULL)
        return FALSE;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL)
        return FALSE;

    pBuffSize = (lstrlen(path) + 1) * sizeof(TCHAR);
    pBuff = VirtualAllocEx(hProcess, NULL, pBuffSize, MEM_COMMIT, PAGE_READWRITE);
    if (pBuff != NULL)
    {
        if (WriteProcessMemory(hProcess, pBuff, (LPVOID)path, pBuffSize, NULL))
        {
            hThread = CreateRemoteThread(hProcess, NULL, 0, lpLoadLibrary, pBuff, 0, NULL);
            if (hThread != NULL)
            {
                WaitForSingleObject(hThread, INFINITE);
                result = GetExitCodeThread(hThread, &exitCode) && exitCode;

                CloseHandle(hThread);
            }
        }

        VirtualFreeEx(hProcess, pBuff, 0, MEM_RELEASE);
    }

    CloseHandle(hProcess);

    return result;
}

BOOL uninjectDll(DWORD pid, PBYTE pDllBaseAddress)
{
    BOOL result = FALSE;

    HMODULE hKernel32;
    LPTHREAD_START_ROUTINE lpFreeLibrary;

    HANDLE hProcess;
    HANDLE hThread;

    DWORD exitCode;

    hKernel32 = GetModuleHandleW(L"kernel32.dll");
    if (hKernel32 == NULL)
        return FALSE;

    lpFreeLibrary = (LPTHREAD_START_ROUTINE)(GetProcAddress(hKernel32, "FreeLibrary"));
    if (lpFreeLibrary == NULL)
        return FALSE;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL)
        return FALSE;

    hThread = CreateRemoteThread(hProcess, NULL, 0, lpFreeLibrary, (PVOID)pDllBaseAddress, 0, NULL);
    if (hThread != NULL)
    {
        WaitForSingleObject(hThread, INFINITE);
        result = GetExitCodeThread(hThread, &exitCode) && exitCode;

        CloseHandle(hThread);
    }

    CloseHandle(hProcess);

    return result;
}
