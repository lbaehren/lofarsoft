//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ProfileAmps.h,v $
   $Revision: 1.2 $
   $Date: 2007/09/19 13:35:19 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileAmps_h
#define __Pulsar_ProfileAmps_h

#include "Pulsar/Container.h"

namespace Pulsar {

  //! Provides protected access to the Profile amplitudes array
  /*!
    By making the amps attribute private, all Profile methods must
    access the array through the get_amps method.
  */
  class ProfileAmps : public Container {

  public:

    //! When true, no memory is allocated for amps
    static bool no_amps;

    //! Constructor initializes the data array
    ProfileAmps (unsigned nbin = 0);

    //! Destructor destroys the data array
    virtual ~ProfileAmps ();
    
    //! Copy constructor
    ProfileAmps (const ProfileAmps&);

    //! Return the number of bins
    unsigned get_nbin () const { return nbin; }

    //! Resize the data area
    virtual void resize (unsigned nbin);

    //! Return a pointer to the amplitudes array
    const float* get_amps () const;

    //! Return a pointer to the amplitudes array
    float* get_amps ();

    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void set_amps (const T* data);

    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void set_amps (const std::vector<T>& data);

    //! set the amplitudes array equal to the contents of the data array
    template <typename T> void get_amps (std::vector<T>& data) const;

    //! expert interface
    class Expert;

  private:

    friend class Expert;

    //! number of bins in the profile
    unsigned nbin;

    //! amplitudes at each pulse phase
    float *amps;

    //! size of the amps array (always >= nbin)
    unsigned amps_size;

  };

}

/*! 
  \param data pointer to the data elements to be copied.
  \pre data must point to at least get_nbin() elements
*/
template <typename T>
void Pulsar::ProfileAmps::set_amps (const T* data)
{
  for (unsigned ibin=0; ibin<nbin; ibin++)
    amps[ibin] = static_cast<float>( data[ibin] );
}

/*! 
  \param data vector of amps
*/
template <typename T>
void Pulsar::ProfileAmps::set_amps (const std::vector<T>& data)
{
  resize (data.size());
  for (unsigned ibin=0; ibin<nbin; ibin++)
    amps[ibin] = static_cast<float>( data[ibin] );
}

/*! 
  \param data vector of amps
*/
template <typename T>
void Pulsar::ProfileAmps::get_amps (std::vector<T>& data) const
{
  data.resize (nbin);
  for (unsigned ibin=0; ibin<nbin; ibin++)
    data[ibin] = static_cast<T>( amps[ibin] );
}

#endif // !defined __Pulsar_ProfileAmps_h
