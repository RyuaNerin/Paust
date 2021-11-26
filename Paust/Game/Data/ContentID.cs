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
    }
}
