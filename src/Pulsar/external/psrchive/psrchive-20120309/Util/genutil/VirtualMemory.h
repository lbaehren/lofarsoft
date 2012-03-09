//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/VirtualMemory.h,v $
   $Revision: 1.5 $
   $Date: 2009/09/01 06:29:02 $
   $Author: straten $ */

#ifndef __VirtualMemory_h
#define __VirtualMemory_h

#include "TemporaryFile.h"
#include "ThreadContext.h"

#include <map>
#include <inttypes.h>

//! Virtual memory manager
class VirtualMemory : public TemporaryFile
{
 public:

  //! Construct a virutal memory resource with the given filename
  VirtualMemory (const std::string& filename, bool unlink = true);

  //! Destructor
  ~VirtualMemory ();

  //! Create a new array
  template<typename T>
  T* create (unsigned elements)
  { return static_cast<T*>( mmap (elements * sizeof(T)) ); }

  //! Destroy an existing array
  void destroy (void* pointer)
  { munmap (pointer); }

 private:

  //! Map the specified number of bytes into memory
  void* mmap (uint64_t length);

  //! Free the memory to which the pointer points
  void munmap (void*);

  typedef std::map<char*, uint64_t> Map;
  typedef Map::iterator Block;

  //! Add allocated memory
  void add_allocated (char* ptr, uint64_t size);

  //! Find allocated memory
  Block find_allocated (char* ptr);

  //! Add available memory
  Block add_available (char* ptr, uint64_t size);

  //! Find available memory
  Block find_available (uint64_t size);

  //! List of available blocks: base addresses and lengths
  Map available;

  //! List of allocated blocks: base addresses and lengths
  Map allocated;

  //! Protection in multi-threaded applications
  ThreadContext* context;

  //! Extend the swap space
  Block extend (uint64_t size);

  //! The total number of bytes mapped into the temporary file
  uint64_t swap_space;

  //! Cleanup all resources
  void destroy ();

};

#endif

