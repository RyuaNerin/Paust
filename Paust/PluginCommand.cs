using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using Dalamud.Game.Command;
using static Dalamud.Game.Command.CommandInfo;

namespace Paust
{
    internal class PluginCommand : IDisposable
    {
        private readonly Plugin plugin;

        private readonly List<string> commands = new();

        public PluginCommand(Plugin plugin)
        {
            this.plugin = plugin;

            foreach (var method in this.GetType().GetMethods(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.InvokeMethod))
            {
                try
                {
                    var commands = method.GetCustomAttribute<CommandAttribute>().Commands;
                    var help = method.GetCustomAttribute<HelpMessageAttribute>().HelpMessage;
                    var showInHelp = method.GetCustomAttribute<ShowInHelpAttribute>()?.ShowInHelp ?? false;

                    var commandInfo = new CommandInfo(method.CreateDelegate<HandlerDelegate>(this))
                    {
                        HelpMessage = help,
                        ShowInHelp = showInHelp,
                    };

                    foreach (var cmd in commands)
                    {
                        DalamudInstance.Commands.AddHandler(cmd, commandInfo);
                        commandInfo.ShowInHelp = false;
                        this.commands.Add(cmd);
                    }
                }
                catch
                {
                }
            }
        }
        ~PluginCommand()
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
                foreach (var s in commands)
                {
                    DalamudInstance.Commands.RemoveHandler(s);
                }
            }
        }

        [Command("/paust", "/파우스트")]
        [HelpMessage("Paust 설정 창을 엽니다.")]
        [ShowInHelp(true)]
        private void OpenWindow(string command, string args)
        {
            this.plugin.ConfigWindow.IsOpen = true;
        }

        [Command("/paustpreset", "/pp", "/프리셋", "/ㅍㄽ", "/ㅍㄹㅅ", "/ㅔㅔ")]
        [HelpMessage("Paust 프로필을 변경합니다.")]
        [ShowInHelp(true)]
        private void ChangeFilter(string command, string args)
        {
            lock (this.plugin.Config)
            {
                PluginConfig.Preset preset;
                var printPresets = false;

                if (string.IsNullOrWhiteSpace(args))
                {
                    preset = PluginConfig.Preset.Empty;
                    printPresets = true;
                }
                else
                {
                    try
                    {
                        preset = this.plugin.Config.Presets.First(e => e.Value.Name == args).Value;
                    }
                    catch (Exception)
                    {
                        preset = PluginConfig.Preset.Empty;
                    }
                }

                this.plugin.Config.SelectedPreset = preset.Guid;

                DalamudInstance.ChatGui.Print($"프리셋을 {preset.Name} 으로 변경하였습니다.");

                if (printPresets)
                {
                    DalamudInstance.ChatGui.Print($"사용 가능한 프리셋:");
                    foreach (var s in this.plugin.Config.Presets)
                    {
                        DalamudInstance.ChatGui.Print($"  - {s.Value.Name}");
                    }
                }
            }
        }

        public class CommandAttribute : Attribute
        {
            public CommandAttribute(params string[] commands)
            {
                this.Commands = commands;
            }
            public string[] Commands { get; }
        }
        public class HelpMessageAttribute : Attribute
        {
            public HelpMessageAttribute(string helpMessage)
            {
                this.HelpMessage = helpMessage;
            }
            public string HelpMessage { get; }
        }
        public class ShowInHelpAttribute : Attribute
        {
            public ShowInHelpAttribute(bool showInHelp)
            {
                this.ShowInHelp = showInHelp;
            }
            public bool ShowInHelp { get; }
        }
    }
}
