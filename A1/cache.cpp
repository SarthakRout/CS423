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
}
void LRUCache::printStats() const{
    std::cout << this->misses << "\n";
}

// Memory
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
    
    return 0;
}