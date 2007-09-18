/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Lars Baehren (<mail>)                                                     *
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

#ifndef H5IMPORTCONFIG_H
#define H5IMPORTCONFIG_H

// Standard library header files
#include <iostream>
#include <string>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class H5ImportConfig
    
    \ingroup IO
    
    \brief Brief description for class H5ImportConfig
    
    \author Lars Baehren

    \date 2007/09/18

    \test tH5ImportConfig.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class H5ImportConfig {

    //! Path to the file, to which the configuration is written
    std::string configFile_p;
    //! The number of dimensions in the dataset
    int rank_p;
    //! Size of the dataset dimensions
    int *dimensions_p;
    //! Path of the output dataset
    std::string path_p;
    //! String denoting the type of input data
    std::string inputClass_p;
    //! An integer denoting the size of the input data, in bits
    int inputSize_p;
    //! A string denoting the type of output data
    std::string outputClass_p;
    //! An integer denoting the size of the output data, in bits
    int outputSize_p;
    //! Type of the output architecture
    std::string outputArchitecture_p;
    //! Output byte order
    std::string outputByteOrder_p;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    H5ImportConfig ();
    
    /*!
      \brief Argumented constructor
      
      \param rank       -- 
      \param dimensions -- 
    */
    H5ImportConfig (int const &rank,
		    int const *dimensions);
      
    /*!
      \brief Argumented constructor

      \param configFile -- 
      \param rank       -- 
      \param dimensions -- 
    */
    H5ImportConfig (std::string const &configFile,
		    int const &rank,
		    int const *dimensions);
      
      /*!
      \brief Copy constructor
      
      \param other -- Another H5ImportConfig object from which to create this new
      one.
    */
    H5ImportConfig (H5ImportConfig const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~H5ImportConfig ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another H5ImportConfig object from which to make a copy.
    */
    H5ImportConfig& operator= (H5ImportConfig const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the path to the file, to which the configuration is written

      \return filename -- Path to the file, to which the configuration is written
    */
    inline std::string configFile () const {
      return configFile_p;
    }

    /*!
      \brief Set the path to the file, to which the configuration is written

      \param filename -- Path to the file, to which the configuration is written

      \return status -- Status of the operation; returns <i>false</i> in case an
                        error was encountered.
    */
    inline bool setConfigFile (std::string const &filename) {
      bool status (true);
      try {
	configFile_p = filename;
      } catch (std::string message) {
	std::cerr << "[H5ImportConfig::setConfigFile] " << message << std::endl;
	status = false;
      }
      return status;
    }
    
    /*!
      \brief Get the size/shape of the dataset as string

      \param fastestFirst -- List the fasted varying array axis first?

      \return dimensions
    */
    std::string dimensionsAsString (bool const &fastestFirst=false);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, H5ImportConfig.
    */
    std::string className () const {
      return "H5ImportConfig";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    bool exportSettings (std::string const &outfile);
    
  private:
    
    /*!
      \brief Initialize the internal parameter list
      
      \param rank       -- 
      \param dimensions -- 
    */
    void init (int const &rank,
	       int const *dimensions);

    /*!
      \brief Initialize the internal parameter list

      \param configFile -- 
      \param rank       -- 
      \param dimensions -- 
    */
    void init (std::string const &configFile,
	       int const &rank,
	       int const *dimensions);

    /*!
      \brief Unconditional copying
    */
    void copy (H5ImportConfig const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* H5IMPORTCONFIG_H */
  
