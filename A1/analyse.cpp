#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<fstream>
#include "cache.h"


struct entry {
    char i_or_d;
    char type;
    unsigned long long addr;
    unsigned pc;
};

void printHelp(){
    std::cout << "\nUsage: ./analyse <policy> where policy can be 'i' for Inclusive Policy, 'n' for Not-Inclusive-Not-Exclusive Policy, 'e' for Exclusive Policy.\n";
    std::cout << "The program will output the results using that policy.\n";
}

int main(int argc, char** argv){
    std::cout << "Application to analyse traces\n";
    if(argc != 2){
        printHelp();
        exit(0);
    }

    // Choosing the policy for inclusivity
    int policy = INCLUSIVE_POLICY;
    if(argv[1][0] == 'i'){
        std::cout << "Choosing Inclusive Policy\n";
        policy = INCLUSIVE_POLICY;
    }
    else if(argv[1][0] == 'n'){
        std::cout << "Choosing NINE Policy\n";
        policy = NINE_POLICY;
    }
    else if(argv[1][0] == 'e'){
        std::cout << "Choosing Exclusive Policy\n";
        policy = EXCLUSIVE_POLICY;
    }
    else{
        printHelp();
        exit(0);
    }


    // Setting up memory and cache layers
    LRUCache L2(8, 64, 512*1024), L3(16, 64, 2*1024*1024);
    Memory mem({L2, L3}, policy);

    std::map<std::string, std::vector<std::string>> apps = {
        {"bzip2", {"bzip2.log_l1misstrace_0", "bzip2.log_l1misstrace_1"}}, 
        {"gcc", {"gcc.log_l1misstrace_0", "gcc.log_l1misstrace_1"}}, 
        {"gromacs", {"gromacs.log_l1misstrace_0"}}, 
        {"h264ref", {"h264ref.log_l1misstrace_0"}}, 
        {"hmmer", {"hmmer.log_l1misstrace_0"}}, 
        {"sphinx3", {"sphinx3.log_l1misstrace_0", "sphinx3.log_l1misstrace_1"}}
    };
    std::string BASE_PATH = "./traces/";
    for(auto app: apps){
        mem.reset(policy);
        std::cout << "\nAnalysing trace for " << app.first << "\n";
        int counter = 0;
        for(auto path: app.second){
            std::fstream file;
            struct entry temp;
            file.open(BASE_PATH + path, std::ios::in|std::ios::binary);
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
        std::cout << "Count: " << counter << "\n";
        mem.printStats();        
    }
    return 0;
}