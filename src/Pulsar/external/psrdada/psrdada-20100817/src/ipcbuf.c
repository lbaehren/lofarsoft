#include "ipcbuf.h"
#include "ipcutil.h"
#include <time.h>

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

/* #define _DEBUG 1 */

/* semaphores */

#define IPCBUF_WRITE  0   /* semaphore locks writer status */
#define IPCBUF_READ   1   /* semaphore locks reader (+clear) status */

#define IPCBUF_SODACK 2   /* acknowledgement of start of data */
#define IPCBUF_EODACK 3   /* acknowledgement of end of data */

#define IPCBUF_FULL   4   /* semaphore counts full buffers */
#define IPCBUF_CLEAR  5   /* semaphore counts emptied buffers */

#define IPCBUF_NSEM   6   /* total number of semaphores */

/* process states */

#define IPCBUF_DISCON  0  /* disconnected */
#define IPCBUF_VIEWER  1  /* connected */

#define IPCBUF_WRITER  2  /* one process that writes to the buffer */
#define IPCBUF_WRITING 3  /* start-of-data flag has been raised */
#define IPCBUF_WCHANGE 4  /* next operation will change writing state */

#define IPCBUF_READER  5  /* one process that reads from the buffer */
#define IPCBUF_READING 6  /* start-of-data flag has been raised */
#define IPCBUF_RSTOP   7  /* end-of-data flag has been raised */

#define IPCBUF_VIEWING 8  /* currently viewing */
#define IPCBUF_VSTOP   9  /* end-of-data while viewer */

static void fsleep (double seconds)
{
  struct timeval t ;
  
  t.tv_sec = seconds;
  seconds -= t.tv_sec;
  t.tv_usec = seconds * 1e6;
  select (0, 0, 0, 0, &t) ;
}

/* *************************************************************** */
/*!
  creates the shared memory block that may be used as an ipcsync_t struct

  \param key the shared memory key
  \param flag the flags to pass to shmget
*/
int ipcsync_get (ipcbuf_t* id, key_t key, uint64_t nbufs, int flag)
{
  int required = sizeof(ipcsync_t) + nbufs + sizeof(key_t) * nbufs;

  if (!id)
  {
    fprintf (stderr, "ipcsync_get: invalid ipcbuf_t*\n");
    return -1;
  }

  id->sync = ipc_alloc (key, required, flag, &(id->syncid));
  if (id->sync == 0)
  {
    fprintf (stderr, "ipcsync_get: ipc_alloc error\n");
    return -1;
  }

  if (nbufs == 0)
    nbufs = id->sync->nbufs;

  id->count = (char*) (id->sync + 1);

#ifdef _DEBUG
  fprintf (stderr, "SYNC=%p COUNT=%p\n", id->sync, id->count);
#endif

  id->shmkey = (key_t*) (id->count + nbufs);
  id->state = 0;
  id->viewbuf = 0;

  return 0;
}

int ipcbuf_get (ipcbuf_t* id, int flag)
{
  int retval = 0;
  ipcsync_t* sync = 0;
  uint ibuf = 0;

  if (!id)
  {
    fprintf (stderr, "ipcbuf_get: invalid ipcbuf_t*\n");
    return -1;
  }

  sync = id->sync;

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_get: semkey=0x%x shmkey=0x%x\n",
                   sync->semkey, id->shmkey[0]);
#endif

  /* all semaphores are created in this id */
  id->semid = semget (sync->semkey, IPCBUF_NSEM, flag);

  if (id->semid < 0)
  {
    fprintf (stderr, "ipcbuf_get: semget(0x%x, %d, 0x%x): %s\n",
                     sync->semkey, IPCBUF_NSEM, flag, strerror(errno));
    retval = -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_get: semid=%d\n", id->semid);
#endif

  id->buffer = (char**) malloc (sizeof(char*) * sync->nbufs);
  assert (id->buffer != 0);
  id->shmid = (int*) malloc (sizeof(int) * sync->nbufs);
  assert (id->shmid != 0);

  for (ibuf=0; ibuf < sync->nbufs; ibuf++)
  {
    id->buffer[ibuf] = ipc_alloc (id->shmkey[ibuf], sync->bufsz, 
          flag, id->shmid + ibuf);

    if ( id->buffer[ibuf] == 0 )
    {
      fprintf (stderr, "ipcbuf_get: ipc_alloc buffer[%u] %s\n",
                       ibuf, strerror(errno));
      retval = -1;
      break;
    }
  }

  return retval;
}


/* *************************************************************** */
/*!
  Creates a new ring buffer in shared memory

  \return pointer to a new ipcbuf_t ring buffer struct
  \param nbufs
  \param bufsz
  \param key
*/

/* start with some random key for all of the pieces */
static int key_increment  = 0x00010000;

