namespace Paust.Game.Data
{
    internal class ContentIdConst
    {
        public static ContentIdConst Instance { get; } = new ContentIdConst();
        private ContentIdConst()
        {
        }

        public int E9S => 750;
        public int E10S => 748;
        public int E11S => 752;
        public int E12S => 759;

        public int GUNNHILDR => 760;
        public int GUNNHILDRS => 761;

        public int LEVIATHAN => 776;
        public int DIAMOND => 782;

        public int BAHAMUT => 280;
        public int ULTIMA => 539;
        public int ALEXANDER => 694;

        public int Hydaelyns => 996;
        public int Zodiarks => 993;

        public int P1S => 1003;
        public int P2S => 1009;
        public int P3S => 1007;
        public int P4S => 1005;
    }
}
