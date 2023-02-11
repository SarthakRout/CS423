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
}

// Memory
void Memory::printStats(){
    std::cout << "Printing Statistics\n";
}

unsigned long long Memory::handlePkt(unsigned long long addr){
    return 0;
}