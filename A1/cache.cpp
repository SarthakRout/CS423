#include "cache.h"
#include <cassert>
#include <iostream>

#define X_MASK(p) (unsigned long long)((1<<(p)) - 1)


// LRUCache
LRUCache::LRUCache(int ways, int block_size, int sz){
    this->ways = ways;
    this->offset = ceil(log2(block_size));
    int sets =  sz / ( ways * block_size);
    assert(sets >= 1);
    this->index = ceil(log2(sets));
    this->misses = 0;
    this->hits = 0;
    this->mem = nullptr;

    this->initialise();
}

void LRUCache::initialise(){
    this->cache.clear();
    for(int i = 0; i<(1<<(this->index)); i++){
        std::multiset<std::pair<int, Block>> temp;
        for(int j = 0; j<this->ways; j++){
            temp.insert({0, {0ULL, 0}});
        }
        this->cache.push_back(temp);
    }
}

inline unsigned long long LRUCache::getBlockAddr(unsigned long long addr) const {
    return addr >> (this->offset);
}

inline unsigned long long LRUCache::getTag(unsigned long long block_addr) const {
    return block_addr >> (this->index);
}

inline unsigned long long LRUCache::getAddr(unsigned long long tag, int indexBits) const{
    return ((tag << this->index) + indexBits) << this->offset;
}

bool LRUCache::search(unsigned long long addr) {
    unsigned long long block_addr = getBlockAddr(addr);
    int indexBits = block_addr & X_MASK(this->index);
    for(auto& block: this->cache[indexBits]){
        if(block.second.valid && block.second.tag == getTag(block_addr)){
            this->hits++;
            this->cache[indexBits].erase(this->cache[indexBits].lower_bound(block));
            this->cache[indexBits].insert({this->mem->timer, {this->getTag(block_addr), 1}});
            // Hits in this layer of cache
            return true;
        }
    }
    // Missed in this layer of cache
    this->misses++;
    return false;
}

unsigned long long LRUCache::insert(unsigned long long addr, bool& evicted){
    unsigned long long victim = 0;
    unsigned long long block_addr = getBlockAddr(addr);
    int indexBits = block_addr & X_MASK(this->index);

    // Pointer to last block in set which will be invalid or LRU replacement victim
    auto block = this->cache[indexBits].begin();
    if(block->second.valid == false){
        evicted = false;
        this->cache[indexBits].erase(this->cache[indexBits].lower_bound(*block));
        this->cache[indexBits].insert({this->mem->timer, {this->getTag(block_addr), 1}});
    }
    else{
        // Evicting this block
        evicted = true;
        victim = this->getAddr(block->second.tag, indexBits);
        this->cache[indexBits].erase(this->cache[indexBits].lower_bound(*block));
        this->cache[indexBits].insert({this->mem->timer, {this->getTag(block_addr), 1}});
    }
    return victim;
}

// Invalidates block with given address if found in the cache
void LRUCache::invalidate(unsigned long long addr){
    unsigned long long block_addr = getBlockAddr(addr);
    int indexBits = block_addr & X_MASK(this->index);
    auto tag = getTag(block_addr);
    for(auto block: this->cache[indexBits]){
        if(block.second.tag == tag && block.second.valid){
            this->cache[indexBits].erase(this->cache[indexBits].lower_bound(block));
            this->cache[indexBits].insert({0, {0ULL, 0}});
            break;
        }
    }
}

void LRUCache::setMem(Memory* ptr){
    this->mem = ptr;
}

int LRUCache::getMisses() const{
    return this->misses;
}

int LRUCache::getHits() const{
    return this->hits;
}

void LRUCache::reset(){
    this->misses = 0;
    this->hits = 0;
    this->initialise();
}

// Memory
Memory::Memory(std::vector<LRUCache> v, int policy_id){
    this->cache_layers = v;
    for(auto& cache: this->cache_layers){
        cache.setMem(this);
    }
    this->policy_id = policy_id;
    this->timer = 0;
}

void Memory::handlePkt(unsigned long long addr){
    this->timer++;
    int i = 0;
    for(i = 0; i<this->cache_layers.size(); i++){
        if(this->cache_layers[i].search(addr)){
            // Hit in this cache layer
            break;
        }
    }
    switch(policy_id){
        case INCLUSIVE_POLICY:
            { 
                this->implInclusivePolicy(addr, i);
                break;
            }
        case NINE_POLICY:
            { 
                this->implNINEPolicy(addr, i);
                break;
            }
        case EXCLUSIVE_POLICY:
            { 
                this->implExclusivePolicy(addr, i);
                break;
            }
        default:
            std::cout << "Policy not implemented\n";
            exit(0);

    }

}

void Memory::implInclusivePolicy(unsigned long long addr, int hit_layer){
    
    for(int j = hit_layer  - 1; j>=0; j--){
        bool evicted;
        unsigned long long victim = this->cache_layers[j].insert(addr, evicted);
        if(evicted){
            for(int k = j-1; k>=0; k--){
                this->cache_layers[k].invalidate(victim);
            }
        }
    }
}

void Memory::implNINEPolicy(unsigned long long addr, int hit_layer){
    for(int j = hit_layer - 1; j>=0; j--){
        bool evicted;
        unsigned long long victim = this->cache_layers[j].insert(addr, evicted);
    }
}

void Memory::implExclusivePolicy(unsigned long long addr, int hit_layer){
    if(hit_layer != 0){
        if(hit_layer < this->cache_layers.size()){
            this->cache_layers[hit_layer].invalidate(addr);
        }
        bool evicted;
        unsigned long long victim = this->cache_layers[0].insert(addr, evicted);
        int j = 1;
        while(evicted && j < this->cache_layers.size()){
            victim = this->cache_layers[j].insert(victim, evicted);
            j++;
        }
    }
}


void Memory::printStats(){
    std::cout << "Printing Statistics\n";
    int layer_id = 2;
    for(const auto& cache: this->cache_layers){
        std::cout << "Layer " << layer_id << " Hits and Misses:\n";
        long long hits = cache.getHits(), misses = cache.getMisses();
        long long per = 100.0 * hits / ( hits + misses);
        std::cout << hits << "(" << (per) << "%)" << "\t" << misses << "(" << (100 - (per))<< "%)" << "\n";
        layer_id++;
    }
}

void Memory::reset(int policy_id){
    this->policy_id = policy_id;
    for(auto& cache: this->cache_layers){
        cache.reset();
    }
    this->timer = 0;
}