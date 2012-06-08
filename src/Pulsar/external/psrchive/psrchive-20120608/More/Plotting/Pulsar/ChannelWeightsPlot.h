/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/




#ifndef CHANNEL_WEIGHTS_H_
#define CHANNEL_WEIGHTS_H_



#include "Pulsar/FrequencyPlot.h"



namespace Pulsar
{

  //! Plot of data weight vs frequency
  class ChannelWeightsPlot : public FrequencyPlot
  {
  public:
    ChannelWeightsPlot();
    ~ChannelWeightsPlot();

  class Interface : public TextInterface::To<ChannelWeightsPlot>
    {
    public:
      Interface( ChannelWeightsPlot *s_instance = 0 );
    };
    
    TextInterface::Parser *get_interface();

    void prepare( const Archive *data );
    virtual void preprocess( Archive *data ) {}
    void draw( const Archive *data );
  private:
    
    std::vector<float> weights;
    std::pair<float,float> weights_minmax;
  };


}



#endif