int ipcbuf_create (ipcbuf_t* id, key_t key, uint64_t nbufs, uint64_t bufsz)
{
  uint64_t ibuf = 0;
  int flag = IPCUTIL_PERM | IPC_CREAT | IPC_EXCL;

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_create: key=%d nbufs=%"PRIu64" bufsz=%"PRIu64"\n",
                   key, nbufs, bufsz);
#endif

  if (ipcsync_get (id, key, nbufs, flag) < 0)
  {
    fprintf (stderr, "ipcbuf_create: ipcsync_get error\n");
    return -1;
  }

  id->sync->nbufs  = nbufs;
  id->sync->bufsz  = bufsz;

  for (ibuf = 0; ibuf < IPCBUF_XFERS; ibuf++)
  {
    id->sync->s_buf  [ibuf] = 0;
    id->sync->s_byte [ibuf] = 0;
    id->sync->e_buf  [ibuf] = 0;
    id->sync->e_byte [ibuf] = 0;
    id->sync->eod    [ibuf] = 1;
  }

  key += key_increment;
  id->sync->semkey = key;

  for (ibuf = 0; ibuf < nbufs; ibuf++)
  {
    id->count[ibuf] = 0;
    key += key_increment;
    id->shmkey[ibuf] = key;
  }

  id->sync->r_buf = 0;
  id->sync->w_buf = 0;
  id->sync->r_xfer = 0;
  id->sync->w_xfer = 0;

  id->sync->r_state = 0;
  id->sync->w_state = 0;

  id->buffer      = 0;
  id->viewbuf     = 0;
  id->xfer        = 0;
  id->soclock_buf = 0;

  if (ipcbuf_get (id, flag) < 0)
  {
    fprintf (stderr, "ipcbuf_create: ipcbuf_get error\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_create: syncid=%d semid=%d\n",
                   id->syncid, id->semid);
#endif

  /* ready to be locked by writer and reader processes */
  if (ipc_semop (id->semid, IPCBUF_WRITE, 1, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_create: error incrementing IPCBUF_WRITE\n");
    return -1;
  }
  if (ipc_semop (id->semid, IPCBUF_READ, 1, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_create: error incrementing IPCBUF_READ\n");
    return -1;
  }

  /* ready for writer to decrement when it needs to set SOD/EOD */
  if (ipc_semop (id->semid, IPCBUF_SODACK, IPCBUF_XFERS, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_create: error incrementing IPCBUF_SODACK\n");
    return -1;
  }
  if (ipc_semop (id->semid, IPCBUF_EODACK, IPCBUF_XFERS, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_create: error incrementing IPCBUF_EODACK\n");
    return -1;
  }

  id->state = IPCBUF_VIEWER;
  return 0;
}

int ipcbuf_connect (ipcbuf_t* id, key_t key)
{
  int flag = IPCUTIL_PERM;

  if (ipcsync_get (id, key, 0, flag) < 0)
  {
    fprintf (stderr, "ipcbuf_connect: ipcsync_get error\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_connect: key=0x%x nbufs=%"PRIu64" bufsz=%"PRIu64"\n",
                    key, id->sync->nbufs, id->sync->bufsz);
#endif

  id->buffer = 0;

  if (ipcbuf_get (id, flag) < 0)
  {
    fprintf (stderr, "ipcbuf_connect: ipcbuf_get error\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_connect: syncid=%d semid=%d\n",
                   id->syncid, id->semid);
#endif

  id->state = IPCBUF_VIEWER;
  return 0;
}


int ipcbuf_disconnect (ipcbuf_t* id)
{
  uint64_t ibuf = 0;

  if (!id)
  {
    fprintf (stderr, "ipcbuf_disconnect: invalid ipcbuf_t\n");
    return -1;
  }

  for (ibuf = 0; ibuf < id->sync->nbufs; ibuf++)
    if (id->buffer[ibuf] && shmdt (id->buffer[ibuf]) < 0)
      perror ("ipcbuf_disconnect: shmdt(buffer)");

  if (id->buffer) free (id->buffer); id->buffer = 0;
  if (id->shmid) free (id->shmid); id->shmid = 0;

  if (id->sync && shmdt (id->sync) < 0)
    perror ("ipcbuf_disconnect: shmdt(sync)");

  id->sync = 0;

  id->state = IPCBUF_DISCON; 

  return 0;
}

int ipcbuf_destroy (ipcbuf_t* id)
{
  uint64_t ibuf = 0;

  if (!id)
  {
    fprintf (stderr, "ipcbuf_destroy: invalid ipcbuf_t\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_destroy: semid=%d\n", id->semid);
#endif

  if (id->semid>-1 && semctl (id->semid, 0, IPC_RMID) < 0)
    perror ("ipcbuf_destroy: semctl");
  id->semid = -1;

  for (ibuf = 0; ibuf < id->sync->nbufs; ibuf++)
  {
#ifdef _DEBUG
    fprintf (stderr, "ipcbuf_destroy: id[%"PRIu64"]=%x\n",
             ibuf, id->shmid[ibuf]);
#endif

    if (id->buffer)
      id->buffer[ibuf] = 0;

    if (id->shmid[ibuf]>-1 && shmctl (id->shmid[ibuf], IPC_RMID, 0) < 0)
      perror ("ipcbuf_destroy: buf shmctl");

  }

  if (id->buffer) free (id->buffer); id->buffer = 0;
  if (id->shmid) free (id->shmid); id->shmid = 0;

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_destroy: syncid=%d\n", id->syncid);
#endif

  if (id->syncid>-1 && shmctl (id->syncid, IPC_RMID, 0) < 0)
    perror ("ipcbuf_destroy: sync shmctl");

  id->sync = 0;
  id->syncid = -1;

  return 0;
}

/*! Lock this process in as the designated writer */
int ipcbuf_lock_write (ipcbuf_t* id)
{
  if (id->state != IPCBUF_VIEWER)
  {
    fprintf (stderr, "ipcbuf_lock_write: not connected\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_lock_write: decrement WRITE=%d\n",
                   semctl (id->semid, IPCBUF_WRITE, GETVAL));
#endif

  /* decrement the write semaphore (only one can) */
  if (ipc_semop (id->semid, IPCBUF_WRITE, -1, SEM_UNDO) < 0)
  {
    fprintf (stderr, "ipcbuf_lock_write: error decrement IPCBUF_WRITE\n");
    return -1;
  }

  /* WCHANGE is a special state that means the process will change into the
     WRITING state on the first call to get_next_write */

  if (id->sync->w_state == 0)
    id->state = IPCBUF_WCHANGE;
  else
    id->state = IPCBUF_WRITING;

  id->xfer = id->sync->w_xfer % IPCBUF_XFERS;

  return 0;
}

int ipcbuf_unlock_write (ipcbuf_t* id)
{
  if (!ipcbuf_is_writer (id))
  {
    fprintf (stderr, "ipcbuf_unlock_write: state != WRITER\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_unlock_write: increment WRITE=%d\n",
                   semctl (id->semid, IPCBUF_WRITE, GETVAL));
#endif

  if (ipc_semop (id->semid, IPCBUF_WRITE, 1, SEM_UNDO) < 0)
  {
    fprintf (stderr, "ipcbuf_unlock_write: error increment IPCBUF_WRITE\n");
    return -1;
  }

  id->state = IPCBUF_VIEWER;

  return 0;
}

char ipcbuf_is_writing (ipcbuf_t* id)
{
  return id->state == IPCBUF_WRITING;
}
 

int ipcbuf_enable_eod (ipcbuf_t* id)
{
  /* must be the designated writer */
  if (id->state != IPCBUF_WRITING)
  {
    fprintf (stderr, "ipcbuf_enable_eod: not writing\n");
    return -1;
  }

  id->state = IPCBUF_WCHANGE;

  return 0;
}

int ipcbuf_disable_sod (ipcbuf_t* id)
{
  /* must be the designated writer */
  if (id->state != IPCBUF_WCHANGE)
  {
    fprintf (stderr, "ipcbuf_disable_sod: not able to change writing state\n");
    return -1;
  }

  id->state = IPCBUF_WRITER;

  return 0;
}

uint64_t ipcbuf_get_sod_minbuf (ipcbuf_t* id)
{
  ipcsync_t* sync = id->sync;

  /* Since we may have multiple transfers, the minimum sod will be relative
   * to the first buffer we clocked data onto */
  uint64_t new_bufs_written = sync->w_buf - id->soclock_buf;

  if (new_bufs_written < sync->nbufs) 
    return id->soclock_buf;
  else
    return sync->w_buf - sync->nbufs + 1;
    
}

  /* start buf should be the buffer to begin on, and should be aware 
   * of previously filled bufs, filled within the same observation
   * or from a previous observation */

int ipcbuf_enable_sod (ipcbuf_t* id, uint64_t start_buf, uint64_t start_byte)
{
  ipcsync_t* sync = id->sync;
  uint64_t new_bufs = 0;
  uint64_t bufnum = 0;

  /* must be the designated writer */
  if (id->state != IPCBUF_WRITER && id->state != IPCBUF_WCHANGE)
  {
    fprintf (stderr, "ipcbuf_enable_sod: not writer state=%d\n", id->state);
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_enable_sod: start buf=%"PRIu64" w_buf=%"PRIu64"\n",
                   start_buf, sync->w_buf);
#endif

  /* start_buf must be less than or equal to the number of buffers written */
  if (start_buf > sync->w_buf)
  {
    fprintf (stderr,
       "ipcbuf_enable_sod: start_buf=%"PRIu64" > w_buf=%"PRIu64"\n",
       start_buf, sync->w_buf);
    return -1;
  }

  if (start_buf < ipcbuf_get_sod_minbuf (id))
  {
    fprintf (stderr,
       "ipcbuf_enable_sod: start_buf=%"PRIu64" < start_min=%"PRIu64"\n",
       start_buf, ipcbuf_get_sod_minbuf (id));
    return -1;
  }

  /* start_byte must be less than or equal to the size of the buffer */
  if (start_byte > sync->bufsz)
  {
    fprintf (stderr,
       "ipcbuf_enable_sod: start_byte=%"PRIu64" > bufsz=%"PRIu64"\n",
       start_byte, sync->bufsz);
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_enable_sod: decrement SODACK=%d\n",
                   semctl (id->semid, IPCBUF_SODACK, GETVAL));
#endif

  /* decrement the start-of-data acknowlegement semaphore */
  if (ipc_semop (id->semid, IPCBUF_SODACK, -1, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_enable_sod: error decrement SODACK\n");
    return -1;
  }

  id->xfer = sync->w_xfer % IPCBUF_XFERS;

  sync->s_buf  [id->xfer] = start_buf;
  sync->s_byte [id->xfer] = start_byte;

  /* changed by AJ to fix a bug where a reader is still reading this xfer
   * and the writer wants to start writing to it... */
  if (sync->w_buf == 0) 
    sync->eod [id->xfer] = 0;

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_enable_sod: xfer=%"PRIu64
	                 " start buf=%"PRIu64" byte=%"PRIu64"\n", sync->w_xfer,
                   sync->s_buf[id->xfer], sync->s_byte[id->xfer]);
#endif

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_enable_sod: w_buf=%"PRIu64"\n", sync->w_buf);
#endif

  for (new_bufs = sync->s_buf[id->xfer]; new_bufs < sync->w_buf; new_bufs++)
  {
    bufnum = new_bufs % sync->nbufs;
    id->count[bufnum] ++;
#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_enable_sod: count[%"PRIu64"]=%u\n",
                   bufnum, id->count[bufnum]);
#endif
  }

  new_bufs = sync->w_buf - sync->s_buf[id->xfer];

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_enable_sod: new_bufs=%"PRIu64"\n", new_bufs);
#endif

  id->state = IPCBUF_WRITING;
  id->sync->w_state = IPCBUF_WRITING;

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_enable_sod: increment FULL=%d by %"PRIu64"\n",
                   semctl (id->semid, IPCBUF_FULL, GETVAL), new_bufs);
#endif

  /* increment the buffers written semaphore */
  if (new_bufs && ipc_semop (id->semid, IPCBUF_FULL, new_bufs, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_enable_sod: error increment FULL\n");
    return -1;
  }

  return 0;
}

char ipcbuf_is_writer (ipcbuf_t* id)
{
  int who = id->state;
  return who==IPCBUF_WRITER || who==IPCBUF_WCHANGE || who==IPCBUF_WRITING;
}

char* ipcbuf_get_next_write (ipcbuf_t* id)
{
  uint64_t bufnum = 0;
  ipcsync_t* sync = id->sync;

  /* must be the designated writer */
  if (!ipcbuf_is_writer(id))
  {
    fprintf (stderr, "ipcbuf_get_next_write: process is not writer\n");
    return NULL;
  }

  if (id->state == IPCBUF_WCHANGE)
  {
#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_get_next_write: WCHANGE->WRITING enable_sod"
	                 " w_buf=%"PRIu64"\n", id->sync->w_buf);
#endif

    if (ipcbuf_enable_sod (id, id->sync->w_buf, 0) < 0)
    {
      fprintf (stderr, "ipcbuf_get_next_write: ipcbuf_enable_sod error\n");
      return NULL;
    }
  }

  bufnum = sync->w_buf % sync->nbufs;

  while (id->count[bufnum])
  {
#ifdef _DEBUG
    fprintf (stderr, "ipcbuf_get_next_write: count[%"PRIu64"]=%u\n",
                     bufnum, id->count[bufnum]);
    fprintf (stderr, "ipcbuf_get_next_write: decrement CLEAR=%d\n",
                     semctl (id->semid, IPCBUF_CLEAR, GETVAL));
#endif

    /* decrement the buffers read semaphore */
    if (ipc_semop (id->semid, IPCBUF_CLEAR, -1, 0) < 0)
    {
      fprintf (stderr, "ipcbuf_get_next_write: error decrement CLEAR\n");
      return NULL;
    }

    id->count[bufnum] --;
  }

  return id->buffer[bufnum];
}

int ipcbuf_mark_filled (ipcbuf_t* id, uint64_t nbytes)
{
  ipcsync_t* sync = 0;
  uint64_t bufnum = 0;

  /* must be the designated writer */
  if (!ipcbuf_is_writer(id))
  {
    fprintf (stderr, "ipcbuf_mark_filled: process is not writer\n");
    return -1;
  }

  /* increment the buffers written semaphore only if WRITING */
  if (id->state == IPCBUF_WRITER)
  {
    id->sync->w_buf ++;
    return 0;
  }

  sync = id->sync;

  if (id->state == IPCBUF_WCHANGE || nbytes < sync->bufsz)
  {
#ifdef _DEBUG
    if (id->state == IPCBUF_WCHANGE)
      fprintf (stderr, "ipcbuf_mark_filled: end xfer #%"PRIu64"->%"PRIu64"\n",
                       sync->w_xfer, id->xfer);
#endif

#ifdef _DEBUG
    fprintf (stderr, "ipcbuf_mark_filled: decrement EODACK=%d\n",
                     semctl (id->semid, IPCBUF_EODACK, GETVAL));
#endif
    
    if (ipc_semop (id->semid, IPCBUF_EODACK, -1, 0) < 0)
    {
      fprintf (stderr, "ipcbuf_mark_filled: error decrementing EODACK\n");
      return -1;
    }

    sync->e_buf  [id->xfer] = sync->w_buf;
    sync->e_byte [id->xfer] = nbytes;
    sync->eod    [id->xfer] = 1;

#ifdef _DEBUG
      fprintf (stderr, "ipcbuf_mark_filled:"
                       " end buf=%"PRIu64" byte=%"PRIu64"\n",
                       sync->e_buf[id->xfer], sync->e_byte[id->xfer]);
#endif

    sync->w_xfer++;
    id->xfer = sync->w_xfer % IPCBUF_XFERS;

    id->state = IPCBUF_WRITER;
    id->sync->w_state = 0;
  }

  bufnum = sync->w_buf % sync->nbufs;

  id->count[bufnum] ++;
  sync->w_buf ++;

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_mark_filled: count[%"PRIu64"]=%u w_buf=%"PRIu64"\n",
                   bufnum, id->count[bufnum], sync->w_buf);
  fprintf (stderr, "ipcbuf_mark_filled: increment FULL=%d\n",
                   semctl (id->semid, IPCBUF_FULL, GETVAL));
#endif
  
  if (ipc_semop (id->semid, IPCBUF_FULL, 1, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_mark_filled: error increment FULL\n");
    return -1;
  }
  
  return 0;
}

/*! Lock this process in as the designated reader */
int ipcbuf_lock_read (ipcbuf_t* id)
{
  if (id->state != IPCBUF_VIEWER)
  {
    fprintf (stderr, "ipcbuf_lock_read: not connected\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_lock_read: decrement READ=%d\n",
                   semctl (id->semid, IPCBUF_READ, GETVAL));
#endif

  /* decrement the read semaphore (only one can) */
  if (ipc_semop (id->semid, IPCBUF_READ, -1, SEM_UNDO) < 0)
  {
    fprintf (stderr, "ipcbuf_lock_read: error decrement READ\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_lock_read: reader status locked\n");
#endif

  if (id->sync->r_state == 0)
    id->state = IPCBUF_READER;
  else
    id->state = IPCBUF_READING;

  id->xfer = id->sync->r_xfer % IPCBUF_XFERS;

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_lock_read: xfer=%"PRIu64
           " start buf=%"PRIu64" byte=%"PRIu64"\n", id->sync->r_xfer,
           id->sync->s_buf[id->xfer], id->sync->s_byte[id->xfer]);
#endif

  return 0;
}

char ipcbuf_is_reader (ipcbuf_t* id)
{
  int who = id->state;
  return who==IPCBUF_READER || who==IPCBUF_READING || who==IPCBUF_RSTOP;
}

int ipcbuf_unlock_read (ipcbuf_t* id)
{
  if (!ipcbuf_is_reader(id))
  {
    fprintf (stderr, "ipcbuf_unlock_read: state != READER\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_unlock_read: increment READ=%d\n",
                   semctl (id->semid, IPCBUF_READ, GETVAL));
#endif

  if (ipc_semop (id->semid, IPCBUF_READ, 1, SEM_UNDO) < 0)
  {
    fprintf (stderr, "ipcbuf_unlock_read: error increment READ\n");
    return -1;
  }

  id->state = IPCBUF_VIEWER;
  return 0;
}

char* ipcbuf_get_next_read_work (ipcbuf_t* id, uint64_t* bytes, int flag)
{
  uint64_t bufnum;
  uint64_t start_byte = 0;
  ipcsync_t* sync = 0;

  if (ipcbuf_eod (id))
    return NULL;

  sync = id->sync;

  if (ipcbuf_is_reader (id))
  {

#ifdef _DEBUG
    fprintf (stderr, "ipcbuf_get_next_read: decrement FULL=%d\n", 
                     semctl (id->semid, IPCBUF_FULL, GETVAL));
#endif

    /* decrement the buffers written semaphore */
    if (ipc_semop (id->semid, IPCBUF_FULL, -1, flag) < 0) {
      fprintf (stderr, "ipcbuf_get_next_read: error decrement FULL\n");
      return NULL;
    }

    if (id->state == IPCBUF_READER)
    {
      id->xfer = sync->r_xfer % IPCBUF_XFERS;

#ifdef _DEBUG
      fprintf (stderr, "ipcbuf_get_next_read: xfer=%"PRIu64
	       " start buf=%"PRIu64" byte=%"PRIu64"\n", sync->r_xfer,
	       sync->s_buf[id->xfer], 
	       sync->s_byte[id->xfer]);
#endif

      id->state = IPCBUF_READING;
      id->sync->r_state = IPCBUF_READING;

      sync->r_buf = sync->s_buf[id->xfer];
      start_byte = sync->s_byte[id->xfer];

#ifdef _DEBUG
    fprintf (stderr, "ipcbuf_get_next_read: increment SODACK=%d\n",
                     semctl (id->semid, IPCBUF_SODACK, GETVAL));
#endif

      /* increment the start-of-data acknowlegement semaphore */
      if (ipc_semop (id->semid, IPCBUF_SODACK, 1, flag) < 0) {
        fprintf (stderr, "ipcbuf_get_next_read: error increment SODACK\n");
        return NULL;
      }

    }

    bufnum = sync->r_buf;

  }
  else
  {
#ifdef _DEBUG
    fprintf (stderr, "ipcbuf_get_next_read: not reader\n");
#endif
    if (id->state == IPCBUF_VIEWER)
    {
#ifdef _DEBUG
      fprintf (stderr, "ipcbuf_get_next_read: start viewing xfer=%"PRIu64
               " start buf=%"PRIu64" byte=%"PRIu64"\n", sync->r_xfer,
               sync->s_buf[id->xfer], sync->s_byte[id->xfer]);
#endif

      id->xfer = sync->r_xfer % IPCBUF_XFERS;
      id->state = IPCBUF_VIEWING;

      id->viewbuf = sync->s_buf[id->xfer];
      start_byte = sync->s_byte[id->xfer];

      /*
        In the following, the viewer seeks to the end of data.
        This is probably the best default behaviour for most monitors,
        which should present the most current data.

        It is also necessary for proper behaviour of the dada_hdu class.
        The header block is never closed; the xfer is always 0 and each
        new transfer is simply a step forward in the ring buffer.
        Therefore, if a viewer always takes the first buffer of the
        current transfer (as above), then it will be reading out-of-date
        header information.  The following lines correct this problem.

        Willem van Straten - 22 Oct 2008
      */
      if (sync->w_buf > id->viewbuf + 1)
      {
#ifdef _DEBUG
        fprintf (stderr, "ipcbuf_get_next_read: viewer seek to end of data\n");
#endif
        id->viewbuf = sync->w_buf - 1;
        start_byte = 0;
      }
    }

    /* Viewers wait until w_buf is incremented without semaphore operations */
    while (sync->w_buf <= id->viewbuf)
    {
      if (sync->eod[id->xfer] && sync->r_buf == sync->e_buf[id->xfer])
      {
        id->state = IPCBUF_VSTOP;
        break;
      }

      fsleep (0.1);
    }

    if (id->viewbuf + sync->nbufs < sync->w_buf)
      id->viewbuf = sync->w_buf - sync->nbufs + 1;

    bufnum = id->viewbuf;
    id->viewbuf ++;
#ifdef _DEBUG
    fprintf (stderr, "ipcbuf_get_next_read: bufnum=%"PRIu64"\n", bufnum);
#endif
  }

  bufnum %= sync->nbufs;

  if (bytes)
  {
    if (sync->eod[id->xfer] && sync->r_buf == sync->e_buf[id->xfer])
    {
#ifdef _DEBUG
      fprintf (stderr, "ipcbuf_get_next_read xfer=%d EOD=true and r_buf="
                       "%"PRIu64" == e_buf=%"PRIu64"\n", (int)id->xfer, 
                       sync->r_buf, sync->e_buf[id->xfer]);
#endif
      *bytes = sync->e_byte[id->xfer] - start_byte;
    }
    else
      *bytes = sync->bufsz - start_byte;
  }

  return id->buffer[bufnum] + start_byte;
}


char* ipcbuf_get_next_read (ipcbuf_t* id, uint64_t* bytes)
{
  return ipcbuf_get_next_read_work (id, bytes, 0);
}

char* ipcbuf_get_next_readable (ipcbuf_t* id, uint64_t* bytes)
{
  return ipcbuf_get_next_read_work (id, bytes, SEM_UNDO);
}

uint64_t ipcbuf_tell (ipcbuf_t* id, uint64_t bufnum)
{
  ipcsync_t* sync = id->sync;

#ifdef _DEBUG
  fprintf (stderr, 
           "ipcbuf_tell: bufnum=%"PRIu64" s_buf=%"PRIu64" s_buf=%"PRIu64"\n",
           bufnum, sync->s_buf[id->xfer], sync->s_byte[id->xfer]);
#endif

  if (bufnum <= sync->s_buf[id->xfer])
    return 0;

  bufnum -= sync->s_buf[id->xfer];

  return bufnum*sync->bufsz - sync->s_byte[id->xfer];
}

int64_t ipcbuf_tell_write (ipcbuf_t* id)
{
  if (ipcbuf_eod (id))
    return -1;

  if (!ipcbuf_is_writer (id))
    return -1;

  return ipcbuf_tell (id, id->sync->w_buf);
}

int64_t ipcbuf_tell_read (ipcbuf_t* id)
{
  if (ipcbuf_eod (id))
    return -1;

  if (id->state == IPCBUF_READING)
    return ipcbuf_tell (id, id->sync->r_buf);
  else if (id->state == IPCBUF_VIEWING)
    return ipcbuf_tell (id, id->viewbuf);
  else
    return 0;
}

int ipcbuf_mark_cleared (ipcbuf_t* id)
{
  ipcsync_t* sync = 0;

  if (!id)
  {
    fprintf (stderr, "ipcbuf_mark_cleared: no ipcbuf!\n");
    return -1;
  }

  if (id->state != IPCBUF_READING)
  {
    fprintf (stderr, "ipcbuf_mark_cleared: not reading\n");
    return -1;
  }

  sync = id->sync;

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_mark_cleared: increment CLEAR=%d\n",
                   semctl (id->semid, IPCBUF_CLEAR, GETVAL));
#endif

  /* increment the buffers cleared semaphore */
  if (ipc_semop (id->semid, IPCBUF_CLEAR, 1, 0) < 0)
    return -1;

  if (sync->eod[id->xfer] && sync->r_buf == sync->e_buf[id->xfer])
  {
#ifdef _DEBUG
    fprintf (stderr, "ipcbuf_mark_cleared: increment EODACK=%d; CLEAR=%d\n",
                     semctl (id->semid, IPCBUF_EODACK, GETVAL),
                     semctl (id->semid, IPCBUF_CLEAR, GETVAL));
#endif

    id->state = IPCBUF_RSTOP;
    id->sync->r_state = 0;

    sync->r_xfer ++;
    id->xfer = sync->r_xfer % IPCBUF_XFERS;

    if (ipc_semop (id->semid, IPCBUF_EODACK, 1, 0) < 0) {
      fprintf (stderr, "ipcbuf_mark_cleared: error incrementing EODACK\n");
      return -1;
    }
  }
  else
  {
#ifdef _DEBUG
    fprintf (stderr, "ipcbuf_mark_cleared: increment r_buf=%"PRIu64"\n",
             sync->r_buf);
#endif
    sync->r_buf ++;
  }

  return 0;
}

int ipcbuf_reset (ipcbuf_t* id)
{
  uint64_t ibuf = 0;
  uint64_t nbufs = ipcbuf_get_nbufs (id);
  ipcsync_t* sync = id->sync;
  unsigned ix = 0;

  /* if the reader has reached end of data, reset the state */
  if (id->state == IPCBUF_RSTOP)
  {
    id->state = IPCBUF_READER;
    return 0;
  }

  /* otherwise, must be the designated writer */
  if (!ipcbuf_is_writer(id))  {
    fprintf (stderr, "ipcbuf_reset: invalid state=%d\n", id->state);
    return -1;
  }

  if (sync->w_buf == 0)
    return 0;

  for (ibuf = 0; ibuf < nbufs; ibuf++)
  {
    while (id->count[ibuf])
    {
#ifdef _DEBUG
      fprintf (stderr, "ipcbuf_reset: decrement CLEAR=%d\n",
                       semctl (id->semid, IPCBUF_CLEAR, GETVAL));
#endif

      /* decrement the buffers cleared semaphore */
      if (ipc_semop (id->semid, IPCBUF_CLEAR, -1, 0) < 0)
      {
        fprintf (stderr, "ipcbuf_reset: error decrementing CLEAR\n");
        return -1;
      }

      id->count[ibuf] --;
    }
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_reset: decrement SODACK=%d\n",
                   semctl (id->semid, IPCBUF_SODACK, GETVAL));
#endif

  if (ipc_semop (id->semid, IPCBUF_SODACK, -IPCBUF_XFERS, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_reset: error decrementing SODACK\n");
    return -1;
  }

#ifdef _DEBUG
  fprintf (stderr, "ipcbuf_reset: decrement EODACK=%d\n",
                   semctl (id->semid, IPCBUF_EODACK, GETVAL));
#endif

  if (ipc_semop (id->semid, IPCBUF_EODACK, -IPCBUF_XFERS, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_reset: error decrementing EODACK\n");
    return -1;
  }

  if (ipc_semop (id->semid, IPCBUF_SODACK, IPCBUF_XFERS, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_reset: error resetting SODACK\n");
    return -1;
  }

  if (ipc_semop (id->semid, IPCBUF_EODACK, IPCBUF_XFERS, 0) < 0)
  {
    fprintf (stderr, "ipcbuf_reset: error resetting EODACK\n");
    return -1;
  }

  sync->w_buf = 0;
  sync->r_buf = 0;
  sync->w_xfer = 0;
  sync->r_xfer = 0;

  for (ix=0; ix < IPCBUF_XFERS; ix++)
    sync->eod[ix] = 1;

  return 0;
}

/* reset the buffer count and end of data flags, without prejudice */
int ipcbuf_hard_reset (ipcbuf_t* id)
{
  ipcsync_t* sync = id->sync;
  unsigned ix = 0;
  int val = 0;

  sync->w_buf = 0;
  sync->r_buf = 0;
  sync->w_xfer = 0;
  sync->r_xfer = 0;

  for (ix=0; ix < IPCBUF_XFERS; ix++)
    sync->eod[ix] = 1;

  if (semctl (id->semid, IPCBUF_FULL, SETVAL, val) < 0)
  {
    perror ("ipcbuf_hard_reset: semctl (IPCBUF_FULL, SETVAL)");
    return -1;
  }

  if (semctl (id->semid, IPCBUF_CLEAR, SETVAL, val) < 0)
  {
    perror ("ipcbuf_hard_reset: semctl (IPCBUF_FULL, SETVAL)");
    return -1;
  }

  return 0;
}

int ipcbuf_lock (ipcbuf_t* id)
{
#ifdef SHM_LOCK

  uint64_t ibuf = 0;

  if (id->syncid < 0 || id->shmid == 0)
    return -1;

  if (shmctl (id->syncid, SHM_LOCK, 0) < 0)
  {
    perror ("ipcbuf_lock: shmctl (syncid, SHM_LOCK)");
    return -1;
  }

  for (ibuf = 0; ibuf < id->sync->nbufs; ibuf++)
    if (shmctl (id->shmid[ibuf], SHM_LOCK, 0) < 0)
    {
      perror ("ipcbuf_lock: shmctl (shmid, SHM_LOCK)");
      return -1;
    }

  return 0;

#else

  fprintf(stderr, "ipcbuf_lock does nothing on this platform!\n");
  return -1;

#endif

}

int ipcbuf_unlock (ipcbuf_t* id)
{
#ifdef SHM_UNLOCK

  uint64_t ibuf = 0;

  if (id->syncid < 0 || id->shmid == 0)
    return -1;

  if (shmctl (id->syncid, SHM_UNLOCK, 0) < 0)
  {
    perror ("ipcbuf_lock: shmctl (syncid, SHM_UNLOCK)");
    return -1;
  }

  for (ibuf = 0; ibuf < id->sync->nbufs; ibuf++)
    if (shmctl (id->shmid[ibuf], SHM_UNLOCK, 0) < 0)
    {
      perror ("ipcbuf_lock: shmctl (shmid, SHM_UNLOCK)");
      return -1;
    }

  return 0;

#else

  fprintf(stderr, "ipcbuf_unlock does nothing on this platform!\n");
  return -1;

#endif

}


int ipcbuf_eod (ipcbuf_t* id)
{
  if (!id)
  {
    fprintf (stderr, "ipcbuf_eod: invalid ipcbuf_t*\n");
    return -1;
  }

  return ( (id->state == IPCBUF_RSTOP) || (id->state == IPCBUF_VSTOP) );
}


int ipcbuf_sod (ipcbuf_t* id)
{
  return id->state == IPCBUF_READING || id->state == IPCBUF_WRITING;
}

uint64_t ipcbuf_get_write_count (ipcbuf_t* id)
{
  return id->sync->w_buf;
}

uint64_t ipcbuf_get_read_count (ipcbuf_t* id)
{
  return id->sync->r_buf;
}

uint64_t ipcbuf_get_nbufs (ipcbuf_t* id)
{
  return id->sync->nbufs;
}

uint64_t ipcbuf_get_bufsz (ipcbuf_t* id)
{
  return id->sync->bufsz;
}

uint64_t ipcbuf_get_nfull (ipcbuf_t* id) 
{
  return semctl (id->semid, IPCBUF_FULL, GETVAL);
}

uint64_t ipcbuf_get_nclear (ipcbuf_t* id)
{
  return semctl (id->semid, IPCBUF_CLEAR, GETVAL);
}

uint64_t ipcbuf_get_sodack (ipcbuf_t* id)
{
  return semctl (id->semid, IPCBUF_SODACK, GETVAL);
}

uint64_t ipcbuf_get_eodack (ipcbuf_t* id)
{
  return semctl (id->semid, IPCBUF_EODACK, GETVAL);
}


/* Sets the buffer at which clocking began.  */
uint64_t ipcbuf_set_soclock_buf (ipcbuf_t* id)
{
  if (id->sync->w_xfer > 0) 
    id->soclock_buf = id->sync->e_buf[(id->sync->w_xfer-1) % IPCBUF_XFERS] + 1;
  else
    id->soclock_buf = 0;

  return id->soclock_buf;
}

