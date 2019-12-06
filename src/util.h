//
// Created by Jason on 2019-12-05.
//

#ifndef CACHEPROJECT240A_UTIL_H
#define CACHEPROJECT240A_UTIL_H


#include <ctime>
#include <unordered_map>
#include <vector>


struct CacheLine {
    const uint32_t tag;
//    bool valid;
    uint32_t timestamp;

    CacheLine();
    CacheLine(uint32_t tag, uint32_t timestamp);
};


class CacheSet {
private:
    const uint32_t associativity;
    std::unordered_map<uint32_t, CacheLine> cachelines;

public:
    // Used to record whether this is an eviction during the last access.
    bool eviction;
    uint32_t evictedTag;

    CacheSet();
    CacheSet(uint32_t associativity);

    /**
     * This function will
     * - Update the timestamp of the given tag.
     * - Return true if it is a hit or false if miss.
     */
    bool contains(uint32_t tag);

    /**
     * Update the cache set.
     */
    void update(uint32_t tag, uint32_t timestamp);

    /**
     * Evict the least recently used cache line.
     */
    void evictLRU();

    /**
     * Remove the element of the given tag from the set.
     */
    void invalidate(uint32_t tag);
};


class Cache {
private:
    std::vector<CacheSet> cacheSets;

    uint32_t getTag(uint32_t addr);

    uint32_t getIdx(uint32_t addr);

public:
    const uint32_t nSets;
    const uint32_t associativity;
    const uint32_t blockSize;
    const uint32_t inclusive;

    // Used to record whether this is an eviction during the last access.
    bool eviction;
    uint32_t evictedAddr;

    Cache(uint32_t nSets, uint32_t associativity, uint32_t blockSize, uint32_t inclusive);

    /**
     * This function is used if:
     * - this cache is L1I or L1D, and
     * - the L2 cache is set to inclusive.
     *
     * Note: If a line is evicted from the L2 then an invalidation must be sent
     * to both L1D as well as L1I.
     * @param addr
     */
    void invalidate(uint32_t addr);

    /**
     * Simulate the process of accessing the given addr in this cache.
     * @return true if it is a hit or false if miss
     */
    bool contains(uint32_t addr);

    /**
     * Update the cache
     */
    void update(uint32_t addr, uint32_t timestamp);
};


#endif //CACHEPROJECT240A_UTIL_H
