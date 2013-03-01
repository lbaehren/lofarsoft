/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/



#ifndef DIGITISER_COUNTS_PLOT_H_
#define DIGITISER_COUNTS_PLOT_H_



#include "Pulsar/SimplePlot.h"
#include <Pulsar/DigitiserCounts.h>
#include <vector>



using std::pair;
using std::vector;



namespace Pulsar
{
  //! Plots a histogram of the DigitiserCounts
  class DigitiserCountsPlot : public SimplePlot
  {
  public:
    DigitiserCountsPlot();

    TextInterface::Parser *get_interface();

  class Interface : public TextInterface::To<DigitiserCountsPlot>
    {
    public:
      Interface( DigitiserCountsPlot *s_instance = NULL );
    };

    void CheckCounts( const Archive *data );

    void prepare( const Archive *data );
    void draw( const Archive *data );

    std::string get_xlabel( const Archive *data );
    std::string get_ylabel( const Archive *data );

    pair<int,int> get_srange() const { return srange; }
    void set_srange( const pair<int,int> &s_srange ) { srange = s_srange; }
    void set_srange( int fsub, int lsub ) { set_srange( pair<int,int>( fsub, lsub) ); }

    int get_subint( void ) const { return subint; }
    void set_subint( int s_subint ) { subint = s_subint; }

  private:
    float min_count;
    float max_count;
    float y_jump;
    int first_nz;
    int last_nz;
    int subint;
    pair<int,int> srange;

    bool valid_data;
  };
}



#endif


