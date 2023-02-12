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
    int policy = INCLUSIVE_POLICY;
    // Setting up memory and cache layers
    LRUCache L2(8, 64, 512*1024), L3(16, 64, 2*1024*1024);
    Memory mem({L2, L3}, policy);

    map<string, vector<string>> apps = {
        {"bzip2", {"bzip2.log_l1misstrace_0", "bzip2.log_l1misstrace_1"}}, 
        {"gcc", {"gcc.log_l1misstrace_0", "gcc.log_l1misstrace_1"}}, 
        {"gromacs", {"gromacs.log_l1misstrace_0"}}, 
        {"h264ref", {"h264ref.log_l1misstrace_0"}}, 
        {"hmmer", {"hmmer.log_l1misstrace_0"}}, 
        {"sphinx3", {"sphinx3.log_l1misstrace_0", "sphinx3.log_l1misstrace_1"}}
    };
    string BASE_PATH = "./traces/";
    for(auto app: apps){
        mem.reset(policy);
        cout << "Analysing trace for " << app.first << "\n";
        int counter = 0;
        for(auto path: app.second){
            fstream file;
            struct entry temp;
            file.open(BASE_PATH + path, ios::in|ios::binary);
            if(file){
                while(file.read((char *)&temp.i_or_d, sizeof(char))){
                    file.read((char *)&temp.type, sizeof(char));
                    file.read((char *)&temp.addr, sizeof(unsigned long long));
                    file.read((char *)&temp.pc, sizeof(unsigned));
                    if(temp.type){
                        mem.handlePkt(temp.addr);
                        counter++;
                    }
                }
                file.close();
            }   
        }
        cout << "Count: " << counter << "\n";
        mem.printStats();        
    }
    return 0;
}