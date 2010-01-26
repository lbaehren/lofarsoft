#include "pad.h"

using namespace std;

string pad (unsigned want_length, const string& text, bool right)
{
  unsigned have_length = text.length();

  if (have_length >= want_length)
    return text;

  string pad (want_length - have_length, ' ');

  if (right)
    return text + pad;
  else
    return pad + text;
}
