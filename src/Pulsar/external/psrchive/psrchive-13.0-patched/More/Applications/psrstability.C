/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten, Stefan Oslowski
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"

#include "Pulsar/Archive.h"

#include "Pulsar/TimeAppend.h"

//assumes J0437-4715!!!
#define PERIOD 0.005757451840111090

using namespace std;
using namespace Pulsar;

#include <time.h>

class stability : public Pulsar::Application
{
public:
 //! Default constructor:
 stability ();

 //! Process the given archive
 void process (Pulsar::Archive*);

 long int i;
  Pulsar::TimeAppend timeApp;

protected:

  void add_options (CommandLine::Menu&);
  
  void set_number_of_turns (long int _turns);
  void set_number_of_turns_seconds (double seconds);
  void set_path_for_output (string _path);
  void set_profile_count_offset (long int _offset);

  vector<string> files;

  long int turns;
  long int offset;

  Reference::To<Pulsar::Archive> total;
  Reference::To<Pulsar::Archive> total_buffered;
  Reference::To<Pulsar::Archive> output;
  Reference::To<Pulsar::Archive> buffer;
  Reference::To<Pulsar::Archive> out_of_buffer;
  //Pulsar::Archive* output;
  //Pulsar::Archive* total;

  //char outnamechar[256];
  //char path[128];
  string path;
  string outnamechar;

  stringstream ss;

  // subints for buffer
  vector<unsigned int> buffer_subints;
  vector<unsigned int> out_of_buffer_subints;

  // buffer control variables
  bool buffer_exists;
  long buffer_left;
  long buffer_size;

  time_t seconds;
  time_t start;
  time_t duration;
};

stability::stability()
  : Application ("stability", "study the stability of a pulsar profile")
{
  i=0;
  set_number_of_turns(52106);
  set_profile_count_offset(0);
  timeApp.must_match = false;
  buffer_exists = false;
  buffer_left = 0;

  start = time (NULL);
}

void stability::add_options ( CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  // blank line in help
  menu.add("");

  arg = menu.add (this, &stability::set_number_of_turns, 't',"turns");
  arg->set_help ("how many turns per profile?");
  arg->set_long_help ("After this number of turns is inserted into the profile \n"
		      "the program will start removing first one and adding another one \n");

  arg = menu.add (this, &stability::set_number_of_turns_seconds, 'T',"seconds");
  arg->set_help ("how many seconds per profile?");
  arg->set_long_help ("After this many seconds are inserted into the profile \n"
		      "the program will start removing first one and adding another one \n"
		      "WARNING:This assumes J0437-4715 period \n");

  arg = menu.add (this, &stability::set_path_for_output, 'O',"path");
  arg->set_help ("set path for output, has to finish with '/'");

  arg = menu.add (this, &stability::set_profile_count_offset, 'o',"offset");
  arg->set_help ("set profile count offset");
  arg->set_long_help ("This option is useful when splitting the stability study\n"
		      "over many processors");
}

void stability::set_path_for_output (string _path)
{
  //cout << "received _path=" << _path << endl;
  path = _path;
  //cout << "set the output path to " << path << endl;
}

void stability::set_number_of_turns (long int _turns)
{
  turns = _turns;
  //cout << "set the number of turns to " << turns << endl;
}

void stability::set_number_of_turns_seconds (double seconds)
{
  turns = (long int)(seconds / PERIOD);
  cout << "set the number of turns to " << turns << endl;
}

void stability::set_profile_count_offset (long int _offset)
{
  offset = _offset;
}

void stability::process (Pulsar::Archive* archive)
{
  archive->fscrunch_to_nchan(1);
  archive->pscrunch();

  if (i < 1)
  {
    total = archive;
    timeApp.init(total);
  }
  else if (i < turns)
  {
    timeApp.append(total, archive);
  }
  else
  {
    if (i == turns) 
    {
      cout << "reached full archive, will start removing/adding" << endl;
    }
    //if (!buffer_exists)
    if (buffer_left == 0)
    {
      //this whole block could be a separate function, void generate_buffer()
      buffer_size = (long)((double)turns * 0.9);
      buffer_left = turns - buffer_size;
      //clear the subints vectors:
      buffer_subints.clear();
      out_of_buffer_subints.clear();

      for (int _i=0;_i<buffer_size;_i++) {
	buffer_subints.push_back(buffer_left+_i);
	if (_i < buffer_left)
	{
	  out_of_buffer_subints.push_back(_i);
	}
      }

/*      cout << "got subints! " << endl;
      cout << "contents of out_of_buffer_subints:" << endl;
      for (int _i=0; _i < out_of_buffer_subints.size(); _i++)
	cout << " " << out_of_buffer_subints[_i];
      cout << endl;
      cout << "contents of buffer_subints:" << endl;
      for (int _i=0; _i < buffer_subints.size(); _i++)
	cout << " " << buffer_subints[_i];
      cout << endl; */

      buffer = total->extract(buffer_subints);
      //timeApp.init(buffer);
      buffer->tscrunch();

      out_of_buffer = total->extract(out_of_buffer_subints);

      total_buffered = out_of_buffer;
      timeApp.init(total_buffered);
      timeApp.append(total_buffered, buffer);
    }
    output = total_buffered->total(true);
    
    ss << path << "profile_" << i-turns << ".ar";
    outnamechar = ss.str();
    ss.str("");
    output->unload(outnamechar);
    timeApp.append(total, archive);
    timeApp.append(total_buffered, archive);
    total->erase (0);
    total_buffered->erase (0);

    //cout << "buff: size=" << buffer_size << " left=" << buffer_left << endl;
    buffer_left--;
  }
  i++;
}

int main (int argc, char** argv)
{
  stability program;
  return program.main (argc, argv);
}
