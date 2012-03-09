#ifndef _WAPP_CONVERT_H
#define _WAPP_CONVERT_H

#include <string.h>
#include "machine_endian.h"
#include "wapp_headers.h"

template<class DEST_HEADER, class ORIG_HEADER>
int wapp_hdr_convert_common(DEST_HEADER *dest, char *rawsrc);

template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v5(DEST_HEADER *dest, char *rawsrc);

template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v6(DEST_HEADER *dest, char *rawsrc); 

template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v7(DEST_HEADER *dest, char *rawsrc); 

template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v8(DEST_HEADER *dest, char *rawsrc); 

template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v9(DEST_HEADER *dest, char *rawsrc); 

template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_iflo_lt9(DEST_HEADER *dest, char *rawsrc);

template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_iflo_v9(DEST_HEADER *dest, char *rawsrc);

int wapp_hdr_convert(WAPP_HEADER *dest, char *rawsrc);

// Convert wrapper func
int wapp_hdr_convert(WAPP_HEADER *dest, char *rawsrc) {
  int ver = *(int32_t *)rawsrc;
  int rv;
  if (ver==2) { 
    rv = wapp_hdr_convert_common<WAPP_HEADER, WAPP_HEADER_v2>(dest,rawsrc);
    return(rv);
  } else if (ver==3) {
    rv = wapp_hdr_convert_common<WAPP_HEADER, WAPP_HEADER_v3>(dest,rawsrc);
    return(rv);
  } else if (ver==4) {
    rv = wapp_hdr_convert_common<WAPP_HEADER, WAPP_HEADER_v4>(dest,rawsrc);
    return(rv);
  } else if (ver==5) {
    rv = wapp_hdr_convert_common<WAPP_HEADER, WAPP_HEADER_v5>(dest,rawsrc);
    wapp_hdr_convert_v5<WAPP_HEADER, WAPP_HEADER_v5>(dest,rawsrc);
    return(rv);
  } else if (ver==6) {
    rv = wapp_hdr_convert_common<WAPP_HEADER, WAPP_HEADER_v6>(dest,rawsrc);
    wapp_hdr_convert_v5<WAPP_HEADER, WAPP_HEADER_v6>(dest,rawsrc);
    wapp_hdr_convert_v6<WAPP_HEADER, WAPP_HEADER_v6>(dest,rawsrc);
    wapp_hdr_convert_iflo_lt9<WAPP_HEADER, WAPP_HEADER_v6>(dest,rawsrc);
    return(rv);
  } else if (ver==7) {
    rv = wapp_hdr_convert_common<WAPP_HEADER, WAPP_HEADER_v7>(dest,rawsrc);
    wapp_hdr_convert_v5<WAPP_HEADER, WAPP_HEADER_v7>(dest,rawsrc);
    wapp_hdr_convert_v6<WAPP_HEADER, WAPP_HEADER_v7>(dest,rawsrc);
    wapp_hdr_convert_iflo_lt9<WAPP_HEADER, WAPP_HEADER_v7>(dest,rawsrc);
    wapp_hdr_convert_v7<WAPP_HEADER, WAPP_HEADER_v7>(dest,rawsrc);
    return(rv);
  } else if (ver==8) {
    rv = wapp_hdr_convert_common<WAPP_HEADER, WAPP_HEADER_v8>(dest,rawsrc);
    wapp_hdr_convert_v5<WAPP_HEADER, WAPP_HEADER_v8>(dest,rawsrc);
    wapp_hdr_convert_v6<WAPP_HEADER, WAPP_HEADER_v8>(dest,rawsrc);
    wapp_hdr_convert_iflo_lt9<WAPP_HEADER, WAPP_HEADER_v8>(dest,rawsrc);
    wapp_hdr_convert_v7<WAPP_HEADER, WAPP_HEADER_v8>(dest,rawsrc);
    wapp_hdr_convert_v8<WAPP_HEADER, WAPP_HEADER_v8>(dest,rawsrc);
    return(rv);
  } else if (ver==9) {
    rv = wapp_hdr_convert_common<WAPP_HEADER, WAPP_HEADER_v9>(dest,rawsrc);
    wapp_hdr_convert_v5<WAPP_HEADER, WAPP_HEADER_v9>(dest,rawsrc);
    wapp_hdr_convert_v6<WAPP_HEADER, WAPP_HEADER_v9>(dest,rawsrc);
    wapp_hdr_convert_iflo_v9<WAPP_HEADER, WAPP_HEADER_v9>(dest,rawsrc);
    wapp_hdr_convert_v7<WAPP_HEADER, WAPP_HEADER_v9>(dest,rawsrc);
    wapp_hdr_convert_v8<WAPP_HEADER, WAPP_HEADER_v9>(dest,rawsrc);
    wapp_hdr_convert_v9<WAPP_HEADER, WAPP_HEADER_v9>(dest,rawsrc);
    return(rv);
    //memcpy(dest,rawsrc,sizeof(WAPP_HEADER_v9));
    //return(0);
  } else { 
    return(-1); 
  }
}

#if MACHINE_LITTLE_ENDIAN
#  define WAPP_SET_PARAM(p) dest->p = src->p
#  define WAPP_CPY_PARAM(p) memcpy(dest->p, src->p, sizeof(src->p))
#else 
#  define WAPP_SET_PARAM(p) do {\
  if (sizeof(src->p)>1) ChangeEndian(src->p); \
  dest->p = src->p; } while (0)
