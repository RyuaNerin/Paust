using System.Collections.Generic;

namespace Paust.Game.Data
{
    internal enum Slot : int
    {
        GLD = 1 << 1,
        PGL = 1 << 2,
        MRD = 1 << 3,
        LNC = 1 << 4,
        ARC = 1 << 5,
        CNJ = 1 << 6,
        THM = 1 << 7,
        PLD = 1 << 8,
        MNK = 1 << 9,
        WAR = 1 << 10,
        DRG = 1 << 11,
        BRD = 1 << 12,
        WHM = 1 << 13,
        BLM = 1 << 14,
        ACN = 1 << 15,
        SMN = 1 << 16,
        SCH = 1 << 17,
        ROG = 1 << 18,
        NIN = 1 << 19,
        MCH = 1 << 20,
        DRK = 1 << 21,
        AST = 1 << 22,
        SAM = 1 << 23,
        RDM = 1 << 24,
        BLU = 1 << 25,
        GNB = 1 << 26,
        DNC = 1 << 27,
    }

    internal class SlotConst
    {
        public static SlotConst Instance { get; } = new SlotConst();
        private SlotConst()
        {
        }

        public Dictionary<Slot, Job> SlotToJob = new()
        {
            { Slot.GLD, Job.GLA },
            { Slot.PGL, Job.PGL },
            { Slot.MRD, Job.MRD },
            { Slot.LNC, Job.LNC },
            { Slot.ARC, Job.ARC },
            { Slot.CNJ, Job.CNJ },
            { Slot.THM, Job.THM },
            { Slot.PLD, Job.PLD },
            { Slot.MNK, Job.MNK },
            { Slot.WAR, Job.WAR },
            { Slot.DRG, Job.DRG },
            { Slot.BRD, Job.BRD },
            { Slot.WHM, Job.WHM },
            { Slot.BLM, Job.BLM },
            { Slot.ACN, Job.ACN },
            { Slot.SMN, Job.SMN },
            { Slot.SCH, Job.SCH },
            { Slot.ROG, Job.ROG },
            { Slot.NIN, Job.NIN },
            { Slot.MCH, Job.MCH },
            { Slot.DRK, Job.DRK },
            { Slot.AST, Job.AST },
            { Slot.SAM, Job.SAM },
            { Slot.RDM, Job.RDM },
            { Slot.BLU, Job.BLU },
            { Slot.GNB, Job.GNB },
            { Slot.DNC, Job.DNC },
        };

        public int _tank =
            (int)Slot.GLD | (int)Slot.MRD |
            (int)Slot.PLD | (int)Slot.WAR |
            (int)Slot.DRK |
            (int)Slot.GNB;

        public int _heal =
            (int)Slot.CNJ | (int)Slot.WHM |
            (int)Slot.SCH |
            (int)Slot.AST;

        public int _deal =
            (int)Slot.PGL | (int)Slot.MNK |
            (int)Slot.LNC | (int)Slot.DRG |
            (int)Slot.ROG | (int)Slot.NIN |
            (int)Slot.SAM |
            (int)Slot.ARC | (int)Slot.BRD |
            (int)Slot.MCH |
            (int)Slot.DNC |
            (int)Slot.THM | (int)Slot.BLM |
            (int)Slot.ACN | (int)Slot.SMN |
            (int)Slot.RDM |
            (int)Slot.BLU;

        public int _deal_meele =
            (int)Slot.PGL | (int)Slot.MNK |
            (int)Slot.LNC | (int)Slot.DRG |
            (int)Slot.ROG | (int)Slot.NIN |
            (int)Slot.SAM;

        public int _deal_range =
            (int)Slot.ARC | (int)Slot.BRD |
            (int)Slot.MCH |
            (int)Slot.DNC;

        public int _deal_caster =
            (int)Slot.THM | (int)Slot.BLM |
            (int)Slot.ACN | (int)Slot.SMN |
            (int)Slot.RDM |
            (int)Slot.BLU;

        public int gld = (int)Slot.GLD;
        public int pgl = (int)Slot.PGL;
        public int mrd = (int)Slot.MRD;
        public int lnc = (int)Slot.LNC;
        public int arc = (int)Slot.ARC;
        public int cnj = (int)Slot.CNJ;
        public int thm = (int)Slot.THM;
        public int pld = (int)Slot.PLD;
        public int mnk = (int)Slot.MNK;
        public int war = (int)Slot.WAR;
        public int drg = (int)Slot.DRG;
        public int brd = (int)Slot.BRD;
        public int whm = (int)Slot.WHM;
        public int blm = (int)Slot.BLM;
        public int acn = (int)Slot.ACN;
        public int smn = (int)Slot.SMN;
        public int sch = (int)Slot.SCH;
        public int rog = (int)Slot.ROG;
        public int nin = (int)Slot.NIN;
        public int mch = (int)Slot.MCH;
        public int drk = (int)Slot.DRK;
        public int ast = (int)Slot.AST;
        public int sam = (int)Slot.SAM;
        public int rdm = (int)Slot.RDM;
        public int blu = (int)Slot.BLU;
        public int gnb = (int)Slot.GNB;
        public int dnc = (int)Slot.DNC;
    }
}
