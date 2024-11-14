#ifndef CACHE_TEST_H
#define CACHE_TEST_H

#include <vector>
#include <cstdint>
#include <iostream>
#include "BusInterconnect.h"

class BusInterconnect;

class CacheBlock {
public:
    bool valid;
    bool dirty;
    uint64_t tag;
    std::vector<uint64_t> data; // 4 daatos de 64 (32 bytes total)
    MESIState mesiState;

    CacheBlock() = default;
    CacheBlock(MESIState initialState, bool initailValid, uint64_t initialTag, const std::vector<uint64_t>& initialData) : state(initialState), valid(initailValid), tag(initialTag), data(initialData) {}
};

class Cache {
public:
    Cache(int numBlocks = 8)    // Constructor de 8 bloques
    int getCacheMisses() const;
    int getInvalidations() const;

    future<uint64_t> read(uint64_t address);
    void write(uint64_t address, uint64_t data);
    void invalidate(uint64_t address);

private:
    int peId;
    BusInterconnect& bus;
    std::vector<CacheBlock> blocks;
    int cacheMisses = 0;
    int invalidations = 0;

    uint64_t getBlockIndex(uint64_t address) const;
    uint64_t getTag(uint64_t address) const;
    int getOffset(uint64_t address) const;
};
#endif // CACHE_TEST_H