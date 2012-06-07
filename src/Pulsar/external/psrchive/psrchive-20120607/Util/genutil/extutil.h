//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/extutil.h,v $
   $Revision: 1.6 $
   $Date: 2009/09/02 21:28:55 $
   $Author: straten $ */

#ifndef __Pulsar_get_extension_h
#define __Pulsar_get_extension_h

#include "Functor.h"

//! implementation of Archive, Integration, and Profile::get<T> methods
template<class T, class Container>
const T* get_ext (const Container* container, const char* method, bool verbose)
{
  const T* extension = 0;

  const unsigned next = container->get_nextension();
  for (unsigned iext=0; iext<next; iext++)
  {
    const typename Container::Extension* ext = container->get_extension (iext);

    if (verbose)
      std::cerr << method << " name="
		<< ext->get_extension_name() << std::endl;
      
    extension = dynamic_cast<const T*>( ext );
      
    if (extension)
      return extension;
  }

  if (verbose)
    std::cerr << method << " failed to find extension type="
	      << typeid(extension).name() << std::endl;

  return extension;
}

template<class T> 
void clean_dangling (std::vector<T>& data)
{
  for (unsigned i=0; i<data.size(); i++)
    if (!data[i])
      data.erase( data.begin() + i );
}


//! For each extension of type E in container C, call method B::method
template<typename E, typename C, typename B>
void foreach (C* container, void (B::*method)())
{
  const unsigned next = container->get_nextension ();

  for (unsigned iext=0; iext < next; iext++)
  {
    E* ext = dynamic_cast<E*> (container->get_extension (iext));
    if (ext)
      (ext->*method) ();
  }
}

//! For each extension of type E in container C, call method M with argument A
template<typename E, typename C, typename M, typename A>
void foreach (C* container, M method, A argument)
{
  const unsigned next = container->get_nextension ();

  for (unsigned iext=0; iext < next; iext++)
  {
    E* ext = dynamic_cast<E*> (container->get_extension (iext));
    if (ext)
      (ext->*method) (argument);
  }
}

//! For each extension of type E in container C, call method Functor(E*)
template<typename E, typename C>
void foreach (C* container, Functor< void(E*) >& functor)
{
  const unsigned next = container->get_nextension ();

  for (unsigned iext=0; iext < next; iext++)
  {
    E* ext = dynamic_cast<E*> (container->get_extension (iext));
    if (ext)
      functor (ext);
  }
}

//! For each extension of type E in container C, call method Functor(const E*)
template<typename E, typename C>
void foreach (const C* container, Functor< void(const E*) >& functor)
{
  const unsigned next = container->get_nextension ();

  for (unsigned iext=0; iext < next; iext++)
  {
    const E* ext = dynamic_cast<const E*> (container->get_extension (iext));
    if (ext)
      functor (ext);
  }
}


#endif

