/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "dirutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_STATVFS_H
#include <sys/statvfs.h>
#endif

#ifdef HAVE_SYS_VFS_H
#include <sys/vfs.h>
#endif

#ifdef HAVE_SYS_MOUNT_H
#include <sys/param.h>
#include <sys/mount.h>
#endif

#include <dirent.h>

int file_exists (const char* filename)
{
  struct stat statistics;
  if (stat (filename, &statistics) < 0) {
    //    fprintf (stderr, "file_exists() error stat (%s)", filename);
    //perror ("");
    return 0;
  }
  return 1;
}

time_t file_mod_time (const char* filename)
{
  struct stat statistics;
  if (stat (filename, &statistics) < 0) {
    // fprintf (stderr, "ftime error stat (%s)", filename);
    // perror ("");
    return 0;
  }
  return statistics.st_mtime;
}

int file_is_directory (const char* filename)
{
  struct stat statistics;
  if (stat (filename, &statistics) < 0) {
    // fprintf (stderr, "file_is_directory error stat (%s)", filename);
    // perror ("");
    return 0;
  }
  return S_ISDIR(statistics.st_mode);
}

/* makedir - function to create a new directory, creating whatever path is
             needed along the way.  It uses the user's umask to set the mode
	     according to the individual's paranoia
 */

int makedir (const char* path)
{
  char fullpath [FILENAME_MAX];

  char * end_path_component = NULL;
  char * start_path_component = NULL;
  char endpath = 0;
  char * errmsg;
  mode_t dirmode = S_ISUID | S_ISGID   /* set user and group id */
                 | S_IRWXU | S_IRWXG | S_IRWXO; /* read write exec all */

  /* get the user's mask */
  mode_t old_mask = umask (0);
  /* put it back */
  umask (old_mask);

  /* mask out the bits */
  dirmode &= ~old_mask;

  if (path == NULL) {
    errno = EFAULT;
    return -1;
  }

  /* make a modifiable copy */
  strcpy (fullpath, path);

  end_path_component = fullpath;

  while (*end_path_component != '\0') {

    /* search for next non-/ character */
    while (*end_path_component == '/')
      end_path_component ++;

    /* mark the current position */
    start_path_component = end_path_component;
    /* search for a / or \0 */
    while (*end_path_component != '/') {
      if (*end_path_component == '\0') {
	endpath = 1;
	break;
      }
      end_path_component ++;
    }
    if (start_path_component == end_path_component) {
      errno = 0;
      return 0;
    }
    /* delimit the string */
    *end_path_component = '\0';
    if (mkdir (fullpath, dirmode) != 0) {
      if (errno != EEXIST) {
	errmsg = (char*) malloc (FILENAME_MAX + 80);
	sprintf (errmsg, "makedir: Could not create: '%s'\n", fullpath);
	perror (errmsg);
	free (errmsg);
	return -1;
      }
    }
    /* else - the directory component is created... continue. */
    if (endpath) {
      errno = 0;
      return 0;
    }
    /* put back the slash */
    *end_path_component = '/';
  }
  errno = 0;
  return 0;
}

/* this function removes a directory, including everything in it */
int removedir (const char *path)
{
  /* the length of the path string plus "/" plus "\0" */
  unsigned pathlength = strlen (path) + 2;

  /* the entry from the directory */
  struct dirent *entry;

  /* the complete path to the entry */
  char filename [FILENAME_MAX];

  /* handle to each sub-directory */
  DIR* subdird = 0;

  /* handle to directory at path */
  DIR* dird = opendir (path);

  if (dird == NULL) {
    fprintf (stderr, "removedir:: error opendir (%s)",path);
    perror ("");
    return -1;
  }

  entry = readdir (dird);

  while (entry != NULL) {

    if (strcmp (entry->d_name, ".")==0 || strcmp (entry->d_name, "..")==0) {
      entry = readdir (dird);
      continue;
    }

    if (strlen(entry->d_name) + pathlength > FILENAME_MAX) {
      fprintf (stderr, "removedir:: error filename too long: %s/%s\n",
	       path, entry->d_name);
      return -1;
    }

    sprintf (filename, "%s/%s", path, entry->d_name);

    subdird = opendir (filename);
    if (subdird) {
      closedir (subdird);

      if (removedir (filename) < 0)
	return -1;
    }
    else {
      if (unlink (filename) < 0) {
	fprintf (stderr, "removedir:: error unlink (%s)", filename);
	return -1;
      }
    }

    entry = readdir (dird);
  }

  closedir (dird);

  if (rmdir (path) < 0) {
    fprintf (stderr, "removedir:: error rmdir (%s)", path);
    return -1;
  }

  return 0;
}


