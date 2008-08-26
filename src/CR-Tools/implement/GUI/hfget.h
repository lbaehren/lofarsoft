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


//Define Default Buffer Size for get/put arrays number 
#define MAX_DATA_BUFFER_SIZE  1024
#define DEF_DATA_BUFFER_SIZE  1024


#define RECORD_SEP ":'"


/*!
  Define pointers to the data and function of different types, all using the
  same memory location. This allows calling a void with variables types (e.g.
  using a switch statement)
*/
#define F_VECTOR_SELECTOR_DEFINITION( VS )  Vector_Selector *vs1,*vs2;	\
  if (VS != NULL) {							\
    if (VS->SelectorType()<=SEL_LIST) {vs1=VS;  vs2=NULL;}		\
    else {vs1 = NULL; vs2=VS;};} else {vs1=NULL; vs2=NULL;}



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

vector<String> split_str_into_vector (String str,
				      char c=',');

int string_find_chars (const String s,
		       const String c,
		       int &n,
		       const int npos=0);

int string_rfind_chars (const String s,
			const String c,
			int &n,
			const int npos=0);

String string_rfind_chars_substr (const String s,
				  const String c=RECORD_SEP,
				  const int npos=0);

void parse_record_name_to_vector (String name,
				  vector<String> &names,
				  vector<DIRECTION> &dirs,
				  String c=RECORD_SEP);

bool parse_record_selection_to_vector (String str,
				       String &name,
				       vector<String> &elems);

//End Utilities
//========================================================================






//------------------------------------------------------------------------
// object logic operators
//------------------------------------------------------------------------

template <class T>
bool object_logic_cmpr(Data* o_ptr, T val);

template <class T>
bool object_logic_in_set(Data* o_ptr, vector<T> &set);

// End object logic operators
//------------------------------------------------------------------------


#endif
