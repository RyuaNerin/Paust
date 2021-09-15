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

    PADDING(22);

    uint8_t welcome_beginner;

    PADDING(1);

    uint8_t settings;
    uint8_t item_rule;

    PADDING(7);

    uint16_t remaining_seconds;

    PADDING(6);

    uint16_t min_item_level;

    PADDING(5);

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

extern const std::map<const char*, uint32_t> ffxiv_job_id;
extern const std::map<const char*, uint32_t> ffxiv_slot_flag;
extern const std::map<const char*, uint32_t> ffxiv_content_id;
