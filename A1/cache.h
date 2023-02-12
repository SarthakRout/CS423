#ifndef CACHE_H
#define CACHE_H

#include<vector>
#include<cmath>
#include<set>
#include<utility>

#define INCLUSIVE_POLICY    0
#define NINE_POLICY         1
#define EXCLUSIVE_POLICY    2

class block {
    public:
    unsigned long long tag;
    int valid;
    block(unsigned long long tag, int valid) : tag(tag), valid(valid) {}
    bool operator<(const block& rhs) const{
        return this->tag < rhs.tag;
    }
};

class Memory;

class LRUCache {
    private:
        int ways;
        int index;
        int offset;
        std::vector<std::multiset<std::pair<int, block>>> cache;
        Memory* mem;
        int misses;

    public:
        LRUCache(int ways, int block_size, int sz);
        inline unsigned long long getBlockAddr(unsigned long long addr) const;
        inline unsigned long long getTag(unsigned long long block_addr) const;
        inline unsigned long long getAddr(unsigned long long tag, int indexBits) const;
        bool search(unsigned long long addr);
        unsigned long long insert(unsigned long long addr, bool& evicted);
        void invalidate(unsigned long long addr);
        int getMisses() const;
        void setMem(Memory* mem);


};


class Memory {
    private:
        std::vector<LRUCache> cache_layers;
        int policy_id;
        int timer;
        void implInclusivePolicy(unsigned long long addr, int hit_layer);
        void implNINEPolicy(unsigned long long addr, int hit_layer);
        void implExclusivePolicy(unsigned long long addr, int hit_layer);

    public:
        friend unsigned long long LRUCache::insert(unsigned long long addr, bool& evicted);
        friend bool LRUCache::search(unsigned long long addr);
        Memory(std::vector<LRUCache> v, int policy_id);
        // Takes address which is to be loaded and returns the block 
        // which contains the address (block address is returned)
        void handlePkt(unsigned long long addr);
        void printStats();
};


#endif