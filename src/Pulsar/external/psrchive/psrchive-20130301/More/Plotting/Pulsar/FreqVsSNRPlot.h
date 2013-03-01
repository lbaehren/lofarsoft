
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/FreqVsSNRPlot.h,v $
   $Revision: 1.5 $
   $Date: 2008/08/29 05:34:45 $
   $Author: straten $ */

#ifndef Freq_Vs_SNR_Plot_h_
#define Freq_Vs_SNR_Plot_h_

#include "Pulsar/FrequencyPlot.h"

namespace Pulsar
{
  class FreqVsSNRPlot : public FrequencyPlot
  {
  public:
    FreqVsSNRPlot();

    void prepare( const Pulsar::Archive * );
    void draw( const Pulsar::Archive * );

    TextInterface::Parser *get_interface();
    
    void set_pol( unsigned new_pol ) { pol = new_pol; }
    void set_subint( unsigned new_subint ) { subint = new_subint; }
    
    unsigned get_pol() const { return pol; }
    unsigned get_subint() const { return subint; }

    class Interface : public TextInterface::To<FreqVsSNRPlot>
    {
    public:
      Interface( FreqVsSNRPlot *s_instance );
    };
    
  private:
    
    unsigned subint;
    unsigned pol;
    
    std::vector <float> snrs;
    
  };
}

#endif

