#include "string_array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>

string_array_t* string_array_create ()
{
  string_array_t* list = (string_array_t*) malloc (sizeof(string_array_t));
  assert (list != 0);

  list->size = 0;
  list->strs = NULL;
  return list;
}

void string_array_free (string_array_t* list)
{
   unsigned i;
   for (i=0; i < list->size; i++)
      free(list->strs[i]);
   free (list->strs);
   free (list);
}

/*! Return the requested string */
char* string_array_get (string_array_t* list, unsigned pos)
{
  if (pos < list->size)
    return list->strs[pos];
  return 0;
}

/*! Return the size of the array */
unsigned string_array_size (string_array_t* list)
{
  return list->size;
}

/*! All empty strings and any text following the # symbol will be ignored. */
int string_array_load (string_array_t* list, const char* filename)
{
  const char* whitespace = " \r\t\n";
  char* text = 0;

  unsigned buffer_size = 1024;
  static char* buffer = 0;
  FILE* fptr = fopen (filename, "r");

  if (!fptr) {
    fprintf (stderr, "string_array_load: error fopen(%s) %s\n",
	     filename, strerror(errno));
    return -1;
  }

  if (!buffer)
    buffer = malloc (buffer_size);
  assert (buffer != 0);

  while (fgets (buffer, buffer_size, fptr)) {

    /* truncate string at first # */
    text = strchr (buffer, '#');
    if (text)
      *text = '\0';

    /* skip leading whitespace */
    text = buffer;
    while (*text && strchr (whitespace, *text))
      text ++;

    if (*text != '\0')
      string_array_append (list, text);

  }

  return 0;
}

/*! This function will insert the string "entry" into the array of
   string_array referenced by "list".  It will be placed into the array
   at the index "pos" (starting at zero).  If a string already
   exists at this position, all entries will be bumped toward the end of
   the array.  If pos > list->size_entries, -1 will be returned.
 */
int string_array_insert (string_array_t* list, const char* entry, unsigned pos)
{
  char** temp;
  unsigned val;

  if (pos > list->size)
    return -1;

  temp = (char**) realloc (list->strs, (list->size+1)*sizeof(char**));
  assert (temp != 0);

  list->strs = temp;
  for (val = list->size; val > pos; val--)
    list->strs[val] = list->strs[val-1];
  list->strs[pos] = strdup (entry);
  assert (list->strs[pos] != 0);
  list->size ++;

  return 0;
}

int string_array_append (string_array_t* list, const char* entry)
{
  return (string_array_insert (list, entry, list->size));
}

int string_array_remove (string_array_t* list, unsigned pos)
{
  unsigned val;
  char ** temp;

  if (list->size == 0)
    return -1;

  if (pos >= list->size)
    return -1;

  free (list->strs[pos]);
  list->size --;

  for (val=pos; val<list->size; val++)
    list->strs[val] = list->strs[val+1];

  if (list->size == 0) {
    free (list->strs);
    list->strs = NULL;
    return 0;
  }

  temp = (char**) realloc (list->strs, (list->size)*sizeof(char**));
  assert (temp != 0);
  list->strs = temp;

  return 0;
}

int string_array_switch (string_array_t* list, unsigned pos1, unsigned pos2)
/* This function switches the two string_array in the array "list" indexed
   by "pos1" and "pos2".  */
{
  char* temp;
  temp = list->strs[pos1];
  list->strs[pos2] = list->strs[pos1];
  list->strs[pos1] = temp;
  return 0;
}

char* string_array_search (string_array_t* list, const char* match)
/* this function returns the string in "list" containing
   the string "match".  If the string does not exist in the array,
   NULL is returned.  */
{
  unsigned i;
  for (i=0; i < list->size; i++)
    if (strcmp(list->strs[i], match) == 0)
      return list->strs[i];
  return 0;
}

/* remove from list if word not found in filter */
int string_array_filter (string_array_t* list, const char* filter)
{
  unsigned istr = 0;
  for (istr=0; istr < string_array_size (list); istr++)
    if (!strstr (filter, string_array_get (list, istr)))
      string_array_remove (list, istr);
  return 0;
}

/* add tokens from string to list - input string is modified */
int string_array_tok (string_array_t* list, char* string, const char* white)
{
  char* word = 0;
  for (word = strtok (string, white); word != 0; word = strtok (NULL, white))
    if (!string_array_search (list, word))
      string_array_append (list, word);  
  return 0;
}
