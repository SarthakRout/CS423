#ifndef CACHE_H
#define CACHE_H

#include<vector>
#include<cmath>
#include<set>
#include<utility>

#define INCLUSIVE_POLICY    0
#define NINE_POLICY         1
#define EXCLUSIVE_POLICY    2

// class Policy {
//     private:
//         int policy_type;
//     public:
//         Policy(int policy_type) : policy_type(policy_type) { }
// };

int timer;

struct block {
    unsigned long long tag;
    int valid;
};

class LRUCache {
    private:
        int ways;
        int index;
        int block_size;
        std::vector<std::set<std::pair<int, struct block>>> cache;
        int misses;

    public:
        LRUCache(int ways, int block_size, int sz);
        int search(unsigned long long addr);
        void insert(unsigned long long block_address);
        unsigned long long evict(int idx);
        void printStats() const;


};


class Memory {
    private:
        std::vector<LRUCache> cache_layers;
        int policy_id;
    public:
        Memory(std::vector<LRUCache> v, int policy_id);
        // Takes address which is to be loaded and returns the block 
        // which contains the address (block address is returned)
        unsigned long long handlePkt(unsigned long long addr);
        void printStats();
};


#endif