#ifndef CACHE_H
#define CACHE_H

#include<vector>
#include<cmath>

#define INCLUSIVE_POLICY    0
#define NINE_POLICY         1
#define EXCLUSIVE_POLICY    2

class Policy {
    private:
        int policy_type;
    public:
        Policy(int policy_type) : policy_type(policy_type) { }
};

class LRUCache {
    private:
        int ways;
        int index;
        int block_size;
    public:
        LRUCache(int ways, int block_size, int sz);
};


class Memory {
    private:
        std::vector<LRUCache> cache_layers;
    public:
        Memory(std::vector<LRUCache> v) : cache_layers(v) {}
        // Takes address which is to be loaded and returns the block 
        // which contains the address (block address is returned)
        unsigned long long handlePkt(unsigned long long addr);
        void printStats();
};


#endif