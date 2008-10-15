/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                 *
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

/* $Id: inverseFFT.h,v 1.5 2007/04/03 14:03:07 bahren Exp $*/

#ifndef _INVERSEFFT_H_
#define _INVERSEFFT_H_

#include <crtools.h>

// AIPS++ header files
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/string.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Cube.h>
#include <casa/Arrays/ArrayIO.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Quanta.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>
#include <scimath/Mathematics/InterpolateArray1D.h>

// CR header files
#include <Math/StatisticsFilter.h>
#include <IO/DataIterator.h>

using casa::DComplex;
using casa::Double;
using casa::FFTServer;
using casa::Matrix;
using casa::Vector;

const casa::Double pi = 3.14159265;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class inverseFFT
    
    \ingroup Analysis
    
    \brief Brief description for class inverseFFT
    
    \author Kalpana Singh

    \date 2006/12/19

    \test tinverseFFT.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class inverseFFT {

    //! Coefficients of the Poly-Phase filter
    Vector<Double> ppfCoefficients_p;
    //! Size of a block of data
    uint dataBlocksize_p;
    //! The 10.7 cm flux on previous day (1e-22 W/m^2/Hz )
    double f107_p;
    //! The geographic longitude (eastward positive)
    double longitude_p;
    //! The geographic latitude
    double latitude_p;
    //! The altitude in km
    double altitude_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    inverseFFT ();
    
    /*!
      \brief Argumented constructor

      \param F107  -- F107 is the 10.7 cm flux on previous day (1e-22 W/m^2/Hz )
      \param mea_F107 -- average F107 over three solar rotations (81 days)
      \param tableno  -- modelcoefficients table has to be used
      \param col -- whether coefficients are called for temperature or for number density
      \param t_day -- day of the year like 20th day or 200th day of the year
      \param tau -- local time in hrs
      \param t -- UT in seconds
      \param Ap -- daily magnetic index
      \param latitude -- geographic latitude
      \param longitude -- geographic longitude (eastward positive)
      \param altitude -- height in Km
      \param molecular weight -- molecular weight of the molecule (just like N2= 28)
      \param n_l_average -- average density at height zl =120 km.
    */

    inverseFFT (Double const &F107,
                const Double& mean_F107,
		const uint& tableno,
                const uint& col,
                const uint& t_d,
                const Double& tau,
                const Double& t,
                const Double& Ap,
                const Double& latitude,
                const Double& longitude,
                const Double& altitude,
                const Double& molecular_weight,
                const Double& n_l_average );
	
    /*!
      \brief Copy constructor 

      \param other -- Another inverseFFT object
    */
    inverseFFT (inverseFFT const &other);
      
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~inverseFFT ();
    
    // ---------------------------------------------------------------- Operators

    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another inverseFFT object from which to make a copy.
    */
    inverseFFT& operator= (inverseFFT const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the 10.7 cm flux on previous day

      \return f107-- The 10.7 cm flux on previous day (1e-22 W/m^2/Hz )
    */
    inline double F107  () const { return f107_p; }
    /*!
      \brief Set the 10.7 cm flux on previous day

      \param f107-- The 10.7 cm flux on previous day (1e-22 W/m^2/Hz )
    */
    inline void setF107 (double const &f107) { f107_p = f107; }
    
    /*!
      \brief Get the geographic longitude

      \return longitude -- The geographic longitude (eastward positive)
    */
    inline double longitude () const { return longitude_p; }
    /*!
      \brief Set the geographic longitude

      \param longitude -- The geographic longitude (eastward positive)
    */
    inline void setLongitude (double const &longitude) { longitude_p = longitude; }

    /*!
      \brief Get the geographic latitude

      \return latitude -- The geographic latitude
    */
    inline double latitude  () const { return latitude_p; }
    /*!
      \brief Set the geographic latitude

      \param latitude -- The geographic latitude
    */
    inline void setLatitude  (double const &latitude)  { latitude_p = latitude; }

    /*!
      \brief Get the altitude

      \return altitude -- height in Km
    */
    inline double altitude () const { return altitude_p; };
    /*!
      \brief Set the altitude

      \param altitude -- height in Km
    */
    inline void setAltitude (double const &altitude) { altitude_p = altitude; };
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, inverseFFT.
    */
    std::string className () const {
      return "inverseFFT";
    }
    
    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief derivatives of associated legendre polynomial functions
      
    */
    Double diff_legendre( const Double& latitude,
                          const uint& degree,
                          const uint& order ) ;


      /*!
      \brief value of associated legendre polynomial functions
      
    */
    Double legendre( const Double& latitude,
                     const uint& degree,
                     const uint& order ) ;

   
     /*!
      \brief value of solar activity function
      
    */

    Double solar_activity( const Double& F107,
                           const Double& mean_F107,
                           const uint& tableno,
                           const uint& col,
                           const Double& latitude ) ;

      
     /*!
      \brief value of symmetrical function
      
    */

    Double symmetrical( const uint& tableno,
                        const uint& col,
                        const Double& t_d,
                        const Double& latitude ) ;

    /*!
      \brief value of asymmetrical function
      
    */

    Double asymmetrical( const Double& F107,
                         const Double& mean_F107,
                         const uint& tableno,
                         const uint& col,
                         const Double& t_d,
                         const Double& latitude ) ;

    /*!
      \brief value of diurnal function
      
    */
   
    Double diurnal( const Double& F107,
                    const Double& mean_F107,
                    const uint& tableno,
                    const uint& col,
                    const Double& t_d,
                    const Double& tau,
                    const Double& latitude ) ; 

    /*!
      \brief value of semidiurnal function
      
    */
   
    Double semidiurnal( const Double& F107,
                        const Double& mean_F107,
                        const uint& tableno,
                        const uint& col,
                        const Double& t_d,
                        const Double& tau,
                        const Double& latitude ) ; 
  
   /*!
      \brief value of terdiurnal function
      
    */
   
    Double terdiurnal( const Double& F107,
                        const Double& mean_F107,
                        const uint& tableno,
                        const uint& col,
                        const Double& t_d,
                        const Double& tau,
                        const Double& latitude ) ; 

   
   /*!
      \brief value of magnetic function
      
    */
   
    Double magnetic( const uint& tableno,
                     const uint& col,
                     const Double& t_d,
                     const Double& tau,
                     const Double& Ap,
                     const Double& latitude ) ; 
 
    /*!
      \brief value of longitudinal function
      
    */
   
    Double longitudinal( const Double& F107,
                         const Double& mean_F107,
                         const uint& tableno,
                         const uint& col,
                         const Double& t_d,
                         const Double& tau,
                         const Double& latitude,
                         const Double& longitude ) ; 
    /*!
      \brief value of UT function
      
    */
   
    Double UT( const Double& F107,
               const Double& mean_F107,
               const uint& tableno,
               const uint& col,
               const Double& t_d,
               const Double& tau,
               const Double& t,
               const Double& latitude,
               const Double& longitude ) ;

    /*!
      \brief value of combined function
      
    */
   
    Double combined( const Double& F107,
                     const Double& mean_F107,
                     const uint& tableno,
                     const uint& col,
                     const Double& t_d,
                     const Double& tau,
                     const Double& t,
                     const Double& Ap,
                     const Double& latitude,
                     const Double& longitude ) ;

    /*!
      \brief value of expansion function
      
    */
   
    Double expansion( const Double& F107,
                      const Double& mean_F107,
                      const uint& tableno,
                      const uint& col,
                      const Double& t_d,
                      const Double& tau,
                      const Double& t,
                      const Double& Ap,
                      const Double& latitude,
                      const Double& longitude ) ;
		     
    /*!
      \brief value of temperature profile      
    */
   
    Double temperature( const Double& F107,
                        const Double& mean_F107,
                        const uint& tableno,
                        const uint& col,
                        const Double& t_d,
                        const Double& tau,
                        const Double& t,
                        const Double& Ap,
                        const Double& latitude,
                        const Double& longitude,
                        const Double& altitude ) ;


    /*!
      \brief value of D profile      
    */
   
    Double Dprofile( const Double& F107,
                     const Double& mean_F107,
                     const uint& tableno,
                     const uint& col,
                     const Double& t_d,
                     const Double& tau,
                     const Double& t,
                     const Double& Ap,
                     const Double& latitude,
                     const Double& longitude,
                     const Double& altitude,
                     const Double& molecular_weight ) ;
    
		     
    /*!
      \brief value of density profile of Nitrogen molecule   
    */
   
    Double N2densityprofile( const Double& F107,
                             const Double& mean_F107,
                             const Double& t_d,
                             const Double& tau,
                             const Double& t,
                             const Double& Ap,
                             const Double& latitude,
                             const Double& longitude,
                             const Double& altitude ) ;
   
			     
   /*!
      \brief value of density profile of Oxygen molecule   
    */			     
 
  Double O2densityprofile( const Double& F107,
                           const Double& mean_F107,
                           const Double& t_d,
                           const Double& tau,
                           const Double& t,
                           const Double& Ap,
                           const Double& latitude,
                           const Double& longitude,
                           const Double& altitude ) ;
     			   
			   
     /*!
      \brief value of density profile of Argon atom 
    */			     
  Double Ardensityprofile( const Double& F107,
                           const Double& mean_F107,
                           const Double& t_d,
                           const Double& tau,
                           const Double& t,
                           const Double& Ap,
                           const Double& latitude,
                           const Double& longitude,
                           const Double& altitude ) ;
      
      
      /*!
      \brief value of density profile of Oxygen atom 
    */			     
  Double Odensityprofile( const Double& F107,
                          const Double& mean_F107,
                          const Double& t_d,
                          const Double& tau,
                          const Double& t,
                          const Double& Ap,
                          const Double& latitude,
                          const Double& longitude,
                          const Double& altitude ) ;
    
    
    
      
      
      /*!
      \brief value of density profile of Nitrogen atom 
    */			     
  Double Ndensityprofile( const Double& F107,
                          const Double& mean_F107,
                          const Double& t_d,
                          const Double& tau,
                          const Double& t,
                          const Double& Ap,
                          const Double& latitude,
                          const Double& longitude,
                          const Double& altitude ) ;
    
    
      
      
      /*!
      \brief value of density profile of Helium atom 
    */			     
    
  Double Hedensityprofile( const Double& F107,
                           const Double& mean_F107,
                           const Double& t_d,
                           const Double& tau,
                           const Double& t,
                           const Double& Ap,
                           const Double& latitude,
                           const Double& longitude,
                           const Double& altitude ) ;
    
    
        
  
  /*!
    \brief value of density profile of Hydrogen atom 
  */			     
  Double Hdensityprofile( const Double& F107,
                          const Double& mean_F107,
                          const Double& t_d,
                          const Double& tau,
                          const Double& t,
                          const Double& Ap,
                          const Double& latitude,
                          const Double& longitude,
                          const Double& altitude ) ;
  
  private:
  
  /*!
    \brief Unconditional copying
  */
  void copy (inverseFFT const &other);
  
  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);
  
  };
  
}  // Namespace CR -- end

#endif /* INVERSEFFT_H */

