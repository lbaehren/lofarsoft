/***************************************************************************
 *
 *   Copyright (C) 1999-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "strutil.h"

#include <string.h>
#include <ctype.h>

using namespace std;

// //////////////////////////////////////////////////////////////////
// stringload
// fills a string with characters from the stream described by FILE*
// until nbytes are read, or an EOF condition arises.
// return value is number of characters read (not incl. \0) or -1 on
// error
// //////////////////////////////////////////////////////////////////

static const unsigned rdsize = 256;
static char* rdline = NULL;

ssize_t stringload (string* str, FILE* fptr, size_t nbytes)
{
  if (fptr == NULL || str == NULL)
    return -1;

  if (rdline == NULL) rdline = new char [rdsize+1];
  size_t eachread = rdsize;
  size_t bytesread = 0;
  while (!nbytes || bytesread<nbytes) {
    if (nbytes && eachread > (nbytes-bytesread))
      eachread = nbytes-bytesread;
    size_t bytes = fread (rdline, 1, eachread, fptr);
    if (!bytes)
      break;
    rdline [bytes] = '\0';
    *str += rdline;
    bytesread += bytes;
  }
  if (ferror (fptr))  {
    perror ("stringload: fread failed");
    return -1;
  }
  return bytesread;
}

ssize_t stringload (string* str, istream &istr, streamsize nbytes)
{
  if (!istr || !str)
    return -1;

  if (rdline == NULL) rdline = new char [rdsize];
  streamsize eachread  = rdsize - 1;
  streamsize bytestoread = nbytes;

  while (istr) {
    if (nbytes && eachread > bytestoread)
      eachread = bytestoread;

    // istream::get() reads to the first \n char and terminates with a \0
    istr.get (rdline, eachread);
    streamsize bytes = istr.gcount();

    *str += rdline;

    if (nbytes) {
      bytestoread -= bytes;
      if (!bytestoread)
	break;
    }

    if (istr.get() == '\n')
      *str += "\n";
  }
  if (istr.bad())
    return -1;
  return 0;
}

// //////////////////////////////////////////////////////////////////
// stringload
// fills a vector of string with the first word from each line
// in the file.  A line is delimited by \n or commented by #.
// //////////////////////////////////////////////////////////////////

int stringfload (vector<string>* lines, const string& filename)
{
  if (filename == "-")
    return stringload (lines, stdin);

  FILE* fptr = fopen (filename.c_str(), "r");
  if (fptr == NULL)
    return -1;

  int ret = stringload (lines, fptr);
  fclose (fptr);
  return ret;
}

int stringload (vector<string>* lines, FILE* fptr)
{
  static char* rdline = NULL;
  if (!rdline) rdline = new char [FILENAME_MAX+1];

  const char* whitespace = " \t\n";
  // load the lines from file
  while (fgets (rdline, FILENAME_MAX, fptr) != NULL)
  {
    char* eol = strchr (rdline, '#');
    if (eol)
      *eol = '\0';
    char* line = strtok (rdline, whitespace);

    // if the string is not printable, then fptr is likely binary
    if (!printable (rdline))
      return -1;

    if (line)
      lines->push_back (string(line));
  }
  return 0;
}

#include <fstream>
#include "Error.h"

void loadlines (std::istream& input, std::vector<std::string>& lines)
{
  std::string line;

  while (input)
  {
    std::getline (input, line);
    line = stringtok (line, "#\n", false);  // get rid of comments

    if (!line.length())
      continue;

    lines.push_back (line);
  }
}

void loadlines (const std::string& filename, std::vector<std::string>& lines)
{
  if (filename == "-")
    loadlines (cin, lines);
  else
  {
    std::ifstream input (filename.c_str());
    if (!input)
      throw Error (FailedSys, "loadlines", "ifstream (" + filename + ")");
    
    loadlines (input, lines);
  }
}

