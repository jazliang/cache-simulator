//========================================================//
//  cache.c                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.h"
#include "util.h"

#include <limits>
#include <iostream>

//
// TODO:Student Information
//
const char *studentName = "";
const char *studentID   = "";
const char *email       = "";

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;     // Number of sets in the I$
uint32_t icacheAssoc;    // Associativity of the I$
uint32_t icacheHitTime;  // Hit Time of the I$

uint32_t dcacheSets;     // Number of sets in the D$
uint32_t dcacheAssoc;    // Associativity of the D$
uint32_t dcacheHitTime;  // Hit Time of the D$

uint32_t l2cacheSets;    // Number of sets in the L2$
uint32_t l2cacheAssoc;   // Associativity of the L2$
uint32_t l2cacheHitTime; // Hit Time of the L2$
uint32_t inclusive;      // Indicates if the L2 is inclusive

uint32_t blocksize;      // Block/Line size
uint32_t memspeed;       // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;       // I$ references
uint64_t icacheMisses;     // I$ misses
uint64_t icachePenalties;  // I$ penalties

uint64_t dcacheRefs;       // D$ references
uint64_t dcacheMisses;     // D$ misses
uint64_t dcachePenalties;  // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//

Cache* l1i;
Cache* l1d;
Cache* l2;

uint32_t timestamp;

//------------------------------------//
//          Cache Functions           //
//------------------------------------//

// Initialize the Cache Hierarchy
//
void init_cache() {
    // Initialize cache stats
    icacheRefs        = 0;
    icacheMisses      = 0;
    icachePenalties   = 0;
    dcacheRefs        = 0;
    dcacheMisses      = 0;
    dcachePenalties   = 0;
    l2cacheRefs       = 0;
    l2cacheMisses     = 0;
    l2cachePenalties  = 0;

    timestamp = 0;

    l1i = new Cache(icacheSets, icacheAssoc, blocksize, inclusive);
    l1d = new Cache(dcacheSets, dcacheAssoc, blocksize, inclusive);
    l2 = new Cache(l2cacheSets, l2cacheAssoc, blocksize, inclusive);
}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t icache_access(uint32_t addr) {
    timestamp++;

    // If uninstantiated, use the next level.
    if (l1i->nSets == 0) {
        return l2cache_access(addr);
    }

    icacheRefs++;

    if (l1i->contains(addr)) {
        // Hit
        l1i->update(addr, timestamp);
        return icacheHitTime;
    }
    else {
        // Miss
        icacheMisses++;

        auto latency = l2cache_access(addr);
        l1i->update(addr, timestamp);

        icachePenalties += latency;
        return latency + icacheHitTime;
    }
}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t dcache_access(uint32_t addr) {
    timestamp++;

    // If uninstantiated, use the next level.
    if (l1d->nSets == 0) {
        return l2cache_access(addr);
    }

    dcacheRefs++;

    if (l1d->contains(addr)) {
        // Hit
        l1d->update(addr, timestamp);
        return dcacheHitTime;
    }
    else {
        // Miss
        dcacheMisses++;

        auto latency = l2cache_access(addr);
        l1d->update(addr, timestamp);

        dcachePenalties += latency;
        return latency + dcacheHitTime;
    }
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t l2cache_access(uint32_t addr) {
    timestamp++;

    // If uninstantiated, use the next level.
    if (l2->nSets == 0) {
        return memspeed;
    }

    l2cacheRefs++;

    if (l2->contains(addr)) {
        // Hit
        l2->update(addr, timestamp);
        return l2cacheHitTime;
    }
    else {
        // Miss
        l2cacheMisses++;

        l2->update(addr, timestamp);

        if (inclusive && l2->eviction) {
            l1d->invalidate(l2->evictedAddr);
            l1i->invalidate(l2->evictedAddr);
        }

        l2cachePenalties += memspeed;
        return memspeed + l2cacheHitTime;
    }
}

void freeCaches() {
    delete l1d;
    delete l1i;
    delete l2;
}
