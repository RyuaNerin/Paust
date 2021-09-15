#pragma once

#include <string>

#include "ffxiv_data.h"

class javascript_engine
{
public:
    javascript_engine();
    ~javascript_engine();

    bool eval(const party_finder_packet_item* item, const std::string& script, bool* succeed, std::string* error_message);

private:
    void* ctx;
};
