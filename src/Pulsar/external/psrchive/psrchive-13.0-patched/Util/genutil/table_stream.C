/***************************************************************************
 *
 *   Copyright (C) 2007 David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "table_stream.h"

table_stream &operator<<( table_stream &rhs, const std::string &lhs )
{
  rhs.add_string( lhs );
  return rhs;
}

static table_stream::token_type newline = std::endl;

extern table_stream &operator<<( table_stream &rhs, 
				 table_stream::token_type lhs )
{
  if( lhs == newline )
    rhs.add_row();

  return rhs;
}




using namespace std;


table_stream::table_stream( ostream *set_target )
{
  adding_headings = true;
  set_stream( set_target );
}


void table_stream::set_stream( ostream *set_target )
{
  target = set_target;
}


void table_stream::flush( void )
{
  determine_widths();
  determine_justifications();

  
  for( unsigned col = 0; col < headings.size(); col ++ )
  {
    (*target) << " " << headings[col];
    int padding = column_widths[col] - headings[col].length();
    for( int i = 0; i < padding; i ++ ) (*target) << " ";
  }
  (*target) << endl << endl;

  for( unsigned row = 0; row < data.size(); row ++ )
  {
    for( unsigned col = 0; col < data[row].size(); col ++ )
    {
      int padding = column_widths[col] - data[row][col].length();
      
      (*target) << " ";
      
      if( justifications[col] == right )
	for( int i = 0; i < padding; i ++ ) (*target) << " ";
      
      (*target) << data[row][col];
      
      if( justifications[col] == left )
	for( int i = 0; i < padding; i ++ ) (*target) << " ";
    }
    (*target) << endl;
  }
}



void table_stream::add_string( string new_cell )
{
  if( adding_headings )
    headings.push_back( new_cell );
  else
    data[current_row].push_back( new_cell );
}


void table_stream::add_row( void )
{
  data.push_back( vector< string>() );

  if( adding_headings )
  {
    current_row = 0;
    adding_headings = false;
  }
  else
  {
    current_row ++;
  }
}



void table_stream::determine_widths( void )
{

  // use the heading widths to initialise the column widths

  column_widths.resize( headings.size() );
  for( unsigned col = 0; col < headings.size(); col ++ )
  {
    column_widths[col] = headings[col].length() + 1;
  }

  // go through each row, updating the column width for each header if the data
  // in a cell is wider than the current width

  for( unsigned row = 0; row < data.size(); row ++ )
  {
    vector< string > next_row = data[row];
    for( unsigned col = 0; col < next_row.size(); col ++ )
    {
      int next_length = data[row][col].length() + 1;
      if( next_length > column_widths[col] )
        column_widths[col] = next_length;
    }
  }
}


// NOTE - who would have thought is_numeric would be so complicated, need to write or find a better one, this comes nowhere
//        near what we require for vap.

bool table_stream::is_numeric( string src )
{
  for( unsigned i =0; i < src.size(); i ++ )
  {
    char next_char = src[i];
    if( next_char != '.' &&
	next_char != 'e' &&
	next_char != '-' &&
	next_char != '+' &&
	(next_char < char('0') || next_char > char('9') ) )
      return false;
  }

  return true;
}



void table_stream::determine_justifications( void )
{
  justifications.resize( headings.size() );
  
  if (!data.size())
    return;

  if (data[0].size() != justifications.size())
    return;

  for( unsigned col = 0; col < data[0].size(); col ++ )
  {
    if( is_numeric( data[0][col] ) )
      justifications[col] = right;
    else
      justifications[col] = left;
  }
}




