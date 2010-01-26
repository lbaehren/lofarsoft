



#ifndef BANDPASS_CHANNEL_WEIGHT_PLOT_H_
#define BANDPASS_CHANNEL_WEIGHT_PLOT_H_



#include "Pulsar/MultiPlot.h"
#include "Pulsar/BandpassPlot.h"
#include "Pulsar/ChannelWeightsPlot.h"



namespace Pulsar
{

  //! Plot of offpulse bandpass and channel weights
  class BandpassChannelWeightPlot : public MultiPlot
  {
  public:
    BandpassChannelWeightPlot();
    ~BandpassChannelWeightPlot();

  class Interface : public TextInterface::To<BandpassChannelWeightPlot>
    {
    public:
      Interface( BandpassChannelWeightPlot *s_instance = NULL );
    };

    TextInterface::Parser *get_interface();

    void prepare( const Archive *data );
    
    virtual void preprocess( Archive *data ) {};
    
    BandpassPlot *get_band() { return &band; }
    ChannelWeightsPlot *get_weights() { return &weights; }
    
    PlotScale *get_x_scale() { return band.get_frame()->get_x_scale(); }
    PlotScale *get_y_scale() { return band.get_frame()->get_y_scale(); }
  private:
    BandpassPlot band;
    ChannelWeightsPlot weights;
  };


}

#endif


