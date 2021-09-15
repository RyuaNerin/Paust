#include "ffxiv_data.h"
#include "defer.h"
#include "utils.h"
#include "debug_log.h"

static const std::array<uint8_t, sizeof(party_finder_packet_item)> sample_data = {
   0x00, 0x00, 0x00, 0x00, 0x13, 0x88, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x88, 0x14, 0xcf, 0x3b, 0x00, 0x00, 0x40, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0xec, 0x02,
   0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00,
   0x05, 0x00, 0x1d, 0x08, 0x00, 0x00, 0x00, 0x04, 0x40, 0x80,
   0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xc5, 0xcf, 0x36, 0x61, 0xde, 0x0b, 0x00, 0x00, 0x06, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x1d, 0x08, 0x1d, 0x08, 0x40, 0x08,
   0x03, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01,
   0x0a, 0x05, 0x20, 0x04, 0x00, 0x20, 0x00, 0x00, 0x40, 0x20,
   0x42, 0x00, 0xb4, 0xda, 0x9d, 0x09, 0xb4, 0xda, 0x9d, 0x09,
   0x13, 0x26, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0xeb, 0xb9,
   0xa1, 0xeb, 0xb9, 0xa1, 0xec, 0x9d, 0xb4, 0xec, 0xb2, 0x98,
   0xec, 0x9d, 0x8c, 0xeb, 0xb3, 0xb4, 0xeb, 0x8b, 0x88, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xea, 0xb0, 0xb1, 0xec, 0x8b, 0xa0, 0x20, 0x33, 0xed, 0x81,
   0xb4, 0x20, 0xec, 0x82, 0xac, 0xec, 0x9e, 0xa5, 0xed, 0x8c,
   0x9f, 0x20, 0x38, 0xeb, 0xb6, 0x84, 0x20, 0x32, 0x30, 0xec,
   0xb4, 0x88, 0x20, 0xeb, 0x82, 0xb4, 0xec, 0x99, 0xb8, 0x20,
   0xec, 0xbb, 0xb7, 0x20, 0x31, 0xed, 0x81, 0xb4, 0xeb, 0x8b,
   0xb9, 0x20, 0xec, 0x9d, 0xb8, 0xeb, 0x8b, 0xb9, 0x20, 0x34,
   0x30, 0x20, 0x20, 0xec, 0xa7, 0x81, 0xec, 0x97, 0x85, 0x20,
   0xec, 0x95, 0x88, 0xea, 0xb0, 0x80, 0xeb, 0xa6, 0xbc, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const party_finder_packet_item* const party_finder_packet_item_sample = (party_finder_packet_item*)sample_data.data();

////////////////////////////////////////////////////////////////////////////////////////////////////

const std::map<const char*, uint32_t> ffxiv_job_id =
{
    { "adv",  0 }, // 모험가
    { "gla",  1 }, // 검술사
    { "pgl",  2 }, // 격투사
    { "mrd",  3 }, // 도끼술사
    { "lnc",  4 }, // 창술사
    { "arc",  5 }, // 궁술사
    { "cnj",  6 }, // 환술사
    { "thm",  7 }, // 주술사
    { "crp",  8 }, // 목수
    { "bsm",  9 }, // 대장장이
    { "arm", 10 }, // 갑주제작사
    { "gsm", 11 }, // 보석공예가
    { "ltw", 12 }, // 가죽공예가
    { "wvr", 13 }, // 재봉사
    { "alc", 14 }, // 연금술사
    { "cul", 15 }, // 요리사
    { "min", 16 }, // 광부
    { "btn", 17 }, // 원예가
    { "fsh", 18 }, // 어부
    { "pld", 19 }, // 나이트
    { "mnk", 20 }, // 몽크
    { "war", 21 }, // 전사
    { "drg", 22 }, // 용기사
    { "brd", 23 }, // 음유시인
    { "whm", 24 }, // 백마도사
    { "blm", 25 }, // 흑마도사
    { "acn", 26 }, // 비술사
    { "smn", 27 }, // 소환사
    { "sch", 28 }, // 학자
    { "rog", 29 }, // 쌍검사
    { "nin", 30 }, // 닌자
    { "mch", 31 }, // 기공사
    { "drk", 32 }, // 암흑기사
    { "ast", 33 }, // 점성술사
    { "sam", 34 }, // 사무라이
    { "rdm", 35 }, // 적마도사
    { "blu", 36 }, // 청마도사
    { "gnb", 37 }, // 건브레이커
    { "dnc", 38 }, // 무도가
};

#define JOB_FLAG(NAME, VALUE) constexpr uint32_t SLOT_FLAG_##NAME = (VALUE);
JOB_FLAG(GLD, 1 << 1)
JOB_FLAG(PGL, 1 << 2)
JOB_FLAG(MRD, 1 << 3)
JOB_FLAG(LNC, 1 << 4)
JOB_FLAG(ARC, 1 << 5)
JOB_FLAG(CNJ, 1 << 6)
JOB_FLAG(THM, 1 << 7)
JOB_FLAG(PLD, 1 << 8)
JOB_FLAG(MNK, 1 << 9)
JOB_FLAG(WAR, 1 << 10)
JOB_FLAG(DRG, 1 << 11)
JOB_FLAG(BRD, 1 << 12)
JOB_FLAG(WHM, 1 << 13)
JOB_FLAG(BLM, 1 << 14)
JOB_FLAG(ACN, 1 << 15)
JOB_FLAG(SMN, 1 << 16)
JOB_FLAG(SCH, 1 << 17)
JOB_FLAG(ROG, 1 << 18)
JOB_FLAG(NIN, 1 << 19)
JOB_FLAG(MCH, 1 << 20)
JOB_FLAG(DRK, 1 << 21)
JOB_FLAG(AST, 1 << 22)
JOB_FLAG(SAM, 1 << 23)
JOB_FLAG(RDM, 1 << 24)
JOB_FLAG(BLU, 1 << 25)
JOB_FLAG(GNB, 1 << 26)
JOB_FLAG(DNC, 1 << 27)

const std::map<const char*, uint32_t> ffxiv_slot_flag =
{
    { "_tank"        , SLOT_FLAG_GLD | SLOT_FLAG_MRD | SLOT_FLAG_PLD | SLOT_FLAG_WAR | SLOT_FLAG_DRK | SLOT_FLAG_GNB },

    { "_heal"        , SLOT_FLAG_CNJ | SLOT_FLAG_WHM | SLOT_FLAG_SCH | SLOT_FLAG_AST },

    { "_deal"        , SLOT_FLAG_PGL | SLOT_FLAG_LNC | SLOT_FLAG_MNK | SLOT_FLAG_DRG | SLOT_FLAG_ROG | SLOT_FLAG_NIN | SLOT_FLAG_SAM |
                       SLOT_FLAG_ARC | SLOT_FLAG_BRD | SLOT_FLAG_MCH | SLOT_FLAG_DNC |
                       SLOT_FLAG_THM | SLOT_FLAG_BLM | SLOT_FLAG_ACN | SLOT_FLAG_SMN | SLOT_FLAG_RDM | SLOT_FLAG_BLU },

    { "_deal_meele"  , SLOT_FLAG_PGL | SLOT_FLAG_LNC | SLOT_FLAG_MNK | SLOT_FLAG_DRG | SLOT_FLAG_ROG | SLOT_FLAG_NIN | SLOT_FLAG_SAM },
    { "_deal_range"  , SLOT_FLAG_ARC | SLOT_FLAG_BRD | SLOT_FLAG_MCH | SLOT_FLAG_DNC },
    { "_deal_caster" , SLOT_FLAG_THM | SLOT_FLAG_BLM | SLOT_FLAG_ACN | SLOT_FLAG_SMN | SLOT_FLAG_RDM | SLOT_FLAG_BLU },

    { "gld", SLOT_FLAG_GLD },
    { "pgl", SLOT_FLAG_PGL },
    { "mrd", SLOT_FLAG_MRD },
    { "lnc", SLOT_FLAG_LNC },
    { "arc", SLOT_FLAG_ARC },
    { "cnj", SLOT_FLAG_CNJ },
    { "thm", SLOT_FLAG_THM },
    { "pld", SLOT_FLAG_PLD },
    { "mnk", SLOT_FLAG_MNK },
    { "war", SLOT_FLAG_WAR },
    { "drg", SLOT_FLAG_DRG },
    { "brd", SLOT_FLAG_BRD },
    { "whm", SLOT_FLAG_WHM },
    { "blm", SLOT_FLAG_BLM },
    { "acn", SLOT_FLAG_ACN },
    { "smn", SLOT_FLAG_SMN },
    { "sch", SLOT_FLAG_SCH },
    { "rog", SLOT_FLAG_ROG },
    { "nin", SLOT_FLAG_NIN },
    { "mch", SLOT_FLAG_MCH },
    { "drk", SLOT_FLAG_DRK },
    { "ast", SLOT_FLAG_AST },
    { "sam", SLOT_FLAG_SAM },
    { "rdm", SLOT_FLAG_RDM },
    { "blu", SLOT_FLAG_BLU },
    { "gnb", SLOT_FLAG_GNB },
    { "dnc", SLOT_FLAG_DNC },
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// contentfindercondition.exh_ko.csv

#define CONTENT(NAME, VALUE) constexpr uint32_t CONTENT_##NAME = (VALUE);
CONTENT(E9S , 750)
CONTENT(E10S, 748)
CONTENT(E11S, 752)
CONTENT(E12S, 759)

const std::map<const char*, uint32_t> ffxiv_content_id =
{
    { "e9s" , CONTENT_E9S  }, // 희망의 낙원 에덴: 재생편(영웅) 1
    { "e10s", CONTENT_E10S }, // 희망의 낙원 에덴: 재생편(영웅) 2
    { "e11s", CONTENT_E11S }, // 희망의 낙원 에덴: 재생편(영웅) 3
    { "e12s", CONTENT_E12S }, // 희망의 낙원 에덴: 재생편(영웅) 4

    { "gunnhildr",  760}, // 군힐드 사원
    { "gunnhildrs", 761}, // 군힐드 사원 (영웅)

    { "leviathan", 776 }, // 환 리바이어선
    { "diamond",   782 },// 다이아몬드

    { "bahamut",   280 }, // 절 바하
    { "ultima",    539 }, // 절 테마
    { "alexander", 694 }, // 절 알렉
};

bool is_eden_raid(uint32_t content)
{
    return content == CONTENT_E9S || content == CONTENT_E10S || content == CONTENT_E11S || content == CONTENT_E12S;
}