using Dalamud.Game.Command;
using Dalamud.Game.Gui;
using Dalamud.Game.Gui.PartyFinder;
using Dalamud.IoC;
using Dalamud.Plugin;

namespace Paust
{
    public class DalamudInstance
    {
        public static void Initialize(DalamudPluginInterface pluginInterface)
            => pluginInterface.Create<DalamudInstance>();

        [PluginService]
        public static DalamudPluginInterface PluginInterface { get; private set; } = null!;

        [PluginService]
        public static CommandManager Commands { get; private set; } = null!;

        [PluginService]
        public static PartyFinderGui PartyFinderGui { get; private set; } = null!;

        [PluginService]
        public static ChatGui ChatGui { get; private set; } = null!;

        //[PluginService] [RequiredVersion("1.0")] public static SigScanner SigScanner { get; private set; } = null!;
        //[PluginService] [RequiredVersion("1.0")] public static DataManager GameData { get; private set; } = null!;
        //[PluginService] [RequiredVersion("1.0")] public static ClientState ClientState { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static SeStringManager        SeStrings       { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static ChatHandlers           ChatHandlers    { get; private set; } = null!;
        //[PluginService] [RequiredVersion("1.0")] public static Framework Framework { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static GameNetwork            Network         { get; private set; } = null!;
        //[PluginService] [RequiredVersion("1.0")] public static Condition Conditions { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static KeyState               Keys            { get; private set; } = null!;
        //[PluginService] [RequiredVersion("1.0")] public static GameGui GameGui { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static FlyTextGui             FlyTexts        { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static ToastGui               Toasts          { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static JobGauges              Gauges          { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static PartyFinderGui         PartyFinder     { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static BuddyList              Buddies         { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static PartyList              Party           { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static TargetManager          Targets         { get; private set; } = null!;
        //[PluginService] [RequiredVersion("1.0")] public static ObjectTable Objects { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static FateTable              Fates           { get; private set; } = null!;
        //[PluginService][RequiredVersion("1.0")] public static LibcFunction           LibC            { get; private set; } = null!;
    }
}
