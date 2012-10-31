/**************************************************************************
 *  Header for the implementation of the CR Pipeline Python bindings.     *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <m.vandenakker@astro.ru.nl>                      *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

#ifndef CR_PIPELINE_BINDINGS_H
#define CR_PIPELINE_BINDINGS_H

// ========================================================================
//
//  Header files
//
// ========================================================================


// ========================================================================
//
//  Function declarations
//
// ========================================================================

void hInit();

namespace PyCR { // Namespace PyCR -- begin

  // Exception translators
  void translateException(Exception const& e);
  void translateValueError(ValueError const& e);
  void translateTypeError(TypeError const& e);
  void translateIndexError(IndexError const& e);
  void translateKeyError(KeyError const& e);
  void translateMemoryError(MemoryError const& e);
  void translateArithmeticError(ArithmeticError const& e);
  void translateEOFError(EOFError const& e);
  void translateFloatingPointError(FloatingPointError const& e);
  void translateOverflowError(OverflowError const& e);
  void translateZeroDivisionError(ZeroDivisionError const& e);
  void translateNameError(NameError const& e);
  void translateNotImplementedError(NotImplementedError const& e);

} // Namespace PyCR -- end


#endif /* CR_PIPELINE_BINDINGS_H */

