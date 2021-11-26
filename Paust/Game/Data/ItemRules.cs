namespace Paust.Game.Data
{
    internal class ItemRules
    {
        public static ItemRules Instance { get; } = new ItemRules();
        private ItemRules()
        {
        }

        public int normal => 0;
        public int greed => 1;
        public int master => 2;
    }
}
