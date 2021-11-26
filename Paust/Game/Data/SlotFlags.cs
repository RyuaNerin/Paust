namespace Paust.Game.Data
{
    internal class SlotFlags
    {
        public static SlotFlags Instance { get; } = new SlotFlags();
        private SlotFlags()
        {
        }

        public int normal => 0;
        public int greed => 1;
        public int master => 2;
    }
}
