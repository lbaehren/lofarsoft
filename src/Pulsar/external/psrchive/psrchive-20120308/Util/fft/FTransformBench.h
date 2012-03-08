//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __FTransformBench_h_
#define __FTransformBench_h_

#include "FTransform.h"

namespace FTransform {

  //! Stores Fourier transform benchmark data
  class Bench : public Reference::Able
  {
  public:

    static bool verbose;

    //! Construct from installed benchmarks
    Bench ();

    //! Set the patch from which benchmarks will be loaded
    void set_path (const std::string& path);

    //! Set the number of threads for which benchmarks will be loaded
    void set_nthread (unsigned nthread);

    //! Get the maximum FFT length measured
    unsigned get_max_nfft () const;

    class Entry;

    //! Get the best FFT cost for the specified transform length
    Entry get_best (unsigned nfft) const;

  protected:

    //! Database entries
    mutable std::vector<Entry> entries;
    mutable unsigned max_nfft;
    mutable bool loaded;

    std::string path;
    unsigned nthread;

    void reset ();

    virtual void load () const;
    virtual void load (const std::string& library,
		       const std::string& filename) const;
  };

  class Bench::Entry
  {
  public:
    std::string library;
    unsigned nfft;
    double cost;

    Entry () { nfft = 0; cost = 0.0; }
  };
}

#endif
