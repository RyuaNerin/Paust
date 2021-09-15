using System.Collections.Generic;
using System.IO;
using System.Text;
using Newtonsoft.Json;

namespace Paust.Core
{
    internal sealed class Settings
    {
        private static Settings instance;
        public static Settings Instance
        {
            get
            {
                if (instance == null)
                {
                    try
                    {
                        using (var fs = File.Open("config.json", FileMode.Open))
                        using (var sr = new StreamReader(fs, Encoding.UTF8))
                        using (var jr = new JsonTextReader(sr))
                        {
                            instance = JsonSerializer.Create().Deserialize<Settings>(jr);
                        }
                    }
                    catch
                    {
                        instance = new Settings();
                    }
                }

                return instance;
            }
        }

        private Settings()
        {
        }

        [JsonProperty("index")]
        public int Index { get; set; }

        [JsonProperty("javascript", ObjectCreationHandling = ObjectCreationHandling.Reuse, DefaultValueHandling = DefaultValueHandling.Populate)]
        public Dictionary<string, string> JavaScript { get; } = new Dictionary<string, string>();

        [JsonProperty("simple", ObjectCreationHandling = ObjectCreationHandling.Reuse, DefaultValueHandling = DefaultValueHandling.Populate)]
        public Dictionary<string, SimpleClass> Simple { get; } = new Dictionary<string, SimpleClass>();

        public class SimpleClass
        {
            [JsonProperty("include")] public List<SimpleRule> Include { get; set; }
            [JsonProperty("exclude")] public List<SimpleRule> Exclude { get; set; }

            public class SimpleRule
            {
                [JsonProperty("type" )] public string Type  { get; set; }
                [JsonProperty("value")] public object Value { get; set; }
            }
        }

        public async void Save()
        {
            using (var fs = File.Create("config.json"))
            {
                fs.SetLength(0);
                await fs.FlushAsync();

                using (var sw = new StreamWriter(fs))
                using (var jw = new JsonTextWriter(sw))
                {
                    JsonSerializer.Create().Serialize(jw, this);
                }
            }
        }

    }
}