/* returns the disk space in bytes */
int dirspace (const char* path, double* space)
{

#ifdef HAVE_SYS_STATVFS_H
  struct statvfs info;
  int error = statvfs (path, &info);
#else
  struct statfs info;
  int error = statfs (path, &info);
#endif

  if (error == -1)
    return (-1);

  *space = (double) info.f_bavail * (double) info.f_bsize;

  return (0);
}

/* for backward compatibility */
int disk_free_space (const char* path, double* space)
{
  return dirspace (path, space);
}

int diruse (const char *path, double* bytes)
{
  DIR*          dird;
  struct dirent *entry;
  struct stat   statistics;
  char          filename [512];

  dird = opendir (path);
  if (dird == NULL) {
    printf ("diruse:: error opendir (%s)",path);
    perror ("");
    return -1;
  }
  entry = readdir (dird);
  *bytes = 0;
  while (entry != NULL) {
    sprintf (filename, "%s/%s", path, entry->d_name);
    
    if (stat (filename, &statistics) < 0) {
      printf ("diruse:: error stat (%s)",filename);
      perror ("");
    }
    *bytes += statistics.st_size;  /* st_size in bytes */
    entry = readdir (dird);
  }
  closedir (dird);
  return 0;
}

/* **********************************************************************
   getoldest

   finds the oldest filename in 'path' ending in 'extension'.

   filename  - return value
   path      - the directory to search
   extension - if specified (ie. not NULL), limits matches to only those
               files with this extension
   time      - if not NULL, the time of the oldest file found is returned
               here

   RETURN VALUES:
     diroldest returns the number of files found in path.
     (that match extension, if specified).  If this number is 0, the
     value returned in 'filename' (or time, if not NULL) is left undefined.
     If this number is -1, an error occured.

   ********************************************************************** */
int diroldest (char* filename, 
	       const char* path, const char* extension,
	       time_t* time)
{
  DIR*          dird;
  struct dirent *entry;
  struct stat   statistics;
  int           found = 0;

  static char* temp_fname = NULL;
  time_t oldest_filetime = 0;

  char* ext = NULL;

  dird = opendir (path);
  if (dird == NULL) {
    fprintf (stderr, "diroldest: error opendir (%s) ::", path);
    perror ("");
    return -1;
  }

  if (temp_fname == NULL)
    temp_fname = (char*) malloc (FILENAME_MAX);

  while ((entry = readdir (dird)) != NULL) {

    if (extension) {
      ext = strstr (entry->d_name, extension);
      if (ext == NULL)
	continue;

      if (strcmp (ext, extension) != 0)
	continue;  /* want exact matches */
    }

    sprintf (temp_fname, "%s/%s", path, entry->d_name);

    if (stat (temp_fname, &statistics) < 0) {
      printf ("diroldest: error stat (%s)::\n", temp_fname);
      perror ("");
      break;
    }

    if (!found || (statistics.st_mtime < oldest_filetime)) {
      oldest_filetime = statistics.st_mtime;    
      /* Time of last data modification */
      strcpy (filename, temp_fname);
    }
    found ++;
  }
  closedir (dird);
  if (time)
    *time = oldest_filetime;
  return found;
}
