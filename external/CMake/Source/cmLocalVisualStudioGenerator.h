/*=========================================================================

  Program:   CMake - Cross-Platform Makefile Generator
  Module:    $RCSfile: cmLocalVisualStudioGenerator.h,v $
  Language:  C++
  Date:      $Date: 2006/07/24 15:19:35 $
  Version:   $Revision: 1.1.2.1 $

  Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef cmLocalVisualStudioGenerator_h
#define cmLocalVisualStudioGenerator_h

#include "cmLocalGenerator.h"

class cmSourceFile;
class cmSourceGroup;

/** \class cmLocalVisualStudioGenerator
 * \brief Base class for Visual Studio generators.
 *
 * cmLocalVisualStudioGenerator provides functionality common to all
 * Visual Studio generators.
 */
class cmLocalVisualStudioGenerator : public cmLocalGenerator
{
public:
  cmLocalVisualStudioGenerator();
  virtual ~cmLocalVisualStudioGenerator();

protected:
  // Safe object file name generation.
  void ComputeObjectNameRequirements(std::vector<cmSourceGroup> const&);
  std::set<const cmSourceFile*> NeedObjectName;
};

#endif
