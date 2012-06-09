/***************************************************************************
 *
 *   Copyright (C) 2000 by Russell Edwards
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <stdio.h>
#include <string>
#include <stdarg.h>

std::string
stringprintf (const char *fmt ...)
{
  char buf[1024];
  va_list args;

  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);

  return std::string(buf);
}

