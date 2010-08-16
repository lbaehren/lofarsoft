/***************************************************************************
 *
 *   Copyright (C) 2007 by David Smith
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#ifndef __Table_Stream_h
#define __Table_Stream_h



#include <iostream>
#include <vector>
#include <string>



class table_stream
{
public:
  table_stream( std::ostream *set_target );
  void set_stream( std::ostream *set_target );

  typedef std::ostream& (*token_type) ( std::ostream& os );

  enum justification { left, right, centre };

  void add_string( std::string );
  void add_row( void );
  void flush( void );
  
  bool is_numeric( std::string );

  void determine_widths( void );
  void determine_justifications( void );

private:
  std::ostream *target;
  bool adding_headings;
  int current_row;

  std::vector< std::string > headings;
  std::vector< std::vector< std::string > > data;
  std::vector< int > column_widths;
  std::vector< justification > justifications;
};

extern table_stream &operator<<( table_stream &rhs, const std::string &lhs );
extern table_stream &operator<<( table_stream &rhs, table_stream::token_type);



#endif // __Table_Stream_h


