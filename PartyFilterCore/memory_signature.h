#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

class memory_signature
{
private:
    void gen_bch();

    const uint8_t* find_signature(const uint8_t* baseAddr, const size_t baseSize) const;

    size_t size = 0;
    std::vector<bool> mask;
    std::vector<uint8_t> value;
    std::shared_ptr<size_t[]> bch = std::make_shared<size_t[]>(256); // Bad Character Heuristic

public:
    memory_signature(const std::string& str);
    void* scan() const;
};