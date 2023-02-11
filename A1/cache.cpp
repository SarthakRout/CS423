#include "cache.h"
#include <cassert>
#include <iostream>


// LRUCache
LRUCache::LRUCache(int ways, int block_size, int sz){
    this->ways = ways;
    this->block_size = block_size;
    int sets =  sz / ( ways * block_size);
    assert(sets >= 1);
    this->index = ceil(log2(sets));
    this->misses = 0;

    for(int i = 0; i<(1<<index); i++){
        std::set<std::pair<int, struct block>> temp;
        for(int j = 0; j<ways; j++){
            temp.insert({0, {0ULL, 0}});
        }
        this->cache.push_back(temp);
    }
}
void LRUCache::printStats() const{
    std::cout << this->misses << "\n";
}

// Memory

Memory::Memory(std::vector<LRUCache> v, int policy_id){
    this->cache_layers = v;
    this->policy_id = policy_id;
    timer = 0;
}
void Memory::printStats(){
    std::cout << "Printing Statistics\n";
    int layer_id = 2;
    for(const auto& cache: this->cache_layers){
        std::cout << "Layer " << layer_id << " Misses:\n";
        cache.printStats();
        layer_id++;
    }
}

unsigned long long Memory::handlePkt(unsigned long long addr){
    timer++;
    return 0;
}