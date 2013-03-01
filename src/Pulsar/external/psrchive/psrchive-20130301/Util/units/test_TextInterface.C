/***************************************************************************
 *
 *   Copyright (C) 2004, 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "TextInterface.h"
#include "Functor.h"
#include "Alias.h"

#include <iostream>
using namespace std;

class extension : public Reference::Able
{
public:

  extension () 
  {
    text = "tui import failure";
  }
  extension (const extension& ext) 
  {
    text = ext.text;
  }
  ~extension () 
  {
  }

  void set_text (const std::string& _text) 
  {
    text = _text;
  }

  std::string get_text () const
  {
    return text;
  }

protected:
  std::string text;
};

class extensionTUI : public TextInterface::To<extension>
{
public:
  extensionTUI ()
  {
    add (&extension::get_text, &extension::set_text, "text");
  }
};

class tester : public Reference::Able
{
public:
  tester () { value = 0; }
  void set_value (double _value) { value = _value; }
  double get_value () const { return value; }
  extension* get_extension() { return &ext; }

  // test the map interface
  extension* get_map (string& text) { return &ext; }

  // test the element get/set interface
  void set_element (unsigned i, double _value) { element[i] = _value; }
  double get_element (unsigned i) const { return element[i]; }
  unsigned get_nelement () const { return 5; }

protected:
  extension ext;
  double value;
  double element [5];
};


class testerTUI : public TextInterface::To<tester>
{
public:
  testerTUI ()
  {
    add (&tester::get_value, "value", "description");
    add (&tester::get_value, "same",  "description");

    VGenerator<double> generator;
    add_value( generator("element", "an array of elements",
			 &tester::get_element,
			 &tester::set_element,
			 &tester::get_nelement) );
  }
};

class testerETUI : public testerTUI {

public:
  testerETUI () {
    add (Functor<double(const tester*)>(this, &testerETUI::mine), 
	 "mine", "my description");
  }
  double mine (const tester* t) { return t->get_value() * 2; }
};

class child : public tester {

public:
  void set_c (int _c) { c = _c; }
  int get_c () const { return c; }
protected:
  int c;
};

class childTUI : public TextInterface::To<child> {

public:
  childTUI () {
    import( testerTUI() );
    add (&child::get_c, "c", "child attribute");
    remove ("same");
  }

};


class tester_array : public Reference::Able {

public:
  tester_array (unsigned size) : array (size) { }
  vector<tester> array;

  unsigned size () const { return array.size(); }
  tester* element (unsigned i) { return &array[i]; }
};


int main () try
{
  tester Test;

  //
  // test the Allocator generator
  //
  TextInterface::Allocator<tester,double> allocate;

  TextInterface::Attribute<tester>* interface;
  interface = allocate ("value", &tester::get_value, &tester::set_value);

  //
  // test the Attribute interface
  //
  interface->set_value (&Test, "3.456");

  cerr << "tester::get_value=" << Test.get_value() << endl;

  if (Test.get_value() != 3.456)
  {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  //
  // test the read-only Attribute interface
  //
  TextInterface::Attribute<tester>* read_only;
  read_only = allocate ("value", &tester::get_value);

  cerr << "AttributeGet::get_value=" << read_only->get_value(&Test) << endl;

  if (read_only->get_value(&Test) != "3.456")
  {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  // should not be able to set a read-only value
  try {
    read_only->set_value (&Test, "0.789");

    cerr << "AttributeGet::set_value does not raise exception" << endl;
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
    
  }
  catch (Error& err) {
    cerr << "AttributeGet::set_value raises exception as expected" << endl;
  }

  testerTUI getset;
  getset.set_instance (&Test);

  cerr << "TextInterface::To<>::get_value="
       << getset.get_value("value") << endl;

  if (getset.get_value("value") != "3.456")
  {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  if (getset.get_value("value%2") != "3.5")
  {
    cerr <<
      "test_TextInterface ERROR precision:\n"
      "value;2 = " << getset.get_value("value%2") << endl;
    return -1;
  }

  if (getset.get_value("value") != "3.456")
  {
    cerr <<
      "test_TextInterface ERROR after precision:\n"
      "value = " << getset.get_value("value") << endl;
    return -1;
  }

  // test ElementGetSet all elements in the vector
  getset.set_value("element", "5.67");
  for (unsigned i=0; i<Test.get_nelement(); i++)
  {
    cerr << "Test.get_element(" << i << ")=" << Test.get_element(i) << endl;
    if (Test.get_element(i) != 5.67)
    {
      cerr << "test_TextInterface ERROR!" << endl;
      return -1;
    }
  }

  getset.set_value("element[3]", "4.32");
  for (unsigned i=0; i<Test.get_nelement(); i++)
  {
    cerr << "Test.get_element(" << i << ")=" << Test.get_element(i) << endl;
    if (i == 3 && Test.get_element(i) != 4.32)
    {
      cerr << "test_TextInterface ERROR!" << endl;
      return -1;
    }
    if (i != 3 && Test.get_element(i) != 5.67)
    {
      cerr << "test_TextInterface ERROR!" << endl;
      return -1;
    }
  }

  cerr << "testing import" << endl;

  getset.import ( "ext", extensionTUI(), &tester::get_extension );

  unsigned nvalue = getset.get_nvalue();

  cerr << "TextInterface::To<> has " << nvalue
       << " attributes after import" << endl;

  if (nvalue != 4) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  for (unsigned i=0; i < nvalue; i++)
    cerr << "  " << getset.get_name (i) << endl;

  std::string teststring = "test of TextInterface::import Component passed";
  std::string gotstring;

  getset.set_value ("ext:text", teststring);

  cerr << Test.get_extension()->get_text() << endl;

  if (Test.get_extension()->get_text() != teststring) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  if (getset.get_value("ext:text") != teststring) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  teststring = "test of TextInterface::import Element passed";
  Test.get_extension()->set_text (teststring);

  Alias alias;
  alias.add ("extext", "ext:text");

  getset.set_aliases (&alias);

  if (getset.get_value("extext") != teststring) {
    cerr << "test_TextInterface Alias ERROR!" << endl;
    return -1;
  }

  cerr << "testing Interface template" << endl;

  TextInterface::Value* value 
    = TextInterface::new_Interpreter ("extension", Test.get_extension(),
				      &extension::get_text,
				      &extension::set_text);

  teststring = "test of Interface::set_value";
  value->set_value (teststring);

  if (Test.get_extension()->get_text () != teststring) {
    cerr << "test_TextInterface Interpreter ERROR!" << endl;
    return -1;
  }

  tester_array Array (5);

  Array.array[3] = Test;

  TextInterface::To<tester_array> array_getset;
  array_getset.set_instance (&Array);
  array_getset.import ( "tester", &getset, 
			&tester_array::element,
			&tester_array::size );

  nvalue = array_getset.get_nvalue();

  cerr << "TextInterface::To<test_array> has " << nvalue 
       << " attributes after import" << endl;

  if (nvalue != 4) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }

  for (unsigned i=0; i < nvalue; i++)
    cerr << "  " << array_getset.get_name (i) << endl;

  gotstring = array_getset.get_value("tester[3]:ext:text");

  cerr << "get_value tester[3]:ext:text=" << gotstring<< endl;

  if (gotstring != teststring) {
    cerr << "test_TextInterface ERROR!" << endl;
    return -1;
  }


  array_getset.set_value ("tester[2]:ext:text", teststring);

  gotstring = Array.array[2].get_extension()->get_text();

  cerr << "tester_array[2].get_extension()->get_text=" << gotstring << endl;

  if (gotstring != teststring) 
  {
    cerr << "test_TextInterface ERROR! &(tester_array[2])=" 
	 << &(Array.array[2]) << endl;
    return -1;
  }

  cerr << "testing childTUI" << endl;

  childTUI child_tui;
  child ch;
  child_tui.set_instance( &ch );

  cerr << "calling childTUI::get_value" << endl;

  cerr << child_tui.get_value ("value") << endl;

  try {
    child_tui.get_value ("same");
    cerr << "childTUI should have thrown an exception" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "childTUI successfully removed 'same' attribute" << endl;
  }

  cerr << "testing import of map interface" << endl;

  getset.import ( "map", string(), extensionTUI(), &tester::get_map );

  cerr << getset.help(true) << endl;

  //
  // test nested_import
  //

  cerr << "testing nested import of another text interface" << endl;

  getset.insert ( "child", &child_tui );

  if ( getset.get_value ("child:value") != "0" )
  {
    cerr << "test_TextInterface ERROR! nested child:value="  
	 << getset.get_value ("child:value") << endl;
    return -1;
  }

  cerr << "test_TextInterface SUCCESS!" << endl;
  return 0;
}
catch (Error& error) {
  cerr << "test_TextInterface ERROR " << error << endl;
  return -1;
}


