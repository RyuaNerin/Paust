using System;
using System.Collections.Generic;
using Dalamud.Configuration;
using Dalamud.Plugin;
using Newtonsoft.Json;

namespace Paust
{
    internal class PluginConfig : IPluginConfiguration
    {
        public int Version { get; set; } = 1;

        public bool ShortName { get; set; }

        public bool HideServer { get; set; }

        public bool HideOptions { get; set; }

        public bool ModifyReservation { get; set; }

        public Guid SelectedPreset { get; set; } = Guid.Empty;
        public Dictionary<Guid, Preset> Presets { get; } = new Dictionary<Guid, Preset>();

        public class Preset
        {
            public static Preset Empty { get; } = new Preset
            {
                Guid = Guid.Empty,
                Name = "사용 안함",
            };

            [JsonIgnore]
            public bool IsEmpty => this.Guid == Guid.Empty;

            [JsonIgnore]
            public string NameId => $"{this.Name}##{this.Guid}";

            public Guid Guid { get; set; }
            public string Name { get; set; }
            public string Javascript { get; set; }
        }

        public static PluginConfig Load(DalamudPluginInterface pluginInterface)
        {
            if (pluginInterface.GetPluginConfig() is PluginConfig config)
            {
                config = Migrate(config);
                return config;
            }

            config = new PluginConfig();
            config.Save();
            return config;
        }

        public static PluginConfig Migrate(PluginConfig config)
        {
            return config;
        }

        public void Save()
        {
            DalamudInstance.PluginInterface.SavePluginConfig(this);
        }
    }
}
