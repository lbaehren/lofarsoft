/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <stdio.h>
#include <vector>

// ///////////////////////////////////////////////////////////////////////
// fwrite_compressed
//    writes out a vector of floats to file as an array of unsigned short
//    writes:
//            float offset;
//            float ratio;
//            float nelements;
//            unsigned short elements [nelements];
//    where:
//            vals[N] = offset + elements[N] * ratio;

int fwrite_compressed (FILE* fptr, const std::vector<float>& vals);

// ///////////////////////////////////////////////////////////////////////
// fread_compressed
//    reads in a vector of floats written by the above function...
//    set 'swapendian' to true if the file was written on a machine of 
//    opposite endian.
int fread_compressed (FILE* fptr, std::vector<float>* vals, 
                      bool swapendian=false);
