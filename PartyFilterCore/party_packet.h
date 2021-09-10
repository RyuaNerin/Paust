#pragma once

#include <cstdint>

typedef struct
{
    int8_t header1[4];
    uint32_t id;

    int8_t header2[4];

    uint32_t ContentIdLower;
    uint16_t unknownShort1;
    uint16_t unknownShort2;

    uint8_t header3[5];

    uint8_t Category;

    uint8_t header4[2];

    uint16_t Duty;
    uint8_t DutyType;

    uint8_t header5[11];

    uint16_t World;

    uint8_t header6[8];

    uint8_t Objective;
    uint8_t BeginnersWelcome;
    uint8_t Conditions;
    uint8_t DutyFinderSettings;
    uint8_t LootRules;

    uint8_t header7[3];

    uint32_t lastPatchHotfixTimestamp; // last time the servers were restarted?
    uint16_t SecondsRemaining;

    uint8_t header8[6]; // 00 00 01 00 00 00 in every pf I've examined

    uint16_t MinimumItemLevel;
    uint16_t HomeWorld;
    uint16_t CurrentWorld;

    uint8_t header9;

    uint8_t NumSlots;

    uint8_t header10[2];

    uint8_t SearchArea;

    uint8_t header11;

    uint8_t NumParties;

    uint8_t header12[3]; // 00 00 00 always. maybe numParties is a u32?

    uint32_t Slots[8];

    uint8_t JobsPresent[8];

    // Note that ByValTStr will not work here because the strings are UTF-8 and there's only a CharSet for UTF-16 in C#.
    uint8_t Name[32];

    uint8_t Description[192];
} party_finder_packet_listing;

typedef struct
{
    int32_t batchNumber;
    char padding[8];
    party_finder_packet_listing listing[4];
} party_finder_packet;
