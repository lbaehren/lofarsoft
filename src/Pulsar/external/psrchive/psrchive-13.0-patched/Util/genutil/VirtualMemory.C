/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "VirtualMemory.h"
#include "Error.h"

#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>

using namespace std;

#include <algorithm>

//! Construct with regular expression
VirtualMemory::VirtualMemory (const string& filename, bool unlink_swapfile)
  : TemporaryFile (filename)
{
  // cerr << "VirtualMemory ctor filename=" << get_filename() << endl;

  // so the swap file will automatically be deleted when the program exists
  if (unlink_swapfile)
    unlink ();

  swap_space = 0;

#ifdef HAVE_PTHREAD
  context = new ThreadContext;
#else
  context = 0;
#endif
}

//! Destructor
VirtualMemory::~VirtualMemory ()
{
}

//! Map the specified number of bytes into memory
void* VirtualMemory::mmap (uint64_t size)
{
  // cerr << "VirtualMemory::mmap size=" << size << endl;

  ThreadContext::Lock lock (context);

  /*
    get a block with at least size bytes
  */
  Block block = find_available (size);
  if (block->second < size)
  {
    cerr << "VirtualMemory::mmap block size=" << block->second 
	     << " < required size=" << size << endl;
    exit (-1);
  }

  char* ptr = block->first;

  /*
    cannot set block->first, so must create a new entry and delete the old
  */
  uint64_t remaining = block->second - size;
  if (remaining)
    add_available (ptr+size, remaining);

  available.erase (block);

  /*
    keep record of what has been allocated
  */
  add_allocated (ptr, size);

  return ptr;
}

//! Free the memory to which the char* points
void VirtualMemory::munmap (void* ptr)
{
  Block block = find_allocated( static_cast<char*>(ptr) );

  add_available (block->first, block->second);

  allocated.erase (block);
}

//! Add allocated memory
void VirtualMemory::add_allocated (char* ptr, uint64_t size)
{
  allocated[ptr] = size;
}

//! Find allocated memory
VirtualMemory::Block VirtualMemory::find_allocated (char* ptr)
{
  for (Block block = allocated.begin(); block != allocated.end(); block++)
    if (block->first == ptr)
      return block;

  cerr << "VirtualMemory::find_allocated address=" << ptr 
       << " not in allocated list" << endl;
  exit (-1);
}

//! Find available memory
VirtualMemory::Block VirtualMemory::find_available (uint64_t size)
{
  for (Block block = available.begin(); block != available.end(); block++)
    if (block->second >= size)
      return block;
  
  return extend (size);
}

//! Add available memory
VirtualMemory::Block VirtualMemory::add_available (char* ptr, uint64_t size)
{
  char* end = ptr+size;

  Block block;

  /*
    check for contiguous blocks that are currently available
  */
  for (block = available.begin(); block != available.end(); block++)
  {
    char* block_end = block->first + block->second;

    if (block_end == ptr)
    {
      /*
	the new block follows an existing block, append to existing block
	and return the existing block
      */
      block->second += size;
      return block;
    }

    if (block->first == end)
    {
      /*
	an existing block follows the new block, erase the existing block
	and increase the size of the new block, then break
      */
      size += block->second;
      available.erase (block);
      break;
    }
  }

  // add a new block
  available[ptr] = size;
  return available.find (ptr);
}

VirtualMemory::Block VirtualMemory::extend (uint64_t size)
{
  const uint64_t MB = 1024 * 1024;
  const uint64_t maximum_increment = 1024*MB; // 1GB
  uint64_t current = swap_space;

  do
  {
    if (swap_space == 0)
      swap_space = getpagesize();
    else if (swap_space < maximum_increment)
      swap_space *= 2;
    else
	  swap_space += maximum_increment;
  }
  while (swap_space-current < size);

  /*
    Stretch the file size to swap_space
  */
  off_t result = lseek( get_fd(), swap_space-1, SEEK_SET );

  if (result == (off_t)-1)
  {
    cerr << "VirtualMemory::extend could not lseek swap file to " 
	     << swap_space-1 << " - " << strerror (errno) << endl;
    exit (-1);
  }

  /*
    Write a null character at the end of the file
  */
  result = write( get_fd(), "", 1 );
  if (result != 1)
  {
    cerr << "VirtualMemory::extend could not write to swap file at " 
	     << swap_space << " - " << strerror (errno) << endl;
    exit (-1);
  }

  /*
    Map the new swap space into memory
  */
  // cerr << "mmap new=" << swap_space-current << " bytes" << endl;

  void* ptr = ::mmap( 0, swap_space-current,
		      PROT_READ | PROT_WRITE, MAP_SHARED,
		      get_fd(), current );

  if (ptr == MAP_FAILED)
  {
    cerr << "VirtualMemory::extend could not mmap swap file from " 
	     << current << " to " << swap_space
	     << " - " << strerror (errno) << endl;
    exit (-1);
  }

  return add_available( static_cast<char*>(ptr), swap_space-current );
}

#if 0

/* Don't forget to free the mmapped memory
 */
if (munmap(map, FILESIZE) == -1) {
  perror("Error un-mmapping the file");
  /* Decide here whether to close(get_fd()) and exit() or not. Depends... */
 }

/* Un-mmaping doesn't close the file, so we still need to do that.
 */
close(get_fd());

#endif

