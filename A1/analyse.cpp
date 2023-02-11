#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<fstream>
#include "cache.h"

using namespace std;

struct entry {
    char i_or_d;
    char type;
    unsigned long long addr;
    unsigned pc;
};

int main(){
    cout << "Application to analyse traces\n";

    // Setting up memory and cache layers
    LRUCache L2(8, 64, 512*1024), L3(16, 64, 2*1024*1024);
    Memory mem({L2, L3}, INCLUSIVE_POLICY);

    map<string, vector<string>> apps = {
        {"bzip2", {"bzip2.log_l1misstrace_0", "bzip2.log_l1misstrace_1"}}, 
        {"gcc", {"gcc.log_l1misstrace_0", "gcc.log_l1misstrace_1"}}, 
        {"gromacs", {"gromacs.log_l1misstrace_0"}}, 
        {"h264ref", {"h264ref.log_l1misstrace_0"}}, 
        {"hmmer", {"hmmer.log_l1misstrace_0"}}, 
        {"sphinx3", {"bzip2.log_l1misstrace_0", "bzip2.log_l1misstrace_1"}}
    };
    string BASE_PATH = "./traces/";
    for(auto app: apps){
        cout << "Analysing trace for " << app.first << "\n";
        int counter = 0;
        for(auto path: app.second){
            fstream file;
            struct entry temp;
            file.open(BASE_PATH + path, ios::in|ios::binary);
            if(file){
                while(file.read((char * )&temp, sizeof(temp))){
                    if(temp.type){
                        mem.handlePkt(temp.addr);
                    }
                }
                file.close();
            }   
        }
        mem.printStats();        
    }
    return 0;
}