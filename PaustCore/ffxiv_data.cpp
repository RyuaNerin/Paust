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

const std::map<const char*, job_id> ffxiv_job_id =
{
    { "adv", JOB_ID_ADV },
    { "gla", JOB_ID_GLA },
    { "pgl", JOB_ID_PGL },
    { "mrd", JOB_ID_MRD },
    { "lnc", JOB_ID_LNC },
    { "arc", JOB_ID_ARC },
    { "cnj", JOB_ID_CNJ },
    { "thm", JOB_ID_THM },
    { "crp", JOB_ID_CRP },
    { "bsm", JOB_ID_BSM },
    { "arm", JOB_ID_ARM },
    { "gsm", JOB_ID_GSM },
    { "ltw", JOB_ID_LTW },
    { "wvr", JOB_ID_WVR },
    { "alc", JOB_ID_ALC },
    { "cul", JOB_ID_CUL },
    { "min", JOB_ID_MIN },
    { "btn", JOB_ID_BTN },
    { "fsh", JOB_ID_FSH },
    { "pld", JOB_ID_PLD },
    { "mnk", JOB_ID_MNK },
    { "war", JOB_ID_WAR },
    { "drg", JOB_ID_DRG },
    { "brd", JOB_ID_BRD },
    { "whm", JOB_ID_WHM },
    { "blm", JOB_ID_BLM },
    { "acn", JOB_ID_ACN },
    { "smn", JOB_ID_SMN },
    { "sch", JOB_ID_SCH },
    { "rog", JOB_ID_ROG },
    { "nin", JOB_ID_NIN },
    { "mch", JOB_ID_MCH },
    { "drk", JOB_ID_DRK },
    { "ast", JOB_ID_AST },
    { "sam", JOB_ID_SAM },
    { "rdm", JOB_ID_RDM },
    { "blu", JOB_ID_BLU },
    { "gnb", JOB_ID_GNB },
    { "dnc", JOB_ID_DNC },
};

////////////////////////////////////////////////////////////////////////////////////////////////////

const std::map<const char*, slot_flag> ffxiv_slot_flag =
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

const std::map<slot_flag, job_id> ffxiv_slot_2_job = {
    { SLOT_FLAG_GLD, JOB_ID_GLA },
    { SLOT_FLAG_PGL, JOB_ID_PGL },
    { SLOT_FLAG_MRD, JOB_ID_MRD },
    { SLOT_FLAG_LNC, JOB_ID_LNC },
    { SLOT_FLAG_ARC, JOB_ID_ARC },
    { SLOT_FLAG_CNJ, JOB_ID_CNJ },
    { SLOT_FLAG_THM, JOB_ID_THM },
    { SLOT_FLAG_PLD, JOB_ID_PLD },
    { SLOT_FLAG_MNK, JOB_ID_MNK },
    { SLOT_FLAG_WAR, JOB_ID_WAR },
    { SLOT_FLAG_DRG, JOB_ID_DRG },
    { SLOT_FLAG_BRD, JOB_ID_BRD },
    { SLOT_FLAG_WHM, JOB_ID_WHM },
    { SLOT_FLAG_BLM, JOB_ID_BLM },
    { SLOT_FLAG_ACN, JOB_ID_ACN },
    { SLOT_FLAG_SMN, JOB_ID_SMN },
    { SLOT_FLAG_SCH, JOB_ID_SCH },
    { SLOT_FLAG_ROG, JOB_ID_ROG },
    { SLOT_FLAG_NIN, JOB_ID_NIN },
    { SLOT_FLAG_MCH, JOB_ID_MCH },
    { SLOT_FLAG_DRK, JOB_ID_DRK },
    { SLOT_FLAG_AST, JOB_ID_AST },
    { SLOT_FLAG_SAM, JOB_ID_SAM },
    { SLOT_FLAG_RDM, JOB_ID_RDM },
    { SLOT_FLAG_BLU, JOB_ID_BLU },
    { SLOT_FLAG_GNB, JOB_ID_GNB },
    { SLOT_FLAG_DNC, JOB_ID_DNC },
};


////////////////////////////////////////////////////////////////////////////////////////////////////

const std::map<const char*, content_id> ffxiv_content_id =
{
    { "e9s" , CONTENT_ID_E9S  }, // 희망의 낙원 에덴: 재생편(영웅) 1
    { "e10s", CONTENT_ID_E10S }, // 희망의 낙원 에덴: 재생편(영웅) 2
    { "e11s", CONTENT_ID_E11S }, // 희망의 낙원 에덴: 재생편(영웅) 3
    { "e12s", CONTENT_ID_E12S }, // 희망의 낙원 에덴: 재생편(영웅) 4

    { "gunnhildr",  CONTENT_ID_GUNNHILDR  }, // 군힐드 사원
    { "gunnhildrs", CONTENT_ID_GUNNHILDRS }, // 군힐드 사원 (영웅)

    { "leviathan", CONTENT_ID_LEVIATHAN }, // 환 리바이어선
    { "diamond",   CONTENT_ID_DIAMOND   },// 다이아몬드

    { "bahamut",   CONTENT_ID_BAHAMUT   }, // 절 바하
    { "ultima",    CONTENT_ID_ULTIMA    }, // 절 테마
    { "alexander", CONTENT_ID_ALEXANDER }, // 절 알렉
};

bool is_eden_raid(uint32_t content)
{
    return content == CONTENT_ID_E9S || content == CONTENT_ID_E10S || content == CONTENT_ID_E11S || content == CONTENT_ID_E12S;
}
