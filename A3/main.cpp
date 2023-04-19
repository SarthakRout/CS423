#include<bits/stdc++.h>
#include "structure.h"
using namespace std;

vector<struct l1_cache> l1_cache_array(8);
vector<struct l2_bank> l2_bank_array(8);


void process_l1_input(uint64 i , struct mem_access & ma){
    if(ma.addr == 0){
        return;
    }
    if(ma.is_write == 0){
        // read
        // if is cahche hit
        if(l1_cache_array[i].is_cache_hit(ma.addr)){
            // then return and update lru
            l1_cache_array[i].update_lru(ma.addr, ma.global_ctr );
        }
        else{
            // else 
            // check for outstanding msg queue 
            for(auto &x: l1_cache_array[i].outstanding_req_buffer){
                if(x.addr == ma.addr){
                    x.global_ctr = ma.global_ctr;
                    return;
                }
            }
            // otherwise send message to l2 homebank to 
            uint64 home = ma.addr&7;
            struct msg mx = {MSG_GET, i, ma.addr, home};
            l2_bank_array[home].msg_incoming.push(mx);
            l1_cache_array[i].outstanding_req_buffer.push_back(ma);
            // request for block and put this in outstanding vector
            return;
        }
    }
    else{
        if(l1_cache_array[i].is_cache_hit(ma.addr)){
            // write
            // if cache hit
            uint64 state = l1_cache_array[i].cache_block_state(ma.addr);
            if(state == EXCLUSIVE){
            // is state is E, silently modify data and change state to M  and update lru
                uint64 set = ma.addr % l1_sets;
                for(auto it=l1_cache_array[i].all_cache_blocks[set].begin() ;it!=l1_cache_array[i].all_cache_blocks[set].end();it++){
                    if(it->tag==ma.addr && it->is_valid == true){
                        auto x = *it;
                        x.lru = ma.global_ctr;
                        x.states = MODIFIED;
                        x.is_dirty = 1;
                        l1_cache_array[i].all_cache_blocks[set].erase(it);
                        l1_cache_array[i].all_cache_blocks[set].insert(x);
                        return;
                    }
                }
                printf("Shouldn't come here2\n");
            }
            else if(state == SHARED){
            // if state is S, send upgrade request and check outstanding req buffer
                for(auto &x: l1_cache_array[i].outstanding_req_buffer){
                    if(x.addr == ma.addr && x.is_write == 1){
                        x.global_ctr = ma.global_ctr;
                        return;
                    }
                }
                // otherwise send message to l2 homebank to 
                uint64 home = ma.addr&7;
                struct msg mx = {MSG_UPGR, i, ma.addr, home};
                l2_bank_array[home].msg_incoming.push(mx);
                l1_cache_array[i].outstanding_req_buffer.push_back(ma);
                // It is possible that due to race condition, the home bank
                // may send invalidation request for this block
                // IN such a case, we receive NACK request, due to which
                // we repeat the request again
            }
            else if(state == MODIFIED){
                // if state is M, modify update lru 
                l1_cache_array[i].update_lru(ma.addr, ma.global_ctr);
            }
            else{
                printf("Should never come here\n");
            }
            // and return
        }
        else{
            //  Write Miss
            for(auto &x: l1_cache_array[i].outstanding_req_buffer){
                if(x.addr == ma.addr && x.is_write == 1){
                    x.global_ctr = ma.global_ctr;
                    return;
                }
            }
            // otherwise send message to l2 homebank to 
            uint64 home = ma.addr&7;
            struct msg mx = {MSG_GETX, i, ma.addr, home};
            l2_bank_array[home].msg_incoming.push(mx);
            l1_cache_array[i].outstanding_req_buffer.push_back(ma);
        }
        
        // else
        // check for outstanding msq queue for write request
        // to l2 home bank otherwise request for block
        // and put this in outstanding request queue. 
    }
}

void process_l1_msg_inc(int i, struct msg & m){
    if(m.addr == 0){
        return;
    }
    // switch(m.msg_name){
    //     case 
    // }

}

void process_l2_msg_inc(int i, struct msg & m){
    if(m.addr == 0);

}

int main()
{

    //Initialisng 8 l1_cache    
    //Initialising 8 l2_cache_bank

    vector<ifstream > traces(8);
    
    for(int i=0;i<8;i++){
        string name = "trace";
        traces[i].open(name + to_string(i) + ".out");

    }
    string myline;
    for(int i=0;i<8;i++){
        struct mem_access m; 
        while(getline(traces[i], myline))
        {
            auto cx = stringstream(myline);
            cx >> m.global_ctr >> m.addr >> m.is_write;
            m.addr >>= 6;
            l1_cache_array[i].req_from_processor.push(m);
        }
        cout << l1_cache_array[i].req_from_processor.size()<<"\n";
    }
    long long cycle = 0, cur_gid = -1;
    while(1){
        int flag = 0;
        vector<struct mem_access> temp_ma(8);
        vector<pair<long long, int>> reqv;
        
        // Pop queue and get data
        for(int i = 0; i<8; i++){
            // Check for snacks
            if( l1_cache_array[i].snacks.size() && l1_cache_array[i].snacks.front().first == cycle){
                temp_ma[i] = l1_cache_array[i].snacks.front().second;
                l1_cache_array[i].snacks.pop();
                flag = true;
            }
            else{
                // Make sorting array for normal memory access
                if(l1_cache_array[i].req_from_processor.size()){
                    reqv.push_back({l1_cache_array[i].req_from_processor.front().global_ctr, i});
                    flag = true;
                }
            }
            
        }
        sort(reqv.begin(), reqv.end());
        for(auto x: reqv){
            // Check for normal memory accesses
            if(x.first == cur_gid + 1){        
                cur_gid++;
                temp_ma[x.second] = l1_cache_array[x.second].req_from_processor.front();
                l1_cache_array[x.second].req_from_processor.pop();
            }
            else{
                // Normal memory access condition was found to be not correct and no snacks 
                break;
            }
        }

        vector<struct msg> temp_msg_l1(8), temp_msg_l2(8);

        for(int i= 0; i<8; i++){
            if(l1_cache_array[i].msg_from_l2_or_l1_cache.size()){
                temp_msg_l1[i] = l1_cache_array[i].msg_from_l2_or_l1_cache.front();
                l1_cache_array[i].msg_from_l2_or_l1_cache.pop();
                flag = true;
            }
        }

        for(int i= 0; i<8; i++){
            if(l2_bank_array[i].msg_incoming.size()){
                temp_msg_l2[i] = l2_bank_array[i].msg_incoming.front();
                l2_bank_array[i].msg_incoming.pop();
                flag = true;
            }
        }

        // Process all data

        // Process L1 input
        for(int i = 0; i<8; i++){
            process_l1_input(i, temp_ma[i]);
        }

        // Process L1 incoming msg
        for(int i = 0; i<8; i++){
            process_l1_msg_inc(i, temp_msg_l1[i]);
        }

        // Process L2 incoming msg
        for(int i = 0; i<8; i++){
            process_l2_msg_inc(i, temp_msg_l2[i]);
        }

        if(!flag){
            break;
        }
        cycle++;
    }


    for(int i = 0; i<8; i++){
        cout << l1_cache_array[i].outstanding_req_buffer.size() << " " << l2_bank_array[i].msg_incoming.size() << "\n";
        traces[i].close();
    }
    return 0;
}