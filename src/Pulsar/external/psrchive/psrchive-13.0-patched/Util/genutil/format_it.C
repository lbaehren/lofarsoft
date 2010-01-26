/***************************************************************************
 *
 *   Copyright (C) 2003 by Haydon Knight
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "format_it.h"
#include "strutil.h"
#include "templates.h"

using namespace std;

vector<string> format_file(string filename, unsigned extra_spaces,
			   vector<unsigned>* rjustify_columns){
  vector<string> lines;
  
  // parse metafile just puts each line into the vector- it's a poorly named function
  loadlines(filename,lines);

  format_it(lines,extra_spaces,rjustify_columns);

  return lines;
}

void format_it(vector<string>& lines, unsigned extra_spaces,
	       vector<unsigned>* rjustify_columns)
{
  bool rjustify_all = false;
  if( rjustify_columns && found(unsigned(0),*rjustify_columns) )
    rjustify_all = true;

  vector<vector<string> > words(lines.size());

  for( unsigned iline=0; iline<lines.size(); iline++)
    words[iline] = stringdecimate(lines[iline],"\t ");

  //
  // For each column, work out the maximum word length
  //
  vector<unsigned> word_lengths;
  
  for( unsigned iline=0; iline<lines.size(); iline++){
    for( unsigned iword=0; iword<words[iline].size(); iword++){
      if( iword==word_lengths.size() )
	word_lengths.push_back(0);
      if( words[iline][iword].length() > word_lengths[iword] ) 
	word_lengths[iword] = words[iline][iword].length();
    }
  }

  //
  // Append to each word spaces to bring them up to the required word length
  //
  for( unsigned iline=0; iline<lines.size(); iline++){
    for( unsigned iword=0; iword<words[iline].size(); iword++){
      unsigned spaces = word_lengths[iword] - words[iline][iword].length() + extra_spaces;
      for( unsigned ispace=0; ispace<spaces; ispace++){
	if( (rjustify_all && iword!=0) || found(iword+1,*rjustify_columns) )
	  words[iline][iword] = ' ' + words[iline][iword];
	else
	  words[iline][iword] += ' ';
      }
      if( rjustify_all || found(iword+1,*rjustify_columns) )
	words[iline][iword] += ' ';
    }
  }

  //
  // Recreate each line
  //
  for( unsigned iline=0; iline<lines.size(); iline++){
    lines[iline] = string();
    for( unsigned iword=0; iword<words[iline].size(); iword++){
      lines[iline] += words[iline][iword];
    }
  }

}