# define WAPP_CPY_PARAM(p) do {\
  if (sizeof(src->p[0])>1) \
    array_changeEndian(sizeof(src->p)/sizeof(src->p[0]), src->p, \
        sizeof(src->p[0])); \
  memcpy(dest->p, src->p, sizeof(src->p)); } while(0)
#endif

// Convert common attributes
template<class DEST_HEADER, class ORIG_HEADER>
int wapp_hdr_convert_common (DEST_HEADER *dest, char *rawsrc) 
{
  ORIG_HEADER *src = (ORIG_HEADER *)rawsrc;
  int orig_version = src->header_version;

  // We only handle versions starting at 2
  if (orig_version<2) { return(-1); }

  // Zero out dest struct
  memset(dest, 0, sizeof(DEST_HEADER));

  // Attributes common to all versions
  WAPP_SET_PARAM(header_version);
  WAPP_SET_PARAM(header_size);
  WAPP_CPY_PARAM(obs_type);
  WAPP_SET_PARAM(src_ra);
  WAPP_SET_PARAM(src_dec);
  WAPP_SET_PARAM(start_az);
  WAPP_SET_PARAM(start_za);
  WAPP_SET_PARAM(start_ast);
  WAPP_SET_PARAM(start_lst);
  WAPP_SET_PARAM(cent_freq);
  WAPP_SET_PARAM(obs_time);
  WAPP_SET_PARAM(samp_time);
  WAPP_SET_PARAM(wapp_time);
  WAPP_SET_PARAM(bandwidth);
  WAPP_SET_PARAM(num_lags);
  WAPP_SET_PARAM(scan_number);
  WAPP_CPY_PARAM(src_name);
  WAPP_CPY_PARAM(obs_date);
  WAPP_CPY_PARAM(start_time);
  WAPP_CPY_PARAM(project_id);
  WAPP_CPY_PARAM(observers);
  WAPP_SET_PARAM(nifs);
  WAPP_SET_PARAM(level);
  WAPP_SET_PARAM(sum);
  WAPP_SET_PARAM(freqinversion);
  WAPP_SET_PARAM(timeoff);
  WAPP_SET_PARAM(lagformat);
  WAPP_SET_PARAM(lagtrunc);
  WAPP_SET_PARAM(firstchannel);
  WAPP_SET_PARAM(nbins);
  WAPP_SET_PARAM(dumptime);
  WAPP_CPY_PARAM(power_analog);
  WAPP_SET_PARAM(psr_dm);
  WAPP_CPY_PARAM(rphase);
  WAPP_CPY_PARAM(psr_f0);
  WAPP_CPY_PARAM(poly_tmid);
  WAPP_CPY_PARAM(coeff);
  WAPP_CPY_PARAM(num_coeffs);

  return(orig_version);
}

// Things introduced in version 5
template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v5(DEST_HEADER *dest, char *rawsrc) 
{
  ORIG_HEADER *src = (ORIG_HEADER *)rawsrc;

  // Versions 5 additions
  WAPP_CPY_PARAM(hostname);
}
  
// Version 6
template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v6(DEST_HEADER *dest, char *rawsrc) 
{
  ORIG_HEADER *src = (ORIG_HEADER *)rawsrc;

  // Version 6 additions
  WAPP_SET_PARAM(fold_bits);

  // Some of the IFLO stuff
  WAPP_SET_PARAM(syn1);
  WAPP_CPY_PARAM(synfrq);
  WAPP_SET_PARAM(ifnum);
  WAPP_SET_PARAM(phbsig);
  WAPP_SET_PARAM(hybrid);
  WAPP_SET_PARAM(phblo);

}

// Convert iflo_flip from versions less than 9 to v9+
template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_iflo_lt9(DEST_HEADER *dest, char *rawsrc)
{
  ORIG_HEADER *src = (ORIG_HEADER *)rawsrc;
#if (MACHINE_LITTLE_ENDIAN==0)
  ChangeEndian(src->iflo_flip);
#endif
  dest->iflo_flip[0] = src->iflo_flip;
  dest->iflo_flip[1] = src->iflo_flip;
}

// Convert iflo_flip for versions v9+
template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_iflo_v9(DEST_HEADER *dest, char *rawsrc)
{
  ORIG_HEADER *src = (ORIG_HEADER *)rawsrc;
  WAPP_CPY_PARAM(iflo_flip);
}

// Version 7
template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v7(DEST_HEADER *dest, char *rawsrc)
{
  ORIG_HEADER *src = (ORIG_HEADER *)rawsrc;
  WAPP_SET_PARAM(isfolding);
  WAPP_SET_PARAM(isalfa);
}

// Version 8
template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v8(DEST_HEADER *dest, char *rawsrc) 
{
  ORIG_HEADER *src = (ORIG_HEADER *)rawsrc;

  // Version 8 additions
  WAPP_CPY_PARAM(frontend);

  // TODO : all the alfa stuff?

}

// Version 9
template<class DEST_HEADER, class ORIG_HEADER>
void wapp_hdr_convert_v9(DEST_HEADER *dest, char *rawsrc)
{
  ORIG_HEADER *src = (ORIG_HEADER *)rawsrc;

  // Version 9 additions
  WAPP_SET_PARAM(isdual);
  
}

#endif
