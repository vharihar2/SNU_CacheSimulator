/*

* Copyright (c) 2025, Shiv Nadar University, Delhi NCR, India. All Rights

* Reserved. Permission to use, copy, modify and distribute this software for

* educational, research, and not-for-profit purposes, without fee and without a

* signed license agreement, is hereby granted, provided that this paragraph and

* the following two paragraphs appear in all copies, modifications, and

* distributions.

*

* IN NO EVENT SHALL SHIV NADAR UNIVERSITY BE LIABLE TO ANY PARTY FOR DIRECT,

* INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST

* PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE.

*

* SHIV NADAR UNIVERSITY SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT

* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A

* PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS PROVIDED "AS IS". SHIV

* NADAR UNIVERSITY HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,

* ENHANCEMENTS, OR MODIFICATIONS.

*/

 

/*

Revision History:

Date          By                     Change Notes
13 Nov 2025   Avi Mathur             Completed Cache class implementation
19 Nov 2025   Avi Mathur             Updated cache.cpp
*/

#include "Cache.h"
#include "MainMemory.h"
#include <algorithm>
#include <iostream>
#include <climits>

using namespace std;

static inline uint32_t block_base(uint32_t addr, int B) { return addr & ~(uint32_t(B) - 1u); }
static inline uint32_t block_idx(uint32_t base, int B)  { return base / uint32_t(B); }
static inline uint32_t set_idx(uint32_t bidx, int S)    { return (S > 0) ? (bidx % (uint32_t)S) : 0; }
static inline uint32_t tag_of(uint32_t bidx, int S)     { return (S > 0) ? (bidx / (uint32_t)S) : bidx; }
static inline uint32_t base_from_set_tag(const Cache& C, uint32_t s, uint32_t t) {
    return ((t * (uint32_t)C.sets_n) + s) * (uint32_t)C.block;
}

static int find_line(Cache& C, uint32_t s, uint32_t tag) {
    for (int i = 0; i < C.ways; ++i)
        if (C.sets[s][i].valid && C.sets[s][i].tag == tag) return i;
    return -1;
}

static int victim(Cache& C, uint32_t s) {
    for (int i = 0; i < C.ways; ++i) if (!C.sets[s][i].valid) return i;
    int v = 0; long long best = LLONG_MAX;
    for (int i = 0; i < C.ways; ++i) {
        if (C.sets[s][i].last_use < best) { best = C.sets[s][i].last_use; v = i; }
    }
    return v;
}

void Cache::configure(int c, int b, int a) {
    size = c; block = b; ways = a;
    int lines = size / block;
    sets_n = max(1, lines / ways);
    sets.assign(sets_n, vector<CacheLine>(ways));
    for (auto &s : sets) for (auto &l : s) l = CacheLine(block);
    clk = 0; hits = misses = writebacks = 0;
}

static void lower_writeback(Cache* lower, uint32_t base, const vector<uint8_t>& blk, Cache& self) {
    if (lower) {
        lower->clk++;
        uint32_t bidx = block_idx(base, lower->block);
        uint32_t s    = set_idx(bidx, lower->sets_n);
        uint32_t tag  = tag_of(bidx, lower->sets_n);
        int li = find_line(*lower, s, tag);
        if (li >= 0) {
            auto& ln = lower->sets[s][li];
            ln.last_use = lower->clk;
            ln.data = blk;
            ln.dirty = true;
            return;
        }
        int v = victim(*lower, s);
        if (lower->sets[s][v].valid && lower->sets[s][v].dirty) {
            uint32_t vb = base_from_set_tag(*lower, s, lower->sets[s][v].tag);
            MainMemory::write_block(vb, lower->sets[s][v].data);
            lower->writebacks++;
        }
        auto& ln = lower->sets[s][v];
        ln.valid = true; ln.dirty = true; ln.tag = tag; ln.last_use = lower->clk; ln.data = blk;
    } else {
        MainMemory::write_block(base, blk);
        self.writebacks++;
    }
}

static vector<uint8_t> cache_fetch_block(Cache& C, Cache* lower, uint32_t base, bool* hit_here) {
    C.clk++;
    uint32_t bidx = block_idx(base, C.block);
    uint32_t s    = set_idx(bidx, C.sets_n);
    uint32_t tag  = tag_of(bidx, C.sets_n);

    int li = find_line(C, s, tag);
    if (li >= 0) {
        C.hits++;
        C.sets[s][li].last_use = C.clk;
        if (hit_here) *hit_here = true;
        return C.sets[s][li].data;
    }
    C.misses++;
    if (hit_here) *hit_here = false;

    vector<uint8_t> down;
    if (lower) {
        down = cache_fetch_block(*lower, nullptr, base, nullptr);
    } else {
        MainMemory::read_block(base, C.block, down);
    }

    int v = victim(C, s);
    if (C.sets[s][v].valid && C.sets[s][v].dirty) {
        uint32_t vb = base_from_set_tag(C, s, C.sets[s][v].tag);
        lower_writeback(lower, vb, C.sets[s][v].data, C);
    }
    auto& ln = C.sets[s][v];
    ln.valid = true; ln.dirty = false; ln.tag = tag; ln.last_use = C.clk; ln.data = down;
    return down;
}

uint8_t Cache::read_byte(uint32_t addr, Cache* lower, string &path) {
    uint32_t base = block_base(addr, block);
    size_t off = addr - base;
    bool hit_here = false;
    uint64_t before_hits = lower ? lower->hits : 0;
    auto blk = cache_fetch_block(*this, lower, base, &hit_here);
    if (hit_here) path = name + " HIT";
    else {
        if (lower) path = name + " MISS -> " + (lower->hits > before_hits ? lower->name + " HIT" : lower->name + " MISS -> MEM");
        else path = name + " MISS -> MEM";
    }
    return blk[off];
}

void Cache::write_byte(uint32_t addr, Cache* lower, uint8_t val) {
    uint32_t base = block_base(addr, block);
    size_t off = addr - base;
    bool dummy = false; (void)dummy;
    auto blk = cache_fetch_block(*this, lower, base, &dummy);
    uint32_t bidx = block_idx(base, block);
    uint32_t s = set_idx(bidx, sets_n);
    uint32_t tag = tag_of(bidx, sets_n);
    int li = find_line(*this, s, tag);
    auto& ln = sets[s][li];
    ln.data[off] = val;
    ln.dirty = true;
}

void Cache::debug_print() const {
    cout << "=== " << name << " ===  (legend: t=tag, *=dirty)  (ways=" << ways << ", sets=0.." << (sets_n>0?sets_n-1:0) << ")\n";
    bool any_shown = false;
    for (size_t si = 0; si < sets.size(); ++si) {
        bool set_has_valid = false;
        for (size_t w = 0; w < sets[si].size(); ++w) {
            if (sets[si][w].valid) { set_has_valid = true; break; }
        }
        if (!set_has_valid) continue; // omit empty sets
        any_shown = true;
        cout << "set " << si << " of " << (sets_n>0?sets_n-1:0) << ": ";
        for (size_t w = 0; w < sets[si].size(); ++w) {
            const auto &ln = sets[si][w];
            if (!ln.valid) cout << "[.--] ";
            else cout << "[t=" << ln.tag << (ln.dirty? "*":"") << "] ";
        }
        cout << "\n";
    }
    if (!any_shown) cout << "(no valid lines in any set)\n";
    cout << "hits=" << hits << " misses=" << misses << " writebacks=" << writebacks << "\n";
}

double Cache::hit_percent() const {
    uint64_t acc = hits + misses;
    return acc ? 100.0 * double(hits)/double(acc) : 0.0;
}
