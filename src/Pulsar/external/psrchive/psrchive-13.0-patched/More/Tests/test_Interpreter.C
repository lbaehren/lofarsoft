/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  When the Interpreter class is constructed, an Error exception will be
  thrown if any of the command names or shortcut keys are duplicated.
*/

#include "Pulsar/Interpreter.h"

int main () try {

  Pulsar::Interpreter test;
  std::cerr << "Pulsar::Interpreter test passed" << std::endl;
  return 0;

}
catch (Error& error) {
  std::cerr << "Pulsar::Interpreter " << error.get_message() << std::endl;
  return -1;
}
