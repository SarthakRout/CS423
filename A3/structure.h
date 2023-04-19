#include<bits/stdc++.h>

using namespace std;
#define INVALID 0
#define SHARED 1
#define MODIFIED 2
#define EXCLUSIVE 3

#define MSG_GET 0
#define MSG_GETX 1
#define MSG_UPGR 2
#define MSG_ACK 3
#define MSG_NACK 4
#define MSG_INVAL 5
#define MSG_SWB 6


uint64_t Cycle = 0;


typedef unsigned long long uint64 ;

struct mem_access{
    uint64 global_ctr;
    uint64 addr;
    uint64 is_write;

    mem_access(){
        global_ctr = 0;
        addr = 0;
        is_write = 0;
    }
};

struct msg{
    uint64 msg_name;
    uint64 sender;
    uint64 addr;
    uint64 receiver;
};


uint64 l1_size = 32*1024;
uint64 l1_block_size=64;
uint64 l1_assoc = 8;
uint64 l1_sets = l1_size/(l1_block_size*l1_assoc);

struct l1_cache_block{
    bool is_valid;
    uint64 tag;
    uint64 states;
    uint64 lru;
    uint64 is_dirty;
    l1_cache_block(){
        is_valid = false;
        tag = 0;
        states = INVALID;
        lru = 0;
        is_dirty = 0;
    }
};


struct l1_cache_block_cmp {
    bool operator() (struct l1_cache_block b1, struct l1_cache_block b2){
        
        return b1.lru< b2.lru;
    }
};

struct l1_cache{
    queue<pair<uint64, struct mem_access>> snacks;
    queue<struct mem_access> req_from_processor;
    queue<struct msg> msg_from_l2_or_l1_cache;

    vector<struct mem_access> outstanding_req_buffer;
    

    vector<set<l1_cache_block,l1_cache_block_cmp>> all_cache_blocks;  

    l1_cache(){
        for(int i=0;i<l1_sets;i++){
            set<l1_cache_block,l1_cache_block_cmp> one_cache_line;
            for(int j=0;j<l1_assoc;j++){
                struct l1_cache_block one_cache_block;
                one_cache_line.insert(one_cache_block);
            }
            all_cache_blocks.push_back(one_cache_line);
        }
    }

    //all utility functions;

    uint64 is_cache_hit(uint64 addr ){
        uint64 set = addr%l1_sets;
        for(auto it=all_cache_blocks[set].begin() ; it!=all_cache_blocks[set].end();it++){
            if(it->tag==addr && it->is_valid == true){
                return 1;

            }
        }
        return 0;
    }

    uint64 cache_block_state(uint64 addr){
        uint64 set = addr%l1_sets;
        for(auto it=all_cache_blocks[set].begin() ;it!=all_cache_blocks[set].end();it++){
            if(it->tag==addr && it->is_valid == true){
                return it->states;
            }
        }
        cout<<"Checking permission of a block which is not in cache"<<endl;
        return INVALID;
    }

    uint64 insert(uint64 addr, uint64 state){

        // uint64 set = addr%l1_sets;
        // for(auto it=all_cache_blocks[set].begin() ;it!=all_cache_blocks[set].end();it++){
        //     if(it->tag==addr && it->is_valid == true){
        //         return it->states;
        //     }
        // }
        return 0;
    }

    void update_lru(uint64 addr, uint64 ctr){
        uint64 set = addr%l1_sets;
        for(auto it=all_cache_blocks[set].begin() ;it!=all_cache_blocks[set].end();it++){
            if(it->tag==addr && it->is_valid == true){
                auto x = *it;
                all_cache_blocks[set].erase(it);
                x.lru = ctr;
                all_cache_blocks[set].insert(x);
            }
        }
    }


};


struct l2_cache_block{
    bool is_valid;
    uint64 tag;
    uint64 dirty;
    uint64 dentry;
    uint64 lru;

    l2_cache_block(){
        is_valid = false;
        tag = 0;
        dirty = 0;
        dentry = 0;
        lru = 0;
    }

};

struct l2_cache_block_cmp {
    bool operator() (struct l2_cache_block b1, struct l2_cache_block b2){
        
        return b1.lru< b2.lru;
    }
};

uint64 l2_bank_size = 512*1024;
uint64 l2_bank_line_size=64;
uint64 l2_bank_assoc = 16;
uint64 l2_bank_sets = l2_bank_size/(l2_bank_line_size*l2_bank_assoc);



struct l2_bank{

    queue<struct msg> msg_incoming;

    vector<set<l2_cache_block,l2_cache_block_cmp>> all_cache_blocks;

    //all utility functions
    l2_bank(){

        for(int i=0;i<l2_bank_sets;i++){
            set<l2_cache_block,l2_cache_block_cmp> one_cache_line;
            for(int j=0;j<l2_bank_assoc;j++){
                struct l2_cache_block one_cache_block;
                one_cache_line.insert(one_cache_block);
            }
            all_cache_blocks.push_back(one_cache_line);
        }

    }



};

