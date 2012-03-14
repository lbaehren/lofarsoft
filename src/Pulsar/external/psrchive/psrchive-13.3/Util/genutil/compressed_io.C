/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "compressed_io.h"
#include "machine_endian.h"
#include <inttypes.h>

#include <iostream>
#include <algorithm>

using namespace std;

int fwrite_compressed (FILE* fptr, const vector<float>& vals)
{
  vector<float>::const_iterator minel = min_element(vals.begin(), vals.end());
  vector<float>::const_iterator maxel = max_element(vals.begin(), vals.end());
  vector<float>::const_iterator ind;

  if (minel == vals.end() || maxel == vals.end())
  {
    cerr << "fwrite_compressed: empty range" << endl;
    return -1;
  }
  float xmin = *minel;
  float xmax = *maxel;
  float ratio = 1.0;

  // dynamic range of quantization in this packing scheme is given by
  // the maximum value of an unsigned short or: (unsigned short)(~0x0)
  if (xmin != xmax)
    ratio = (xmax - xmin) / float((unsigned short)(~0x0));

  size_t writ = fwrite (&xmin, sizeof(float), 1, fptr);
  if (writ < 1)
  {
    perror ("fwrite_compressed: error fwrite(offset)\n");
    return -1;
  }
  writ = fwrite (&ratio, sizeof(float), 1, fptr);
  if (writ < 1)
  {
    perror ("fwrite_compressed: error fwrite(ratio)\n");
    return -1;
  }

  uint64_t size = vals.size();
  writ = fwrite (&size, sizeof(uint64_t), 1, fptr);
  if (writ < 1)
  {
    perror ("fwrite_compressed: error fwrite(size)\n");
    return -1;
  }

  unsigned short value = 0;
  for (ind = vals.begin(); ind != vals.end(); ind++)
  {
    value = (unsigned short) ((*ind - xmin) / ratio);
    writ = fwrite (&value, sizeof(unsigned short), 1, fptr);
    if (writ < 1)
    {
      perror ("fwrite_compressed: error fwrite(value)\n");
      return -1;
    }
  }

  return 0;
}

bool correct_uint64_t_bug = false;

int fread_compressed (FILE* fptr, vector<float>* vals, bool swapendian)
{
  size_t red;

  float offset = 0.0;
  red = fread (&offset, sizeof(float), 1, fptr);
  if (red < 1)
  {
    perror ("fread_compressed: error fread(offset)\n");
    return -1;
  }

  float ratio  = 1.0;
  red = fread (&ratio, sizeof(float), 1, fptr);
  if (red < 1)
  {
    perror ("fread_compressed: error fread(ratio)\n");
    return -1;
  }

  uint64_t size = 0;

  if (correct_uint64_t_bug)
  {
    uint32_t fix_size;
    red = fread (&fix_size, sizeof(uint32_t), 1, fptr);
    if (swapendian)
      ChangeEndian (fix_size);
    size = fix_size;
  }
  else
  {
    red = fread (&size, sizeof(uint64_t), 1, fptr);
    if (swapendian)
      ChangeEndian (size);
  }

  if (red < 1)
  {
    perror ("fread_compressed: error fread(size)\n");
    return -1;
  }
  
  if (swapendian)
  {
    ChangeEndian (offset);
    ChangeEndian (ratio);
  }
  
  vals->resize (size);
  
  unsigned short value = 0;
  for (vector<float>::iterator ind=vals->begin(); ind!=vals->end(); ind++)
  {
    red = fread (&value, sizeof(unsigned short), 1, fptr);
    if (red < 1)
    {
      perror ("fread_compressed: error fread(value)\n");
      return -1;
    }
    if (swapendian)
      ChangeEndian (value);

    *ind = offset + float(value) * ratio;
  }
  return 0;
}

