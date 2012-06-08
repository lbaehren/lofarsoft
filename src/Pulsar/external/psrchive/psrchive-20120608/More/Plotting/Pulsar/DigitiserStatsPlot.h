/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#ifndef DIGITISER_STATS_H_
#define DIGITISER_STATS_H_


#include "Pulsar/SimplePlot.h"
#include <Pulsar/DigitiserStatistics.h>
#include <vector>



using std::pair;
using std::vector;



namespace Pulsar
{
  class DigitiserStatsPlot : public SimplePlot
  {
  public:
    DigitiserStatsPlot();

  class Interface : public TextInterface::To<DigitiserStatsPlot>
    {
    public:
      Interface( DigitiserStatsPlot *s_instance = NULL );
    };

    TextInterface::Parser *get_interface();

    std::string get_xlabel( const Archive * );
    std::string get_ylabel( const Archive * );

    void AdjustSubRange( void );
    bool CheckStats( Reference::To<DigitiserStatistics> ext );

    void prepare( const Archive * );
    void draw( const Archive * );

    void set_subint( int s_subint ) { subint = s_subint; }
    int get_subint() const { return subint; }

    pair<int,int> get_srange() const { return srange; }
    void set_srange( const pair<int,int> &s_srange ) { srange = s_srange; }
  private:
    float y_min, y_max;
    vector< vector< vector< float > > > profiles;
    bool valid_archive;

    int subint;
    pair<int,int> srange;

    int ncycsub;
    int ndigr;
    int npar;
    int nsub;
  };
}

#endif
