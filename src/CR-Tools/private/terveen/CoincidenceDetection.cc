#include <iostream>
#include <stdio.h>
#include <fstream>
#include "FRATcoincidence.h"
#include "FRATcoincidence.cc"

using namespace std;
using namespace FRAT::coincidence;

main(int argc, char *argv[] ) {
    if(argc<4){
        cout << "usage: " << argv[0] << " <CoincidenceNumber> <CoincidenceTime> <Filename>" << endl;
        return 200;
    }
    int CoinNr=atoi(argv[1]);
    int CoinTime=atoi(argv[2]);
    CoinCheck* cc;
    cc = new CoinCheck();
    char* filename= argv[3];
    //ifstream b_file("/home/veen/RFItest.txt");
    ifstream b_file(filename);
    string temp;
    string freq;
    string time;
    string strength;
    struct triggerEvent trigger;
    int latestindex;
    while(!b_file.eof())
    {
        b_file >> temp;
        b_file >> temp;
        b_file >> freq;
        trigger.subband=atoi(freq.c_str());
        b_file >> temp;
        b_file >> temp;
        b_file >> temp;
        b_file >> time;
        trigger.time=atoi(time.c_str());
        b_file >> temp;
        b_file >> temp;
        b_file >> strength;
        trigger.max=atoi(strength.c_str());
        latestindex=cc->add2buffer(trigger);
        if(cc->coincidenceCheck(latestindex, CoinNr, CoinTime)) {
             cout << "Trigger found at time: " << time << endl;
        }
        //cout << freq << " " << time << " " << strength << endl; 

    }
    
    
    return 0;
    
} 
