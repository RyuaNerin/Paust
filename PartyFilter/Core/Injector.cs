using System;
using System.IO;
using System.IO.Pipes;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace PartyFilter.Core
{
    internal static class Injector
    {
        public const string DllName = "PartyFilterCore.dll";

#pragma warning disable CA1032, CA2229, CA2237
        public class InjectionException : Exception
#pragma warning restore CA1032, CA2229, CA2237
        {
        }

        public class IpcInput
        {
            [JsonProperty("shortname_enabled")]
            public bool ShortNameEnabled { get; set; }

            [JsonProperty("filter_enabled")]
            public bool FilterEnabled { get; set; }

            [JsonProperty("filter_script", NullValueHandling = NullValueHandling.Ignore)]
            public string FilterScript { get; set; }
        }

        public class IpcOutput
        {
            [JsonProperty("succeed")] public bool Succeed { get; set; }
            [JsonProperty("error"  )] public string Error { get; set; }
        }

        public static async Task<IpcOutput> ApplyFilter(IpcInput data)
        {
            var gameWindow = NativeMethods.FindWindowByClass("FFXIVGAME", IntPtr.Zero);
            if (gameWindow == IntPtr.Zero)
                throw new InjectionException();

            if (NativeMethods.GetWindowThreadProcessId(gameWindow, out var pid) == 0)
                throw new InjectionException();

            if (!CheckInjection(pid, DllName, out _) && !InjectDll(pid))
                throw new InjectionException();

            return await SendToDll(pid, data);
        }
        public static async void ApplyFilterWithoutInjection(IpcInput data)
        {
            var gameWindow = NativeMethods.FindWindowByClass("FFXIVGAME", IntPtr.Zero);
            if (gameWindow == IntPtr.Zero)
                return;

            if (NativeMethods.GetWindowThreadProcessId(gameWindow, out var pid) == 0)
                return;

            await SendToDll(pid, data);
        }

        private static bool CheckInjection(uint pid, string moduleName, out IntPtr baseAddress)
        {
            var snapEntry = new NativeMethods.MODULEENTRY32
            {
                dwSize = (uint)Marshal.SizeOf(typeof(NativeMethods.MODULEENTRY32)),
            };

            using (var snapShot = new IntPtrWrap(NativeMethods.CreateToolhelp32Snapshot(NativeMethods.SnapshotFlags.Module, pid)))
            {
                if (NativeMethods.Module32FirstW(snapShot, ref snapEntry))
                {
                    do
                    {
                        if (snapEntry.szModule == moduleName)
                        {
                            baseAddress = snapEntry.modBaseAddr;
                            return true;
                        }
                    } while (NativeMethods.Module32NextW(snapShot, ref snapEntry));
                }
            }

            baseAddress = IntPtr.Zero;
            return false;
        }

        private static bool InjectDll(uint pid)
        {
            var dllPath = Path.Combine(
                Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location),
                DllName
            );
            dllPath += "\0";

            var lpKernel32 = NativeMethods.GetModuleHandle("kernel32.dll");
            if (lpKernel32 == IntPtr.Zero) throw new InjectionException();

            var lpLoadLibrary = NativeMethods.GetProcAddress(lpKernel32, "LoadLibraryW");
            if (lpLoadLibrary == IntPtr.Zero) throw new InjectionException();

            using (var hProcess = new IntPtrWrap(NativeMethods.OpenProcess(NativeMethods.ProcessAccessFlags.All, false, pid)))
            {
                var pathSize = new IntPtr(Encoding.Unicode.GetByteCount(dllPath));
                var pathBuff = Encoding.Unicode.GetBytes(dllPath);

                using (var pBuff = new IntPtrWrap(
                    NativeMethods.VirtualAllocEx(hProcess, IntPtr.Zero, pathSize, NativeMethods.AllocationType.Commit, NativeMethods.MemoryProtection.ReadWrite),
                    ptr => NativeMethods.VirtualFreeEx(hProcess, ptr, IntPtr.Zero, NativeMethods.AllocationType.Release))
                )
                {
                    if (!NativeMethods.WriteProcessMemory(hProcess, pBuff, pathBuff, pathSize, out var written) || written != pathSize) return false;

                    using (var hThread = new IntPtrWrap(NativeMethods.CreateRemoteThread(hProcess, IntPtr.Zero, IntPtr.Zero, lpLoadLibrary, pBuff, 0, out var thread_id)))
                    {
                        _ = NativeMethods.WaitForSingleObject(hThread, uint.MaxValue);
                        if (NativeMethods.GetExitCodeThread(hThread, out var exitCode))
                            return exitCode != 0;
                    }
                }
            }

            return false;
        }

        private static readonly JsonSerializerSettings jsonSettings = new JsonSerializerSettings
        {
            StringEscapeHandling = StringEscapeHandling.Default,
            Formatting = Formatting.None,
        };

        private static async Task<IpcOutput> SendToDll(uint pid, IpcInput rpcData)
        {
            var dataRaw = Encoding.UTF8.GetBytes(JsonConvert.SerializeObject(rpcData, jsonSettings) + "\0");

            var pipeName = $"PartyFilterCore\\{pid}";

            using (var readBuffer = new MemoryStream())
            {
                using (var cts = new CancellationTokenSource())
                {
                    cts.CancelAfter(TimeSpan.FromSeconds(5));

                    var token = cts.Token;


                    using (var pipe = new NamedPipeClientStream(".", pipeName, PipeDirection.InOut, PipeOptions.Asynchronous))
                    {
                        await pipe.ConnectAsync(1000, token);

                        pipe.ReadMode = PipeTransmissionMode.Message;
                        await pipe.WriteAsync(dataRaw, 0, dataRaw.Length, token);

                        await pipe.CopyToAsync(readBuffer, 4096, token);
                    }
                }

                readBuffer.Position = 0;
                using (var sr = new StreamReader(readBuffer, Encoding.UTF8))
                using (var jr = new JsonTextReader(sr))
                {
                    return new JsonSerializer().Deserialize<IpcOutput>(jr);
                }
            }
        }

        private class IntPtrWrap : IDisposable
        {
            public IntPtrWrap(IntPtr ptr) : this(ptr, e => NativeMethods.CloseHandle(e))
            {
            }
            public IntPtrWrap(IntPtr ptr, Action<IntPtr> action)
            {
                if (ptr == IntPtr.Zero) throw new InjectionException();

                this.Handle = ptr;
                this.action = action;
            }

            public IntPtr Handle { get; }

            private readonly Action<IntPtr> action;

            ~IntPtrWrap()
            {
                this.Dispose(false);
            }

            public void Dispose()
            {
                this.Dispose(true);
                GC.SuppressFinalize(this);
            }

            private bool m_disposed = false;
            protected virtual void Dispose(bool disposing)
            {
                if (this.m_disposed) return;
                this.m_disposed = true;

                if (disposing)
                {
                    this.action(this.Handle);
                }
            }

            public static implicit operator IntPtr(IntPtrWrap alloc)
            {
                return alloc.Handle;
            }
        }

        private static class NativeMethods
        {
            [DllImport("user32.dll", EntryPoint = "FindWindow", CharSet = CharSet.Unicode)]
            public static extern IntPtr FindWindowByClass(string ZeroOnly, IntPtr lpWindowName);

            [DllImport("user32.dll")]
            public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);

            [DllImport("kernel32.dll")]
            public static extern IntPtr OpenProcess(ProcessAccessFlags processAccess, bool bInheritHandle, uint processId);

            [DllImport("kernel32.dll")]
            public static extern bool CloseHandle(IntPtr hHandle);

            [DllImport("kernel32.dll")]
            public static extern IntPtr CreateToolhelp32Snapshot(SnapshotFlags dwFlags, uint th32ProcessID);

            [DllImport("kernel32.dll")]
            public static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, IntPtr dwSize, AllocationType flAllocationType, MemoryProtection flProtect);

            [DllImport("kernel32.dll")]
            public static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, IntPtr nSize, out IntPtr lpNumberOfBytesWritten);

            [DllImport("kernel32.dll")]
            public static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, IntPtr dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, out IntPtr lpThreadId);

            [DllImport("kernel32.dll", CharSet = CharSet.Ansi)]
            public static extern IntPtr GetProcAddress(IntPtr hModule, string procName);

            [DllImport("kernel32.dll")]
            public static extern uint WaitForSingleObject(IntPtr hHandle, uint dwMilliseconds);

            [DllImport("kernel32.dll")]
            public static extern bool GetExitCodeThread(IntPtr hThread, out uint lpExitCode);

            [DllImport("kernel32.dll")]
            public static extern bool VirtualFreeEx(IntPtr hProcess, IntPtr lpAddress, IntPtr dwSize, AllocationType dwFreeType);

            [DllImport("kernel32.dll")]
            public static extern bool Module32FirstW(IntPtr hSnapshot, ref MODULEENTRY32 lpme);

            [DllImport("kernel32.dll")]
            public static extern bool Module32NextW(IntPtr hSnapshot, ref MODULEENTRY32 lpme);

            [DllImport("kernel32.dll", CharSet = CharSet.Unicode)]
            public static extern IntPtr GetModuleHandle(string lpModuleName);

            [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
            public struct COPYDATASTRUCT
            {
                public IntPtr dwData;
                public int cbData;
                public IntPtr lpData;
            }

            [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
            public struct PROCESSENTRY32
            {
                public const int MAX_PATH = 260;
                public uint dwSize;
                public uint cntUsage;
                public uint th32ProcessID;
                public IntPtr th32DefaultHeapID;
                public uint th32ModuleID;
                public uint cntThreads;
                public uint th32ParentProcessID;
                public int pcPriClassBase;
                public uint dwFlags;
                [MarshalAs(UnmanagedType.ByValTStr, SizeConst = MAX_PATH)]
                public string szExeFile;
            }

            [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
            public struct MODULEENTRY32
            {
                public uint dwSize;
                public uint th32ModuleID;
                public uint th32ProcessID;
                public uint GlblcntUsage;
                public uint ProccntUsage;
                public IntPtr modBaseAddr;
                public uint modBaseSize;
                public IntPtr hModule;
                [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
                public string szModule;
                [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
                public string szExePath;
            }

            [Flags]
            public enum ProcessAccessFlags : uint
            {
                All = 0x001F0FFF,
                Terminate = 0x00000001,
                CreateThread = 0x00000002,
                VirtualMemoryOperation = 0x00000008,
                VirtualMemoryRead = 0x00000010,
                VirtualMemoryWrite = 0x00000020,
                DuplicateHandle = 0x00000040,
                CreateProcess = 0x000000080,
                SetQuota = 0x00000100,
                SetInformation = 0x00000200,
                QueryInformation = 0x00000400,
                QueryLimitedInformation = 0x00001000,
                Synchronize = 0x00100000
            }

            [Flags]
            public enum SnapshotFlags : uint
            {
                HeapList = 0x00000001,
                Process = 0x00000002,
                Thread = 0x00000004,
                Module = 0x00000008,
                Module32 = 0x00000010,
                Inherit = 0x80000000,
                All = 0x0000001F,
                NoHeaps = 0x40000000
            }

            [Flags]
            public enum AllocationType : uint
            {
                Commit = 0x1000,
                Reserve = 0x2000,
                Decommit = 0x4000,
                Release = 0x8000,
                Reset = 0x80000,
                Physical = 0x400000,
                TopDown = 0x100000,
                WriteWatch = 0x200000,
                LargePages = 0x20000000
            }

            [Flags]
            public enum MemoryProtection : uint
            {
                Execute = 0x10,
                ExecuteRead = 0x20,
                ExecuteReadWrite = 0x40,
                ExecuteWriteCopy = 0x80,
                NoAccess = 0x01,
                ReadOnly = 0x02,
                ReadWrite = 0x04,
                WriteCopy = 0x08,
                GuardModifierflag = 0x100,
                NoCacheModifierflag = 0x200,
                WriteCombineModifierflag = 0x400
            }
        }
    }
}
