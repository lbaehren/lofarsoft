/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "fitsutil.h"
#include <fitsio.h>

#ifdef sun
#include <ieeefp.h>
float fits_nullfloat = FP_QNAN;
#else
float fits_nullfloat = strtod("NAN(n-charsequence)", (char**) NULL);
#endif
  
const char* fits_datatype_str (int datatype)
{
  switch (datatype) {

  case TBIT:
    return "bit 'X'";
  case TBYTE:
    return "byte 'B'";
  case TLOGICAL:
    return "logical 'L'";
  case TSTRING:
    return "string 'A'";
  case TSHORT:
    return "short 'I'";

#ifdef linux
  case TINT32BIT:
    return "32-bit int 'J'";
#endif

#ifdef __alpha
    case TLONG:
    return "long 'J'";
#endif

  case TFLOAT:
    return "float 'E'";
  case TDOUBLE:
    return "double 'D'";
  case TCOMPLEX:
    return "complex 'C'";
  case TDBLCOMPLEX:
    return "double complex 'M'";
  case TINT:
    return "int";
  case TUINT:
    return "unsigned int";
  case TUSHORT:
    return "unsigned short";
  case TULONG:
    return "unsigned long";
  default:
    return "unknown";
  }

}
