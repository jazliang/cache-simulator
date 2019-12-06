//
// Created by Jason on 2019-12-05.
//

#include "util.h"
#include <ctime>
#include <limits>
#include <cmath>

#include <iostream>


CacheLine::CacheLine() : tag(0), timestamp(0) {}

CacheLine::CacheLine(uint32_t tag, uint32_t timestamp)
    : tag(tag), timestamp(timestamp) {}

CacheSet::CacheSet() : associativity(0), eviction(false), evictedTag(0) {}

CacheSet::CacheSet(uint32_t associativity) : associativity(associativity), eviction(false), evictedTag(0) {
    this->cachelines = std::unordered_map<uint32_t, CacheLine>();
}

void CacheSet::evictLRU() {
    uint32_t lruTime = std::numeric_limits<uint32_t>::max();
    uint32_t lruTag = 0;

//    if (this->cachelines.size() != 8) {
//        for (auto line : this->cachelines) {
//            std::cout << line.first << ' ' << line.second.timestamp << ", ";
//        }
//    }

    // Find the least recently used cache line.
    for (auto line : this->cachelines) {
        if (line.second.timestamp < lruTime) {
            lruTime = line.second.timestamp;
            lruTag = line.first;
        }
    }

    // Remove this cache line.
    this->cachelines.erase(lruTag);

    this->eviction = true;
    this->evictedTag = lruTag;
}

void CacheSet::invalidate(uint32_t tag) {
    // If this tag is in the set
    if (this->cachelines.find(tag) != this->cachelines.end()) {
        this->cachelines.erase(tag);
//        std::cout << tag << ", ";
    }
}

bool CacheSet::contains(uint32_t tag) {
    return this->cachelines.find(tag) != this->cachelines.end();
}

void CacheSet::update(uint32_t tag, uint32_t timestamp) {
    this->eviction = false;

    // This tag is not in the set.
    if (this->cachelines.find(tag) == this->cachelines.end()) {
        // Is the set full?
        if (this->cachelines.size() >= this->associativity) {
            evictLRU();
        }

        // Put this tag in the set.
        this->cachelines.insert(std::make_pair(tag, CacheLine(tag, timestamp)));
    }
    else {
        // Update the timestamp.
        this->cachelines[tag].timestamp = timestamp;
    }
}

Cache::Cache(uint32_t nSets, uint32_t associativity, uint32_t blockSize, uint32_t inclusive)
    : nSets(nSets), associativity(associativity), blockSize(blockSize), inclusive(inclusive),
    eviction(false), evictedAddr(0) {
    this->cacheSets = std::vector<CacheSet>(this->nSets, CacheSet(associativity));
}

uint32_t Cache::getTag(uint32_t addr) {
    return addr >> (uint32_t) std::log2(this->blockSize);
}

uint32_t Cache::getIdx(uint32_t addr) {
    return this->getTag(addr) & (this->nSets - 1);
}

void Cache::invalidate(uint32_t addr) {
    auto idx = this->getIdx(addr);
    auto tag = this->getTag(addr);

    this->cacheSets[idx].invalidate(tag);
}

bool Cache::contains(uint32_t addr) {
    auto idx = this->getIdx(addr);
    auto tag = this->getTag(addr);

    return this->cacheSets[idx].contains(tag);
}

void Cache::update(uint32_t addr, uint32_t timestamp) {
    auto idx = this->getIdx(addr);
    auto tag = this->getTag(addr);

    this->cacheSets[idx].update(tag, timestamp);

    // If eviction happens..
    if (this->cacheSets[idx].eviction) {
        this->eviction = true;
        this->evictedAddr = this->cacheSets[idx].evictedTag << (uint32_t) std::log2(this->blockSize);
    } else {
        this->eviction = false;
    }
}


