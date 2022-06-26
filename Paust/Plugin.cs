using System;
using System.Runtime.InteropServices;
using Dalamud.Game.Gui.PartyFinder;
using Dalamud.Interface.Windowing;
using Dalamud.Logging;
using Dalamud.Plugin;
using Paust.Game;
using Paust.Game.Data;
using Paust.PartyFinder;

namespace Paust
{
    public class Plugin : IDalamudPlugin, IDisposable
    {
        private const int ShortNameLength = 10;
        private const int ShortNameLengthWithServerMark = 5;

        public string Name => "Paust";

        internal PluginConfig  Config        { get; }
        internal JintWrapper   JintWrapper   { get; }
        internal PluginCommand Command       { get; }
        internal PluginUI      ConfigWindow      { get; }

        private readonly IntPtr memory;
        private readonly WindowSystem windowSystem = new("Paust");

        public Plugin(DalamudPluginInterface pluginInterface)
        {
            try
            {
                DalamudInstance.Initialize(pluginInterface);

                this.memory = Marshal.AllocHGlobal(PartyFinderPacket.PacketSize);

                this.Config = PluginConfig.Load(pluginInterface);
                this.JintWrapper = new JintWrapper();

                DalamudInstance.PartyFinderGui.Enable();
                DalamudInstance.PartyFinderGui.ReceiveRawPacket += this.PartyFinderGui_ReceiveRawPacket;

                this.ConfigWindow = new PluginUI(this);
                this.windowSystem.AddWindow(this.ConfigWindow);
                DalamudInstance.PluginInterface.UiBuilder.Draw += this.OnDraw;
                DalamudInstance.PluginInterface.UiBuilder.OpenConfigUi += this.UiBuilder_OpenConfigUi;

                this.Command = new PluginCommand(this);
            }
            catch (Exception ex)
            {
                PluginLog.Error(ex, "Paust .Ctor");
                this.Dispose(true);

                throw;
            }
        }
        ~Plugin()
        {
            this.Dispose(false);
        }

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }
        private bool disposed;
        protected void Dispose(bool disposing)
        {
            if (this.disposed) return;
            this.disposed = true;

            if (disposing)
            {
                this.Config.Save();

                this.Command.Dispose();

                DalamudInstance.PluginInterface.UiBuilder.Draw -= this.OnDraw;
                DalamudInstance.PluginInterface.UiBuilder.OpenConfigUi -= this.UiBuilder_OpenConfigUi;
                this.windowSystem.RemoveAllWindows();
                this.ConfigWindow.Dispose();

                DalamudInstance.PartyFinderGui.ReceiveRawPacket -= this.PartyFinderGui_ReceiveRawPacket;

                Marshal.FreeHGlobal(this.memory);
            }
        }

        private void UiBuilder_OpenConfigUi()
        {
            this.ConfigWindow.IsOpen = true;
        }

        private void OnDraw()
        {
            try
            {
                this.windowSystem.Draw();
            }
            catch
            {
            }
        }

        // Dalamud.Game.Gui.PartyFinder.HandleListingEvents
        private void PartyFinderGui_ReceiveRawPacket(PartyFinderRawEventArgs args)
        {
            args.Handled = true;

            var dataPtr = args.Data + 0x10;

            var packet = Marshal.PtrToStructure<PartyFinderPacket>(dataPtr);

            var needToRewrite = false;

            lock (this.Config)
            {
                if (this.Config.SelectedPreset != Guid.Empty &&
                    this.Config.Presets.TryGetValue(this.Config.SelectedPreset, out var preset))
                {
                    for (var i = 0; i < packet.Listings.Length; i++)
                    {
                        try
                        {
                            if (!this.JintWrapper.Eval(preset.Javascript, packet.Listings[i]))
                            {
                                packet.Listings[i] = default;
                                needToRewrite = true;
                            }
                        }
                        catch
                        {

                        }
                    }
                }

                for (var i = 0; i < packet.Listings.Length; i++)
                {
                    if (packet.Listings[i].Id == 0)
                    {
                        continue;
                    }

                    if (this.Config.ModifyReservation)
                    {
                        for (var k = 0; k < packet.Listings[i].JobsPresent.Length; k++)
                        {
                            if (packet.Listings[i].JobsPresent[k] != 0)
                            {
                                continue;
                            }

                            // 확직인가?
                            int available_slot_jobs = 0;
                            Slot available_slot_flag = 0;
                            Job available_slot_job = 0;
                            foreach (var v in SlotConst.Instance.SlotToJob)
                            {
                                if ((packet.Listings[i].Slots[k] & (int)v.Key) != 0)
                                {
                                    available_slot_jobs++;
                                    available_slot_flag = v.Key;
                                    available_slot_job = v.Value;
                                }
                            }

                            if (available_slot_jobs == 1)
                            {
                                packet.Listings[i].Slots[k] = (uint)available_slot_flag;
                                packet.Listings[i].JobsPresent[k] = (byte)available_slot_job;

                                needToRewrite = true;
                            }
                        }
                    }

                    if (this.Config.HideServer)
                    {
                        packet.Listings[i].CurrentWorld = 0;
                        needToRewrite = true;
                    }

                    if (this.Config.HideOptions)
                    {
                        // 잡 중복 없음 제거
                        packet.Listings[i].SearchArea &= (byte.MaxValue - (1 << 5));

                        // 완료 목적, 연습, 반복공략 제거
                        packet.Listings[i].Objective = 0;

                        // 완료 조건 제거
                        packet.Listings[i].Conditions = 0;

                        // 새싹 제거
                        packet.Listings[i].BeginnersWelcome = 0;

                        needToRewrite = true;
                    }

                    if (this.Config.ShortName)
                    {
                        int len_target = this.Config.HideOptions ? ShortNameLength : ShortNameLengthWithServerMark;

                        var str = SeString.ToString(packet.Listings[i].Name);

                        var strLen = 0;
                        foreach (var c in str)
                            strLen += (0 < c && c <= 128) ? 1 : 2;

                        if (strLen >= len_target)
                        {
                            var len = 0;
                            var char_width = 0;

                            foreach (var c in str)
                            {
                                char_width += (0 < c && c <= 128) ? 1 : 2;
                                if (char_width >= len_target)
                                {
                                    break;
                                }

                                len++;
                            }

                            if (len < str.Length)
                            {
                                str = str.Substring(0, len) + "..";

                                var strBytes = SeString.ToBytes(str);

                                for (var k = 0; k < packet.Listings[i].Name.Length; k++)
                                {
                                    if (k < strBytes.Length)
                                    {
                                        packet.Listings[i].Name[k] = strBytes[k];
                                    }
                                    else
                                    {
                                        packet.Listings[i].Name[k] = 0;
                                    }
                                }

                                needToRewrite = true;
                            }
                        }
                    }
                }
            }

            if (!needToRewrite)
            {
                return;
            }

            Marshal.StructureToPtr(packet, this.memory, false);

            unsafe
            {
                Buffer.MemoryCopy((void*)this.memory, (void*)dataPtr, PartyFinderPacket.PacketSize, PartyFinderPacket.PacketSize);
            }
        }
    }
}
