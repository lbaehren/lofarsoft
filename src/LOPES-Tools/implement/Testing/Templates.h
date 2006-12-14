/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

namespace LOPES {  // namespace LOPES -- begin

  // ============================================================================
  //
  //  The base class
  //
  // ============================================================================

  /*!
    \class TemplatesBase

    \brief A base class for testing of templates usage

    \author Lars B&auml;hren

    \date 2006/12/13
   */
  template <class T> class TemplatesBase {
    
    T data_p;
    
  public:
    
    //! Default constructor
    TemplatesBase () {
      data_p = T(0);
    }
    
    //! Argumented constructor
    TemplatesBase (T const &data) {
      setData (data);
    }
    
    //! Destructor
    ~TemplatesBase () {}
    
    //! Get the data
    T data () {
      return data_p;
    }
    
    //! Set the data
    void setData (T const &data) {
      data_p = data;
    }

    //! Provide a summary of the current object
    void summary ();
    
  };
  
  // ============================================================================
  //
  //  A child class
  //
  // ============================================================================

}  // namespace LOPES -- end
