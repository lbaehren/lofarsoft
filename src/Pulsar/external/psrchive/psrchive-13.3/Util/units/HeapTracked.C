/***************************************************************************
 *
 *   Copyright (C) 2004-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "HeapTracked.h"
#include "Error.h"

#include <algorithm>

using namespace std;

#ifdef HAVE_PTHREAD

#include <pthread.h>

static pthread_mutex_t heap_mutex = PTHREAD_MUTEX_INITIALIZER;

#define LOCK_HEAP    pthread_mutex_lock (&heap_mutex);
#define UNLOCK_HEAP  pthread_mutex_unlock (&heap_mutex);

#else

#define LOCK_HEAP
#define UNLOCK_HEAP

#endif

// list of addresses that have been dynamically allocated on the heap
static vector<const void*> heap_addresses;

size_t Reference::HeapTracked::get_heap_queue_size ()
{
  return heap_addresses.size();
}

bool Reference::verbose = false;

void* Reference::HeapTracked::operator new (size_t size, void* ptr)
{
  if (ptr)
  {
#ifdef _DEBUG
    cerr << "Reference::HeapTracked::operator placement new size=" << size 
	 << " ptr=" << ptr << endl;
#endif

    return ptr;
  }

  LOCK_HEAP

  ptr = ::operator new (size);

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::operator new size=" << size 
       << " ptr=" << ptr << endl;
#endif

  heap_addresses.push_back (ptr);

  UNLOCK_HEAP

  return ptr;
}

void Reference::HeapTracked::operator delete (void* ptr)
{
#ifdef _DEBUG
  cerr << "Reference::HeapTracked::operator delete void*=" << ptr << endl;
#endif

  ::operator delete (ptr);
}

bool Reference::HeapTracked::__is_on_heap () const
{
  const char is_on_heap = 0x01;

  if (__heap_state)
  {
#ifdef _DEBUG
    cerr << "Reference::HeapTracked::__is_on_heap __heap_state="
         << (int)__heap_state << endl;
#endif

    return __heap_state == is_on_heap;
  }

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::is_on_heap this=" << this << endl;
#endif

  const void* raw_address = dynamic_cast<const void*>(this);

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::is_on_heap void*=" 
       << raw_address << endl;
#endif

  if (raw_address == NULL)
    throw Error (InvalidPointer, "Reference::HeapTracked::is_on_heap",
		 "failed dynamic_cast<const void*>(%p)", this);
  
  vector<const void*>::iterator it;

  LOCK_HEAP

  it = std::find( heap_addresses.begin(), heap_addresses.end(), raw_address );

  if ( it != heap_addresses.end() )
  {
    heap_addresses.erase (it);
    __heap_state = is_on_heap;

#ifdef _DEBUG
  cerr << "Reference::HeapTracked::is_on_heap true heap_state="
         << (int)__heap_state << endl;
#endif

    UNLOCK_HEAP

    return true;
  }

  __heap_state = 0x04;

#ifdef _DEBUG
    cerr << "Reference::HeapTracked::is_on_heap false heap_state="
         << (int)__heap_state << endl;
#endif

  UNLOCK_HEAP

  return false;
}

