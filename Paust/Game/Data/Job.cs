namespace Paust.Game.Data
{
    internal enum Job : byte
    {
        ADV = 0, // 모험가
        GLA = 1, // 검술사
        PGL = 2, // 격투사
        MRD = 3, // 도끼술사
        LNC = 4, // 창술사
        ARC = 5, // 궁술사
        CNJ = 6, // 환술사
        THM = 7, // 주술사
        CRP = 8, // 목수
        BSM = 9, // 대장장이
        ARM = 10, // 갑주제작사
        GSM = 11, // 보석공예가
        LTW = 12, // 가죽공예가
        WVR = 13, // 재봉사
        ALC = 14, // 연금술사
        CUL = 15, // 요리사
        MIN = 16, // 광부
        BTN = 17, // 원예가
        FSH = 18, // 어부
        PLD = 19, // 나이트
        MNK = 20, // 몽크
        WAR = 21, // 전사
        DRG = 22, // 용기사
        BRD = 23, // 음유시인
        WHM = 24, // 백마도사
        BLM = 25, // 흑마도사
        ACN = 26, // 비술사
        SMN = 27, // 소환사
        SCH = 28, // 학자
        ROG = 29, // 쌍검사
        NIN = 30, // 닌자
        MCH = 31, // 기공사
        DRK = 32, // 암흑기사
        AST = 33, // 점성술사
        SAM = 34, // 사무라이
        RDM = 35, // 적마도사
        BLU = 36, // 청마도사
        GNB = 37, // 건브레이커
        DNC = 38, // 무도가
        RPR = 39, // 리퍼
        SGE = 40, // 현자
    }
    internal class JobConst
    {
        public static JobConst Instance { get; } = new JobConst();
        private JobConst()
        {
        }

        public int adv => (int)Job.ADV;
        public int gla => (int)Job.GLA;
        public int pgl => (int)Job.PGL;
        public int mrd => (int)Job.MRD;
        public int lnc => (int)Job.LNC;
        public int arc => (int)Job.ARC;
        public int cnj => (int)Job.CNJ;
        public int thm => (int)Job.THM;
        public int crp => (int)Job.CRP;
        public int bsm => (int)Job.BSM;
        public int arm => (int)Job.ARM;
        public int gsm => (int)Job.GSM;
        public int ltw => (int)Job.LTW;
        public int wvr => (int)Job.WVR;
        public int alc => (int)Job.ALC;
        public int cul => (int)Job.CUL;
        public int min => (int)Job.MIN;
        public int btn => (int)Job.BTN;
        public int fsh => (int)Job.FSH;
        public int pld => (int)Job.PLD;
        public int mnk => (int)Job.MNK;
        public int war => (int)Job.WAR;
        public int drg => (int)Job.DRG;
        public int brd => (int)Job.BRD;
        public int whm => (int)Job.WHM;
        public int blm => (int)Job.BLM;
        public int acn => (int)Job.ACN;
        public int smn => (int)Job.SMN;
        public int sch => (int)Job.SCH;
        public int rog => (int)Job.ROG;
        public int nin => (int)Job.NIN;
        public int mch => (int)Job.MCH;
        public int drk => (int)Job.DRK;
        public int ast => (int)Job.AST;
        public int sam => (int)Job.SAM;
        public int rdm => (int)Job.RDM;
        public int blu => (int)Job.BLU;
        public int gnb => (int)Job.GNB;
        public int dnc => (int)Job.DNC;
        public int rpr => (int)Job.RPR;
        public int sge => (int)Job.SGE;
    }
}
