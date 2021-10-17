#pragma once

#include <array>
#include <cstdint>
#include <map>

#pragma pack (1)

#define PADDING___(SIZE, LINE)  ___pad___sz_##SIZE
#define PADDING__(SIZE, LINE)   std::array<uint8_t, SIZE> PADDING___(SIZE, LINE)__line_##LINE
#define PADDING_(SIZE, LINE)    PADDING__(SIZE, LINE)
#define PADDING(SIZE)           PADDING_(SIZE, __LINE__)

typedef struct
{
    PADDING(4);

    uint32_t party_id;

    PADDING(17);

    uint8_t content_category;

    PADDING(2);

    uint16_t content_id;
    uint8_t is_roulette_eq_1;

    PADDING(21);

    uint8_t objective;
    uint8_t welcome_beginner;
    uint8_t conditions;
    uint8_t settings;
    uint8_t item_rule;

    PADDING(7);

    uint16_t remaining_seconds;

    PADDING(6);

    uint16_t min_item_level;

    PADDING(2);

    uint16_t server_current;

    PADDING(1);

    uint8_t slot_count;

    PADDING(2);

    uint8_t settings2;

    PADDING(1);

    uint8_t party_count;

    PADDING(3);

    std::array<uint32_t, 8> slot_available_flag;
    std::array<uint8_t, 8> slot_user_job;
    std::array<uint8_t, 32> owner_name; // utf-8
    std::array<uint8_t, 192> desc; // utf-8
} party_finder_packet_item;

typedef struct
{
    PADDING(16); // packet header...
    PADDING(12);
    std::array<party_finder_packet_item, 4> items;
} party_finder_packet;

#undef PADDING
#undef PADDING_
#undef PADDING__
#undef PADDING___

#pragma pack ()

extern const party_finder_packet_item* const party_finder_packet_item_sample;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef uint8_t  job_id;
typedef uint32_t slot_flag;
typedef uint32_t content_id;

extern const std::map<const char*, job_id> ffxiv_job_id;
extern const std::map<const char*, slot_flag> ffxiv_slot_flag;
extern const std::map<const char*, content_id> ffxiv_content_id;

extern const std::map<slot_flag, job_id> ffxiv_slot_2_job;

////////////////////////////////////////////////////////////////////////////////////////////////////

#define JOB_ID(NAME, VALUE) constexpr job_id JOB_ID_##NAME = (VALUE);
JOB_ID(ADV, 0) // 모험가
JOB_ID(GLA, 1) // 검술사
JOB_ID(PGL, 2) // 격투사
JOB_ID(MRD, 3) // 도끼술사
JOB_ID(LNC, 4) // 창술사
JOB_ID(ARC, 5) // 궁술사
JOB_ID(CNJ, 6) // 환술사
JOB_ID(THM, 7) // 주술사
JOB_ID(CRP, 8) // 목수
JOB_ID(BSM, 9) // 대장장이
JOB_ID(ARM, 10) // 갑주제작사
JOB_ID(GSM, 11) // 보석공예가
JOB_ID(LTW, 12) // 가죽공예가
JOB_ID(WVR, 13) // 재봉사
JOB_ID(ALC, 14) // 연금술사
JOB_ID(CUL, 15) // 요리사
JOB_ID(MIN, 16) // 광부
JOB_ID(BTN, 17) // 원예가
JOB_ID(FSH, 18) // 어부
JOB_ID(PLD, 19) // 나이트
JOB_ID(MNK, 20) // 몽크
JOB_ID(WAR, 21) // 전사
JOB_ID(DRG, 22) // 용기사
JOB_ID(BRD, 23) // 음유시인
JOB_ID(WHM, 24) // 백마도사
JOB_ID(BLM, 25) // 흑마도사
JOB_ID(ACN, 26) // 비술사
JOB_ID(SMN, 27) // 소환사
JOB_ID(SCH, 28) // 학자
JOB_ID(ROG, 29) // 쌍검사
JOB_ID(NIN, 30) // 닌자
JOB_ID(MCH, 31) // 기공사
JOB_ID(DRK, 32) // 암흑기사
JOB_ID(AST, 33) // 점성술사
JOB_ID(SAM, 34) // 사무라이
JOB_ID(RDM, 35) // 적마도사
JOB_ID(BLU, 36) // 청마도사
JOB_ID(GNB, 37) // 건브레이커
JOB_ID(DNC, 38) // 무도가
#undef JOB_ID

////////////////////////////////////////////////////////////////////////////////////////////////////

#define JOB_FLAG(NAME, VALUE) constexpr slot_flag SLOT_FLAG_##NAME = (VALUE);
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
#undef JOB_FLAG

////////////////////////////////////////////////////////////////////////////////////////////////////
// contentfindercondition.exh_ko.csv

#define CONTENT_ID(NAME, VALUE) constexpr content_id CONTENT_ID_##NAME = (VALUE);
CONTENT_ID(E9S, 750)
CONTENT_ID(E10S, 748)
CONTENT_ID(E11S, 752)
CONTENT_ID(E12S, 759)

CONTENT_ID(GUNNHILDR, 760)
CONTENT_ID(GUNNHILDRS, 761)

CONTENT_ID(LEVIATHAN, 776)
CONTENT_ID(DIAMOND, 782)

CONTENT_ID(BAHAMUT, 280)
CONTENT_ID(ULTIMA, 539)
CONTENT_ID(ALEXANDER, 694)
#undef CONTENT_ID

////////////////////////////////////////////////////////////////////////////////////////////////////

bool is_eden_raid(content_id content);
