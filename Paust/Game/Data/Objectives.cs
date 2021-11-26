namespace Paust.Game.Data
{
    internal class Objectives
    {
        public static Objectives Instance { get; } = new Objectives();
        private Objectives()
        {
        }

        public int none            => 0;
        public int duty_completion => 1;
        public int practice        => 2;
        public int loot            => 4;
    }
}
