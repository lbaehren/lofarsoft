/***************************************************************************
 *
 *   Copyright (C) 2002, 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifndef __CommandParser_h
#define __CommandParser_h

#include "Reference.h"
#include "Error.h"

#include <vector>
#include <string>
#include <iostream>

class CommandParser : public Reference::Able {

 public:

  //! verbose output from CommandParser methods
  static bool debug;

  //! the prompt shown to the user
  std::string prompt;

  //! null constructor
  CommandParser ();

  //! destructor
  ~CommandParser ();

  //! Initialize GNU readline command editor, enabling completion and history
  void initialize_readline (const char*);

  //! Print the prompt and read a line of text from the terminal
  std::string readline ();

  //! Process a script
  virtual void script (const std::string& filename);

  //! Process a vector of commands
  virtual void script (const std::vector<std::string>& commands);

  //! return a help string
  std::string help (const std::string& command = "");

  //! parse a command and arguments in one string
  std::string parse (const std::string& commandargs);

  //! parse a command and its arguments
  std::string parse2 (const std::string& command, const std::string& args);

  //! implements 'if (condition) command'
  std::string if_command (const std::string& condition_command);

  //! implements 'while (condition) command'
  std::string while_command (const std::string& condition_command);

  //! evaluate a boolean expression
  virtual bool evaluate (const std::string& expression);

  //! string returned on an empty command
  virtual std::string empty ();

  //! quit flag raised by "quit"
  bool quit;

  //! verbose operation enabled by "verbose"
  bool verbose;

  //! abort on exception flag set by "abort" command
  bool abort;

  //! Pure virtual base class of interface to parser methods
  class Method;

 protected:

  //! Flag raised whenever a parsing error or other fault occurs
  bool fault;

  //! Name of command parser when nested
  std::string nested;

  //! Derived classes add commands to the list using this method
  template <class Parser>
    void add_command (std::string (Parser::*method)(const std::string&),
		      const std::string& command,
		      const std::string& help,
		      const std::string& detailed_help = "",
		      char shortcut = 0);

  //! Derived classes add commands with shortcut keys using this method
  template <class Parser>
    void add_command (std::string (Parser::*method)(const std::string&),
		      char shortcut,
		      const std::string& command,
		      const std::string& help,
		      const std::string& detailed_help = "")
    { add_command (method, command, help, detailed_help, shortcut); }

  //! Copy the commands from another CommandParser
  void import (CommandParser*);

  //! Import a nested CommandParser
  void import (CommandParser*,
               const std::string& command,
               const std::string& help,
               char shortcut = 0);

  //! Nested CommandParser Method implementation
  class Nested;

  //! Add Method instance
  void add_command (Method*);

  //! Get the name of the current command
  std::string get_command () const { return current_command; }

  //! Get the help string of the current command
  std::string usage () { return help(current_command); }

  //! Separate text into condition and command
  void conditional (const std::string& text,
		    std::string& condition,
		    std::string& command);

 private:

  //! Available commands
  std::vector<Method*> commands;

  //! the current command
  std::string current_command;

  //! process 'init' and 'end' commands
  bool startCommand, endCommand;

  // readline interface
  static char** completion (const char *text, int start, int end);
  
  // readline interface
  static char* command_generator (const char* text, int state);

};

//! Pure virtual base class of the template class Command
class CommandParser::Method {
 public:
  Method() {}
  virtual ~Method () {}

  virtual std::string execute (const std::string& command) = 0;

  virtual std::string detail () const = 0;

  //! The command string corresponding to this method
  std::string command;
  //! The help string for this method
  std::string help;
  //! The shortcut character corresponding to this method
  char shortcut;
};

//! Nested CommandParser Method implementation
class CommandParser::Nested : public Method {
 public:
  Nested( CommandParser*,
	  const std::string& command,
	  const std::string& help,
	  char shortcut );
  std::string execute (const std::string& command);
  std::string detail () const;
 protected:
  Reference::To<CommandParser, false> parser;
};

//! Stores a pointer to a CommandParser sub-class and one of its methods
template <class Parser> class Command : public CommandParser::Method 
{
  friend class CommandParser;
  
  typedef std::string (Parser::*Method) (const std::string&);
  
 public:
  
  Command (Parser* _instance, Method _method, const std::string& _command,
	   const std::string& _help, const std::string& _detailed_help,
	   char _shortcut)
    {
      command  = _command;
      help     = _help;
      shortcut = _shortcut;

      detailed_help = _detailed_help;
      instance      = _instance;
      method        = _method;
    }

  //! Execute method
  std::string execute (const std::string& args)
    { return (instance->*method) (args); }

  std::string detail () const
    { return detailed_help; }

 protected:
  //! Method of the sub-class to execute
  Method method;

  //! Instance through which method is called
  Parser* instance;

  //! The detailed help string for this method
  std::string detailed_help;
};

//! derived types may add commands to the list using this method
template<class P>
void CommandParser::add_command (std::string (P::*method) (const std::string&),
				 const std::string& cmd,
				 const std::string& help, 
				 const std::string& detailed_help,
				 char shortcut)
{
  if (debug)
    std::cerr << "CommandParser::add_command \"" << cmd << "\"" << std::endl;

  P* instance = dynamic_cast<P*> (this);
  if (!instance)
    throw Error (InvalidState, "CommandParser::add_command",
		 "instance/method mis-match");

  if (debug)
    std::cerr << "CommandParser::add_command new Command<P>" << std::endl;

  add_command (new Command<P> 
	       (instance, method, cmd, help, detailed_help, shortcut));
}


#endif
