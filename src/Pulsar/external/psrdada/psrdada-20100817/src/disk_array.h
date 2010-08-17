#ifndef __DADA_DISK_ARRAY_H
#define __DADA_DISK_ARRAY_H

/* ************************************************************************

   ************************************************************************ */

#include <pthread.h>
#include <sys/types.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct {

    dev_t device;
    char*  path;
    unsigned long f_bsize;    /* file system block size */

  } disk_t;

  typedef struct {
    
    disk_t*  disks;    /* disks to which data will be written */
    unsigned ndisk;    /* number of disks */
    uint64_t space;    /* number of bytes total */

    char overwrite;    /* when set, allow data file to be over-written */

    /* for multi-threaded use of the dbdisk struct */
    pthread_mutex_t mutex;
    
  } disk_array_t;

  /*! Create a new disk array */
  disk_array_t* disk_array_create ();

  /*! Destroy a disk array */
  int disk_array_destroy (disk_array_t*);

  /*! Add a disk to the array */
  int disk_array_add (disk_array_t*, char* path);

  /*! Enable/disable file overwriting */
  int disk_array_set_overwrite (disk_array_t* array, char value);

  /*! Open a file on the disk array, return the open file descriptor */
  int disk_array_open (disk_array_t*, char* name, uint64_t size, uint64_t* bs, int add_flags);

  /*! Close a file, reopen it and seek to the end of the file */
  int disk_array_reopen (disk_array_t* array, int curr_fd, char* filename);

  /*! Get the total amount of disk space */
  uint64_t disk_array_get_total (disk_array_t*);

  /*! Get the available amount of disk space */
  uint64_t disk_array_get_available (disk_array_t*);

#ifdef __cplusplus
}
#endif

#endif
