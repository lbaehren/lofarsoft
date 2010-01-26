/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// #define _DEBUG

#include "Pulsar/Generator.h"
#include "Pulsar/Parameters.h"

#ifdef HAVE_TEMPO2
#include "T2Generator.h"
#endif
#include "Predict.h"

#include "templates.h"
#include "FilePtr.h"
#include <string.h>

using namespace std;

template<class T>
vector<string> parsewords (T* data)
{
  FilePtr temp = tmpfile();
  if (!temp)
    throw Error (FailedSys, "words", "tmpfile");

  data->unload (temp);
  rewind (temp);

  vector<string> result;

  vector<char> buffer (FILENAME_MAX);
  char* line = &(buffer[0]);

  const char* whitespace = " \t\n";

  while (fgets (line, FILENAME_MAX, temp) != NULL)
  {
    char* eol = strchr (line, '#');
    if (eol)
      *eol = '\0';

    char* word = strtok (line, whitespace);
    while (word)
    {
      result.push_back (word);
      word = strtok (NULL, whitespace);
    }
  }

  return result;
}

Pulsar::Generator* Pulsar::Generator::factory (const Pulsar::Parameters* param)
{
#ifdef HAVE_TEMPO2

  DEBUG("Pulsar::Generator::factory tempo2 is an option");

  Pulsar::Generator* default_generator = get_default_generator();

  // if the default generator is a tempo2 generator, usea a tempo2 generator
  if (dynamic_cast<Tempo2::Generator*> (default_generator))
    return new Tempo2::Generator;

  DEBUG("Pulsar::Generator::factory tempo2 is not the default");

  // or if the parameters contain tempo2 keywords, use a tempo2 generator
  vector<string> words = parsewords (param);

  DEBUG("Pulsar::Generator::factory testing " << words.size() << " words");

  for (unsigned i=0; i < words.size(); i++)
  {
    DEBUG("Pulsar::Generator::factory testing '" << words[i] << "'");

    if (found (words[i], Tempo2::Generator::get_keywords()))
      return new Tempo2::Generator;
  }

  DEBUG("Pulsar::Generator::factory tempo2 is not required");

#endif

  return new Tempo::Predict;
}
