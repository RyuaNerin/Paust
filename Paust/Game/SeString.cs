using System.Linq;
using Dalamud.Game.Text.SeStringHandling.Payloads;
using DSeString = Dalamud.Game.Text.SeStringHandling.SeString;

namespace Paust.Game
{
    internal static class SeString
    {
        public static string ToString(byte[] b)
        {
            return DSeString.Parse(b.TakeWhile(b => b != 0).ToArray()).TextValue;
        }

        public static byte[] ToBytes(string str)
        {
            return new TextPayload(str).Encode();
        }
    }
}
