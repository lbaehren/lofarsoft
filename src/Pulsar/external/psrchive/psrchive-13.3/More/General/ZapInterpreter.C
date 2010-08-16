/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ZapInterpreter.h"
#include "Pulsar/Integration.h"

#include "Pulsar/ChannelZapMedian.h"
#include "Pulsar/LawnMower.h"

#include "TextInterface.h"
#include "pairutil.h"

using namespace std;

string index_help (const string& cmd)
{
  return
    "usage: " + cmd + " iex1 [iex2 ...]\n"
    "  string iexN   unsigned index exression: [i|i1-i2|-iend|istart-]";
}

string pair_help (const string& cmd)
{
  return
    "usage: " + cmd + " i1,j1 [i2,j2 ...]\n"
    "  string i1,j1  unsigned index pair";
}

Pulsar::ZapInterpreter::ZapInterpreter ()
{
  add_command 
    ( &ZapInterpreter::median, 
      "median", "median smooth the passband and zap spikes",
      "usage: median <TI> \n"
      "  type 'zap median help' for text interface help" );

  add_command 
    ( &ZapInterpreter::mow, 
      "mow", "median smooth the profile and clean spikes",
      "usage: mow <TI> \n"
      "  type 'zap mow help' for text interface help" );

  add_command
    ( &ZapInterpreter::chan,
      "chan", "zap specified channels",
      index_help("chan") );

  add_command
    ( &ZapInterpreter::subint,
      "subint", "zap specified integrationss",
      index_help("subint") );

  add_command
    ( &ZapInterpreter::such,
      "such", "zap specified integration and channel",
      pair_help("such") );

  add_command
    ( &ZapInterpreter::edge,
      "edge", "zap fraction of band edges",
      "usage: edge <fraction> \n"
      "  float <fraction>  fraction of band zapped on each edge \n");
}

Pulsar::ZapInterpreter::~ZapInterpreter ()
{
}

string Pulsar::ZapInterpreter::median (const string& args) try
{ 
  vector<string> arguments = setup (args);

  if (!zap_median)
    zap_median = new ChannelZapMedian;

  if (!arguments.size())
  {
    (*zap_median)( get() );
    return response (Good);
  }

  //! Zap median interface
  Reference::To<TextInterface::Parser> interface = zap_median->get_interface();

  string retval;
  for (unsigned icmd=0; icmd < arguments.size(); icmd++) {
    if (icmd)
      retval += " ";
    retval += interface->process (arguments[icmd]);
  }

  return retval;
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::ZapInterpreter::mow (const string& args) try
{ 
  vector<string> arguments = setup (args);

  if (!lawn_mower)
    lawn_mower = new LawnMower;

  if (!arguments.size())
  {
    Reference::To<Archive> data = get();
    for (unsigned isub = 0; isub < data->get_nsubint(); isub++)
    {
      cerr << "mowing subint " << isub << endl;
      lawn_mower->transform( data->get_Integration( isub ) );
    }
    return response (Good);
  }

  //! Zap median interface
  Reference::To<TextInterface::Parser> interface = lawn_mower->get_interface();

  string retval;
  for (unsigned icmd=0; icmd < arguments.size(); icmd++) {
    if (icmd)
      retval += " ";
    retval += interface->process (arguments[icmd]);
  }

  return retval;
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

string Pulsar::ZapInterpreter::empty ()
{
  return response (Fail, help());
}

void parse_indeces (vector<unsigned>& indeces,
		    const vector<string>& arguments,
		    unsigned limit)
{
  // note that the first argument is the command name
  for (unsigned i=0; i<arguments.size(); i++)
    TextInterface::parse_indeces (indeces, "[" + arguments[i] + "]", limit);
}

string Pulsar::ZapInterpreter::chan (const string& args) try 
{
  vector<string> arguments = setup (args);

  vector<unsigned> channels;
  parse_indeces (channels, arguments, get()->get_nchan());

  // zap selected channels in all sub-integrations
  unsigned nsubint = get()->get_nsubint();
  for (unsigned isub=0; isub<nsubint; isub++) {
    Integration* subint = get()->get_Integration(isub);
    for (unsigned i=0; i<channels.size(); i++)
      subint->set_weight( channels[i], 0.0 );
  }
  
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


string Pulsar::ZapInterpreter::subint (const string& args) try 
{
  vector<string> arguments = setup (args);

  vector<unsigned> subints;
  parse_indeces (subints, arguments, get()->get_nsubint());

  // zap all channels in selected sub-integrations
  unsigned nchan = get()->get_nchan();
  for (unsigned i=0; i<subints.size(); i++) {
    Integration* subint = get()->get_Integration( subints[i] );
    for (unsigned ichan=0; ichan<nchan; ichan++)
      subint->set_weight( ichan, 0.0 );
  }
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


template<typename T, typename U>
void parse_pairs (vector< pair<T,U> >& pairs,
		  const vector<string>& arguments,
		  T limit_first, const string& name_first,
		  U limit_second, const string& name_second)
{
  pairs.resize( arguments.size() );

  for (unsigned i=0; i<pairs.size(); i++) {

    pairs[i] = fromstring< pair<T,U> > ( "(" + arguments[i] + ")" );
    
    if (pairs[i].first >= limit_first) {
      Error error (InvalidParam, "parse_pairs");
      error << "i" << name_first << "=" << pairs[i].first 
	    << " >= n" << name_first << "=" << limit_first;
      throw error;
    }

    if (pairs[i].second >= limit_second) {
      Error error (InvalidParam, "parse_pairs");
      error << "i" << name_second << "=" << pairs[i].second
	    << " >= n" << name_second << "=" << limit_second;
      throw error;
    }

    cerr << pairs[i] << endl;
  }
}

string Pulsar::ZapInterpreter::such (const string& args) try 
{
  vector<string> arguments = setup (args);

  vector< pair<unsigned,unsigned> > pairs;
  parse_pairs (pairs, arguments,
	       get()->get_nsubint(), "subint",
	       get()->get_nchan(), "chan");
  
  for (unsigned i=0; i<pairs.size(); i++)
    get()->get_Integration(pairs[i].first)->set_weight(pairs[i].second,0.0);
  
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


// //////////////////////////////////////////////////////////////////////
//
string Pulsar::ZapInterpreter::edge (const string& args)
try {

  float fraction = setup<float> (args);

  if (fraction <= 0.0 || fraction >= 0.5)
    return response (Fail, "invalid fraction " + tostring(fraction));

  Archive* archive = get();

  unsigned isub,  nsub = archive->get_nsubint();
  unsigned ichan, nchan = archive->get_nchan();

  unsigned nedge = unsigned( nchan * fraction );

  for (isub=0; isub < nsub; isub++) {
    Integration* subint = archive->get_Integration (isub);
    for (ichan=0; ichan < nedge; ichan++)
      subint->set_weight (ichan, 0.0);
    for (ichan=nchan-nedge; ichan < nchan; ichan++)
      subint->set_weight (ichan, 0.0);
  }

  return response (Good);

}
catch (Error& error) {
  return response (Fail, error.get_message());
}
