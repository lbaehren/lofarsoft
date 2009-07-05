#ifndef HFGET_H
#define HFGET_H

///usr/share/qt4/bin/qmake hfget.pro

#include "VectorSelector.h"
#include "mainwindow.h"
#include "hfget-defs.h"
#include <GUI/Data.h>

#include <QtCore/QMetaObject>
#include <QtGui/QLabel>

using namespace std;

//Empty forward definitions
class DataFuncLibraryClass; 
class DataFuncDescriptor;
class ObjectFunctionClass;

//#include <gtk/gtk.h>
//#include "thread.h" -> qt

//Define Address Space for data elements (i.e. position in an array)
//Use long int if you want to address long data sets


#define RECORD_SEP ":'"


/*!
  Define pointers to the data and function of different types, all using the
  same memory location. This allows calling a void with variables types (e.g.
  using a switch statement)
*/


//------------------------------------------------------------------------
// Casting Operations
//------------------------------------------------------------------------



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// FUNCTION DEFINITIONS
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//========================================================================
//Utilities
//========================================================================

void * new_vector (DATATYPE type, address len);
bool del_vector (void *p, DATATYPE type);
bool del_value (void *p, DATATYPE type);

vector<HString> split_str_into_vector (HString str,
				      char c=',');

int string_find_chars (const HString s,
		       const HString c,
		       int &n,
		       const int npos=0);

int string_rfind_chars (const HString s,
			const HString c,
			int &n,
			const int npos=0);

HString string_rfind_chars_substr (const HString s,
				  const HString c=RECORD_SEP,
				  const int npos=0);

void parse_record_name_to_vector (HString name,
				  vector<HString> &names,
				  vector<DIRECTION> &dirs,
				  HString c=RECORD_SEP);

bool parse_record_selection_to_vector (HString str,
				       HString &name,
				       vector<HString> &elems);

//End Utilities
//========================================================================






//------------------------------------------------------------------------
// object logic operators
//------------------------------------------------------------------------

template <class T>
bool object_logic_cmpr(const Data* o_ptr, const T val);

template <class T>
bool object_logic_in_set(const Data* o_ptr, const vector<T> &set);

// End object logic operators
//------------------------------------------------------------------------


#endif
