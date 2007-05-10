#include "TH2.h"
#include "TFile.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TNtuple.h"
//#include "MyObject_newdata.h"
#include "TMath.h"
#include "TSpectrum.h"
#include "TStopwatch.h"
#include "TVirtualFFT.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

char inFileName1[256];

//void ReadOne()
{
    char *memblock;
    long size;
    
    sprintf(inFileName1, "side1.time.output");
    ifstream file1;
    file1.open(inFileName1, ios::in|ios::binary);
  
    if (file1.is_open())
    {
      size = file1.tellg();
      memblock = new char [size];
      file1.seekg (0, ios::beg);
      file1.read (memblock, size);
      file1.close();

      cout << "the complete file content is in memory";

      delete[] memblock;
    }
    //else cout << "Unable to open file";

}


