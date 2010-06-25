/*-------------------------------------------------------------------------*
 | $Id:: rmFITS.cpp 2939 2009-08-12 09:02:41Z baehren                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Sven Duscha (sduscha@mpa-garching.mpg.de)                             *
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

#include <iostream>
#include <string.h>
#include <sys/stat.h>	// needed to check for existence of a file
#include "rmFITS.h"

using namespace std;

namespace RM {

  // ============================================================================
  //
  //  Construction / Destruction
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                      rmFITS
  
  rmFITS::rmFITS()
  {
    // initialise critical pointers
    fptr       = NULL;
    fitsstatus = 0;
    nulval     = 0;
    anynul     = 0;
    iomode     = 0;   /* default READONLY mode */
    
    memset(this->fits_error_message, 0, MAX_MESSAGE_LENGTH); 	  
    
#ifdef HAVE_CASA	   
    lattice_p    = NULL;    // initialise casa lattice
#endif	  
  }
  
  //_____________________________________________________________________________
  //                                                                       rmFITS
  
  /*!
    \param &filename - filename of FITS file
    \param iomode - I/O Read () OR Write
	 \param extension - FITS extension to open (optional)
  */
  rmFITS::rmFITS (const string &filename,
		  int iomode,
		  int extension)
  {
    fptr				= NULL;    // initialise FITS filepointer
    fitsstatus		= 0;       // initialise FITS status
    nulval			= 0.0;
    anynul			= 0;
    
    memset(this->fits_error_message, 0, MAX_MESSAGE_LENGTH);  
    
#ifdef HAVE_CASA	   
    lattice_p    = NULL;    // initialise casa lattice
#endif
    
    // Check if file exists: if it exists open in iomode
    if (fileExists(filename))
      {
        // Depending on iomode: OPEN for READING,WRITING, RW or CREATE a FITS file
        if (fits_open_file(&fptr, filename.c_str(), iomode, &fitsstatus))
	  {
	    if(fitsstatus)
	      {
		fits_get_errstatus(fitsstatus, fits_error_message);
		cout << fits_error_message << endl;
		throw "rmFITS::rmFITS could not open file";	// get fits error from fitsstatus property later
	      }
	  }
      }
    else
      {
        // if file didnt exist, create a new one ...
        if (fits_create_file(&fptr, const_cast<char *>(filename.c_str()), &fitsstatus))
	  {
	    fits_get_errstatus(fitsstatus, fits_error_message);
	    cout << fits_error_message << endl;	
            throw "rmFITS::open could not create file";
	  }
	
    	/*    // ... and open it
	      if (fits_open_file(&fptr, filename.c_str(), iomode, &fitsstatus))
	      {
	      throw "rmFITS::open";	// get fits error from fitsstatus property later
	      }
	*/
	
      }
    
  }
  
  //_____________________________________________________________________________
  //                                                                      rmFITS
  
  /*!
    \brief Copy constructor that copies the whole FITS file
    
    \param other - rmFITS object to be copied to
  */
  rmFITS::rmFITS (rmFITS const &other)
  {
    if (fits_copy_file(fptr, other.fptr, 1, 1, 1, &fitsstatus))
      {
        throw "rmFITS::rmFITS copy constructor";
      }
  }
  
  //_____________________________________________________________________________
  //                                                                      rmFITS
  
  /*!
    \brief Copy constructor that copies a complete rmFITS object to another
    
    \param &other - other rmFITS object to be copied to
    \param previous - copy previous (before CHDU) HDUs to other rmFITS object
    \param current - copy current HDU to other rmFITS object
    \param following - copy following (after CHDU) HDUs to other rmFITS object
  */
  rmFITS::rmFITS (rmFITS const &other,
		  bool previous,
                  bool current,
                  bool following)
  {
    /* Copy prev, current, following to other FITS file */
    int previousInt=0;
    int currentInt=0;
    int followingInt=0;
    
    if (previous)	// if previous bool is true...
      previousInt=1;	// ... set int logic variable to true
    if (current)		// and so on...
      currentInt=1;
    if (following)
      followingInt=1;
    
    if (fits_copy_file(fptr, other.fptr, previousInt, currentInt, followingInt, &fitsstatus))
      {
        throw "rmFITS::rmFITS copy constructor";
      }
  }
  
  //_____________________________________________________________________________
  //                                                                     copyCHDU
  
  /*!
    \brief Copy constructor that copies only one HDU and appends it to another rmFITS object
    
    \param &other - other rmFITS object to append HDU to
    \param hdu - HDU number to copy
  */
  void rmFITS::copyCHDU(rmFITS const &other)
  {
    int morekeys=0;	// Don't reserve space for more keys
    
    if (fits_copy_hdu(fptr, other.fptr, morekeys, &fitsstatus))
      {
        throw "rmFITS::rmFITS copy constructor";
      }
  }
  
  
  /*!
    \brief Destructor
  */
  rmFITS::~rmFITS()
  {
    if(fptr!=NULL)				// only try to close the FITS file if it hasn't been closed before...
      {
	//		cout << "closing FITS file" << endl;		// debug
	this->close();				// ...close the FITS file
      }
    else
      {
	//		cout << "Destroying without closing." << endl;
	
	
	/*
	 *  Deallocate memory: not necessary at the moment, since no memory allocated
	 *  in constructor
	 */
      }
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                     	 open
  
  /*!
    \brief Open an existing file with READONLY or READWRITE access
    
    \param filename - name of file to open
    \param iomode - I/O mode to open the file READONLY / READWRITE
  */
  void rmFITS::open(const string &filename, int iomode, int extension)
  {
    if(fits_open_file(&fptr, const_cast<char *>(filename.c_str()), iomode, &fitsstatus))
      {
	throw "rmFITS::open failed to open file";
      }
  }
  
  
  /*!
    \brief Open the first data HDU in the FITS object
    
    \param iomode - I/O-mode: READONLY, READWRITE
  */
  void rmFITS::openData (const std::string &filename,
			 int iomode)
  {
    if (fits_open_data(&fptr, const_cast<char *>(filename.c_str()), iomode , &fitsstatus))
      {
        throw "rmFITS::openData";
      }
    
    if (getHDUType()==IMAGE_HDU)	 // if it is an image extension
      {
        updateImageDimensions();		// Update dimensions-vector,
      }
    
  }
  
  //_____________________________________________________________________________
  //                                                                    openImage
  
  /*!
    \param iomode - I/O-mode: R, RW
  */
  void rmFITS::openImage (const std::string &filename,
			  int iomode)
  {
    if (fits_open_image(&fptr, const_cast<char *>(filename.c_str()), iomode, &fitsstatus))
      {
        throw "rmFITS::openImage";
      }
    
    if (getHDUType()==IMAGE_HDU)	 // if it is an image extension
      {
        updateImageDimensions();		// Update dimensions-vector,
      }
  }
  
  //_____________________________________________________________________________
  //                                                                    openTable
  
  /*!
    \param iomode - I/O-mode: R, RW
  */
  void rmFITS::openTable (const std::string &filename,
			  int iomode)
  {
    if (fits_open_table(&fptr, const_cast<char *>(filename.c_str()), iomode, &fitsstatus))
      {
        throw "rmFITS::openTable";
      }
    
  }
  
  //_____________________________________________________________________________
  //                                                                   getLattice
  
  /*!
    \brief Get a (casa) Lattice<Float> to access the fits file
  */
  /*
    void rmFITS::getLattice()
    {
    // Generic lattice variable for casa lattice
    casa::LatticeBase *latticeBase;
    // Local string to hold filename 
    std::string filename;
    // Register the FITS and Miriad image types
    casa::FITSImage::registerOpenFunction();
    
    // Get filename of rmFITS object
    
    // Try open the file with generic casa function
    latticeBase=casa::ImageOpener::openImage (filename);
    
    if (lattice_p==NULL) {
    throw "rmFITS::getLattice ";
    }
    
    // Currently only support double lattices:
    lattice_p=dynamic_cast<casa::ImageInterface<casa::Float>*>(latticeBase);
    
    // determine data type of lattice
    switch(lattice->dataType()){
    case TpFloat:
    lattice_float=dynamic_cast<ImageInterface<Float>*>(lattice_Q);
    break;
    case TpDouble:
    lattice_float=dynamic_cast<ImageInterface<Float>*>(lattice_Q);
    break;
    case TpComplex:
    lattice_complex=dynamic_cast<ImageInterface<Complex>*>(lattice_Q);
    break;
    case TpDComplex:
    lattice_dcomplex=dynamic_cast<ImageInterface<DComplex>*>(lattice_Q);
    break;
    default:
    throw AipsError("Image has an invalid data type");
    break;
    }
    
    // check if data type is in accordance with FITS header entry
    }
  */
  
  //_____________________________________________________________________________
  //                                                                        close
  
  /*  
      \brief Close the FITS file of this object		
  */
  void rmFITS::close()
  {
    fits_close_file(this->fptr, &fitsstatus);
    if(fitsstatus)
      {
	cerr << getError();
	throw "rmFITS::close error while closing";
      }
    else
      {
	this->fptr=NULL;		// after successful closing set the fitsfile pointer to NULL
      }
  }
  
  
  //_____________________________________________________________________________
  //                                                                     filename
  
  /*!
    \brief Get the filename of the FITS file opened in this rmFITS object
  */  
  string rmFITS::filename()
  {
    string filename;			// string to contain FITS filename
    
    if(fits_file_name(fptr, const_cast<char*>(filename.c_str()), &fitsstatus))
      {
	throw "rmFITS::filename could not get filename of object";
      }
    
    return filename;
  }
  
  
  //_____________________________________________________________________________
  //                                                                     getError
  
  /*!
    \brief Get the corresponding cfits error message to the current fitsstatus
    
    \return fitserrormsg - Complete string with all FITS errors on the error stack
  */
  std::string rmFITS::getError ()
  {
    char fits_error_message[81];
    //std::string error_message;
    
    
    fits_get_errstatus(fitsstatus, fits_error_message);	// returns descriptive text string (30 char max.) corresponding to a CFITSIO error status code
    
    if(strncmp(fits_error_message, "", 80)!=0)				// if the is an FITS error (correct?)
      {
	return(static_cast<string>(fits_error_message));
      }
    else 
      {
	return("");	// return empty string if there is no FITS error
      }
    
    
    /*
      while (fits_read_errmsg(fits_error_message)!=0)
      {
      cerr << fits_error_message << endl;		// print further messages
      complete_error_message += fits_error_message;	// concat complete error msg
      }
    */
    
    /* Martin's code
       void fitshandle::check_errors() const
       {
       if (status==0) return;
       char msg[81];
       fits_get_errstatus (status, msg);
       cerr << msg << endl;
       while (firead_errmsg(msg)) cerr << msg << endl;
       planck_fail("FITS error");
       }
    */
  }

  //_____________________________________________________________________________
  //                                                                 getAllErrors	
  
  /*!
    \brief Get all FITS error messages on the stack
    
    \return errors - vector of strings of error messages on the FITS stack
  */
  std::vector<std::string> rmFITS::getAllErrors ()
  {
    char message[MAX_MESSAGE_LENGTH];
    std::vector<std::string> errors;
    
    // Since the newest FITS error is on top of the stack, push_back will give us the same order in the vector
    fits_get_errstatus(fitsstatus, message);
    while(fitsstatus)
      {
	fits_read_errmsg(message);
	errors.push_back(static_cast<string>(message));	
      }
    
    return errors;
  }
  
  //_____________________________________________________________________________
  //                                                               writeErrorMark	
  
  /*!
    \brief Write error mark on the FITS error stack
    
    The fits write errmark routine puts an invisible marker on the CFITSIO error
    stack. The fits clear errmark routine can then be used to delete any more
    recent error messages on the stack, back to the position of the marker. This
    preserves any older error messages on the stack. The fits clear errmsg
    routine simply clears all the messages (and marks) from the stack. These
    routines are called without any arguments.
  */
  void rmFITS::writeErrorMark(void)
  {
    /* This routine does not set the fitsstatus, nor does it return 
       success/failure */
    fits_write_errmark();
  }
  
  
  //_____________________________________________________________________________
  //                                                               clearErrorMark	
  
  /*!
    \brief Clear error mark on the FITS error stack	
  */
  void rmFITS::clearErrorMark(void)
  {	
    fits_clear_errmark();		// this routine does not set the fitsstatus, nor does it return success/failure
  }	  
  
  /*!
    \brief Clear all FITS error messages (and marks) on the stack
  */
  void rmFITS::clearErrorMessages() 
  {  
    fits_clear_errmsg();
  }
  
  
  //_____________________________________________________________________________
  //                                                               printAllErrors	
  
  /*!
    \brief Get the corresponding cfits error message to the current fitsstatus
    
    \return fitserrormsg - Complete string with all FITS errors on the error stack
  */
  void rmFITS::printAllErrors()
  {
    char fits_error_message[81];
    std::string complete_error_message;

    fits_get_errstatus(fitsstatus, fits_error_message);	// returns descriptive text string (30 char max.) corresponding to a CFITSIO error status code

    cerr << fits_error_message << endl;		// print error message on cerr

    while (fits_read_errmsg(fits_error_message)!=0)
    {
    	cerr << fits_error_message << endl;					// print further messages
     // complete_error_message += fits_error_message;	// concat complete error msg
    }

    /* Martin's code
    void fitshandle::check_errors() const
    {
    if (status==0) return;
    char msg[81];
    fits_get_errstatus (status, msg);
    cerr << msg << endl;
    while (firead_errmsg(msg)) cerr << msg << endl;
    planck_fail("FITS error");
    }
    */
  }

  //_____________________________________________________________________________
  //                                                                   getNumHDUs

  /*!
    \return numHDUS - number of HDUs in FITS file
  */
  int rmFITS::getNumHDUs ()
  {
    int nofHDUs=0;	// number of hdus in FITS file
    
    if (fits_get_num_hdus(fptr, &nofHDUs ,&fitsstatus))
      {
        throw "rmFITS::getNumHDUs";
      }
    
    return nofHDUs;
  }
  
  //_____________________________________________________________________________
  //                                                              moveAbsoluteHDU
  
  /*!
    \param hdu - move to HDU number
    
    \return hdutype - type of HDU moved to
  */
  void rmFITS::moveAbsoluteHDU(int hdu)
  {
    if (fits_movabs_hdu(fptr, hdu, NULL, &fitsstatus)) {
      throw "rmFITS::moveAbsoluteHDU";
    }
    
    /* If it is an image extension, update dimensions-vector. */
    if (getHDUType()==IMAGE_HDU) {
        updateImageDimensions();
      }
  }
  
  //_____________________________________________________________________________
  //                                                              moveRelativeHDU	
  
  /*!
    \brief Move relative by hdu units
    
    \param hdu - move relative number of HDUs
  */
  void rmFITS::moveRelativeHDU(int nhdu)
  {
    if (fits_movrel_hdu(fptr, nhdu, NULL, &fitsstatus))	// try to move nhdu
      {
        throw "rmFITS::moveRelativeHDU";
    }

    if (getHDUType()==IMAGE_HDU)	 // if it is an image extension
    {
       updateImageDimensions();		// Update dimensions-vector,
    }
  }

  //_____________________________________________________________________________
  //                                                                getCurrentHDU

  /*!
    \return chdu - current HDU in FITS file
  */
  int rmFITS::getCurrentHDU()
  {
    int hdupos=0;		// local variable to hold chdu

    if (fits_get_hdu_num(fptr, &hdupos))
    {
    	throw "rmFITS::getCurrentHDU";
    }

    return hdupos;		// return to caller
  }

  //_____________________________________________________________________________
  //                                                                  moveNameHDU

  /*!
    \param extname - extension name to move to

    \return hdutype - Type of HDU moved to
  */
  int rmFITS::moveNameHDU(const std::string &extname)
  {
    int hdutype=0;			// type of HDU

    // ignoring the version number of the extension
    if (fits_movnam_hdu(fptr, hdutype, const_cast<char*>(extname.c_str()) , NULL, &fitsstatus))
      {
        throw "rmFITS::moveNameHDU";
      }

    if (getHDUType()==IMAGE_HDU)	 // if it is an image extension
      {
        updateImageDimensions();		// Update dimensions-vector,
      }

    return hdutype;
  }
  
  //_____________________________________________________________________________
  //                                                                  moveNameHDU
  
  /*!
    \brief Write the current HDU to the output stream outstream
    
    \param outstream - file output stream to write to
  */
  void rmFITS::writeHDU(ostream &outstream)
  {
    //TODO
    if(!outstream.good()) {
      throw "rmFITS::writeHDU outstream is not ready for I/O";
    }
  }
  
  //_____________________________________________________________________________
  //                                                                         getX	
  
  /*!
    \brief Get the X dimension of the FITS image
    
    \return X - X dimension of an image
  */
  int64_t rmFITS::getX()
  {
    if(dimensions.size()==0)
      throw "rmFITS::X dimensions not set";
    else
      return this->dimensions[0];
  }
  
  //_____________________________________________________________________________
  //                                                                         getY	
  
  /*!
    \brief Get the Y dimension of the FITS image
    
    \return Y - Y dimension of an image
  */	
  int64_t rmFITS::getY()
  {
    if(dimensions.size()==0)
      throw "rmFITS::Y dimensions not set";
    else		
      return this->dimensions[1];
  }
  
  //_____________________________________________________________________________
  // 		                                                             getZ	
  
  /*!
    \brief Get the Z dimension of the FITS image
    
    \return Z - Z dimension of an image
  */	
  int64_t rmFITS::getZ()
  {
    if(dimensions.size()==0)
      throw "rmFITS::Y dimensions not set";
    else
      return this->dimensions[2];
  }
  
  
  //_____________________________________________________________________________
  // 							       getImageDimensions	
  
  /*!
    \brief Get image dimensions
    
    \return dim - vector of length equal the number of axes and in each entry length of that axis
  */
  vector<int64_t> rmFITS::getImageDimensions()
  {
    updateImageDimensions();
    return this->dimensions;
  }
  
  
  //_____________________________________________________________________________
  //                                                        updateImageDimensions	
  
  /*!
    \brief Update the information contained in the dimensions-vector of the rmFITS object
  */
  void rmFITS::updateImageDimensions()
  {
    int i=0;
    int naxis=0;
    long *naxes=0;
    
    if (getHDUType()==IMAGE_HDU) {
      naxis=getImgDim();			// get number of axis
      naxes=(long*) calloc(sizeof(long), naxis);// allocate memory for axis dimensions
      
      dimensions.resize(naxis);		// resize dimensions vector
      getImgSize(naxis, naxes);		// get the size of each axis
      
      for (i=0; i<naxis; i++)
	{
	  dimensions[i]=naxes[i];
	}
    }
  }
  
  //_____________________________________________________________________________
  //                                                        updateImageDimensions	
  
  /*!
    \brief Read type of HDU (IMAGE_HDU, ASCII_TBL, or BINARY_TBL)
    
    \return hduype - Type of current HDU
  */
  int rmFITS::getHDUType()
  {
    int hdutype=0;

    if (fits_get_hdu_type(fptr,  &hdutype, &fitsstatus))
	 {
       throw "rmFITS::getDUType";
    }

    return hdutype;
  }

	
  //_____________________________________________________________________________
  //                                                                  getFilename	

  /*!
    \brief Read the filename of the currently opened FITS file

    \return filename - Name of FITS file currently opened in rmFITS object
  */
  std::string rmFITS::getFilename()
  {
    std::string filename;	// local variable to hold FITS filename

    if (fits_file_name(fptr, const_cast<char *>(filename.c_str()), &fitsstatus))
    {
        throw "rmFITS::getFilename";
    }

    return filename;
  }

	
  //_____________________________________________________________________________
  //                                                                  getFileMode

  /*!
    \brief Read the IO mode of the currently opened FITS file
	
	 \return mode - file I/0 mode of FITS file (0=READONLY, 1=READWRITE, -1=error)
  */
  int rmFITS::getFileMode()
  {
    int mode=-1;

    if (fits_file_mode(fptr, &mode, &fitsstatus))
    {
        throw "rmFITS::getFileMode";
    }

    return mode;
  }

	
  //_____________________________________________________________________________
  //                                                                   getURLType	

  /*!
    \brief Read the url type, e.g. file://, ftp:// of the currently opened FITS file
  */
  std::string rmFITS::getURLType()
  {
    string urltype;

    if (fits_url_type(fptr, const_cast<char *> (urltype.c_str()), &fitsstatus))
    {
        throw "rmFITS::getURLType";
    }

    return urltype;
  }

	
  //_____________________________________________________________________________
  //                                                               deleteFITSfile	

  /*!
    \brief Delete the fitsfile of the rmFITS object
  */
  void rmFITS::deleteFITSfile()
  {
    if (fits_delete_file(fptr, &fitsstatus))
	 {
        throw "rmFITS::deleteFITSfile";
    }
  }

  //_____________________________________________________________________________
  //                                                           preemptivelyDelete
	
  /*!
	 \brief Preemptively delete an existing FITS file
	 
	 \param filename - name of file to check for preemptive deletion
  */
  void rmFITS::preemptivelyDelete(const string &filename)
  {
		int exists=0;

 	  //------------------------------------------------------------------
	  // Preemptively delete FITS file
	  fits_file_exists(filename.c_str(), &exists, &this->fitsstatus);
	  if(exists)				// Check if it exists...
	  {
	     fits_open_file(&this->fptr, filename.c_str(), READWRITE, &this->fitsstatus);	// need to open it, since delete only works on fileptr not filename		
		  if(this->fitsstatus)
		  {
		     fits_get_errstatus(this->fitsstatus, this->fits_error_message);
			  throw "rmFITS::merge2Dto3D could not open output file for deletion";			//  throw exception				
		  }
		  fits_delete_file(this->fptr, &this->fitsstatus);									// ... then delete it
		  if(this->fitsstatus)
		  {
		     fits_get_errstatus(this->fitsstatus, this->fits_error_message);	
			  throw "rmFITS::merge2Dto3D could not delete old output file";    //  throw exception				
		  }			
		}
  }	
	
	
  //___________________________________________________________________________
  //																					flushFITSfile
	
  /*!
    \brief Flush the FITS file, close and reopen
  */
  void rmFITS::flushFITSfile()
  {
    if (fits_flush_file(fptr, &fitsstatus))
      {
        throw "rmFITS::fits_flush_file";
      }
  }

	
  //___________________________________________________________________________
  //																					flushFITSfile	

  /*!
    \brief Flush buffer (without proper closing and reopening)
  */
  void rmFITS::flushFITSBuffer()
  {
    if (fits_flush_buffer(fptr, 0, &fitsstatus))
      {
        throw "rmFITS::fitsFITSBbuffer";
      }
  }

	//___________________________________________________________________________
	//																					     getNulval	 
	/*!
	   \brief Get the current nulval setting of the FITS object
	   \return double nulval - current nulvalue
	*/
	double rmFITS::getNulval()
	{
		return this->nulval;
	}
	
 
  //___________________________________________________________________________
  //																					     setNulval	 
  /*!
     \brief Set the current nulval setting of the FITS object
	  \param nulvalue - new nulval to set in FITS object
  */
  void rmFITS::setNulval(double nulvalue)
  {
		this->nulval=nulvalue;
  }	

	
  //___________________________________________________________________________
  //																					     getAnynul	
	
  /*!
     \brief Get anynul value
   
	  \return anynul - indicator if there has been any nulvalue read or written
  */
  int rmFITS::getAnynul(void)
  {
     return this->anynul;		
  }
	
	
  // ============================================================================
  //
  //	Image access functions
  //
  // ============================================================================

	
  //_____________________________________________________________________________
  //                                                                   getImgType	
	
  /*!
      \brief Get image type of the FITS image

      \return bitpix - Bits per pixel
   */
  int rmFITS::getImgType()
  {
    int bitpix=0;

    if (fits_get_img_type(fptr, &bitpix, &fitsstatus))
	 {
       throw "rmFITS::getImgType";
	 }

    return bitpix;	// pass on cfitsio return value
  }


  //_____________________________________________________________________________
  //                                                                    getImgDim

  /*!
    \brief Get image dimensions of the FITS image

    \return naxis - Number of axes in image
  */
  int rmFITS::getImgDim()
  {
    int naxis=0;

    if (fits_get_img_dim(fptr, &naxis,  &fitsstatus))
	 {
	    throw "rmFITS::getImgDim";
    }

    dimensions.resize(naxis);	// resize dimensions vector in rmFITS object

    return naxis;	// return number of axes
  }

	
  //___________________________________________________________________________
  //                                                                 getImgSize
    
  /*!
     \brief Get image size of the FITS image
      
	  This functions will also update rmFITS object
  */
  std::vector<int64_t> rmFITS::getImgSize()
  {
    unsigned int i=0;			// loop variable
    int maxdim=getImgDim();	// maximum number of dimensions
    long *naxes=(long *) calloc(maxdim, sizeof(long));

    if (fits_get_img_size(fptr, maxdim, naxes , &fitsstatus))
	 {
        throw "rmFITS::getImageSize";
    }

	 dimensions.resize(maxdim); 
    for (i=0; i<dimensions.size(); i++)
      dimensions[i]=naxes[i];
	  
	 return dimensions;
  }

    
  //___________________________________________________________________________
  //	 																				  getImgSize

  /*!
     \brief Get image size of the FITS image (legacy interface)
      
     \param maxdim - Maximum number of dimensions
     \param &naxes - Array to hold axes lengths
  */
  void rmFITS::getImgSize(int maxdim,  long *naxes)
  {
     unsigned int i=0;		// loop variable
      
     if (fits_get_img_size(fptr, maxdim, naxes , &fitsstatus))
	  {
	    throw "rmFITS::getImageSize";
	  }
      
	  dimensions.resize(maxdim);
	  for (i=0; i<dimensions.size(); i++)
	     dimensions[i]=naxes[i];
  }

	
  //___________________________________________________________________________
  //                                                                getImgParam	
    
  /*!
      \brief Get image parameters: maxdim, bitpix, naxis, naxes
      
    \param maxdim - Maximum number of dimensions returned
    \param &bitpix - Bits per pixel
    \param &naxis - Number of axes
    \param *naxes - Array with length of each axis
  */
  void rmFITS::getImgParam(int maxdim,  int &bitpix, int &naxis, long *naxes)
  {
    if (fits_get_img_param(fptr, maxdim, &bitpix, &naxis, naxes, &fitsstatus))
	 {
        throw "rmFITS::getImageParam";
    }
  }

	
  //___________________________________________________________________________
  //                                                                  createImg
  /*!
    \brief Create an image extension (legacy interface)

    \param bitpix - Bits per pixel
    \param naxis - Number of axes
    \param *naxes - Array with length of each axis
  */
  void rmFITS::createImg(int bitpix, int naxis, long *naxes)
  {
    //-----------------------------------------------------------
    // Check input parameters
    //
    if(bitpix != -32)		// we currently only support TDOUBLE FITS images
    {
      throw "rmFITS::createImg bitpix is not TDOUBLE_IMG";
    }
    if (naxis==0 || naxes==NULL)
    {
        throw "rmFITS::createImg naxis or naxes NULL";
    }

    //-----------------------------------------------------------
    if (fits_create_img(fptr, bitpix, naxis , naxes, &fitsstatus))
    {
        throw "rmFITS::createImg";
    }
  }

  
  //___________________________________________________________________________
  //                                                                  createImg	
  /*!
    \brief Create an image extension
    
    \param bitpix - Bits per pixel
    \param dimensions - Image dimension vector
  */
  void rmFITS::createImg(int bitpix,
			 std::vector<int64_t> &dimensions)
  {
    long naxis=0;
    long *naxes=NULL;
    
    if(bitpix != -32)		// we currently only support TDOUBLE FITS images
      {
	throw "rmFITS::createImg bitpix is not TDOUBLE_IMG";
      }
    if(dimensions.size()==0)
      throw "rmFITS::createImg dimensions is 0";
    
    naxis=dimensions.size();
    naxes=static_cast<long *>(calloc(naxis, sizeof(long)));
    
    for(long i=0; i < naxis; i++)
      naxes[i]=dimensions[i];
    
    if (fits_create_img(fptr, bitpix, naxis , naxes, &fitsstatus))
      {
	throw "rmFITS::createImg";
      }
    
    free(naxes);
  }
  
  
  //___________________________________________________________________________
  //                                                                   writePix
  
  /*!
    \brief Write nelements pixels to FITS image extension
    
    \param datatype - Datatype of elements in array
    \param *fpixel - Starting pixel location
    \param nelements - Number of elements to write
    \param *array - Array containing data
  */
  void rmFITS::writePix(int datatype, long *fpixel, long nelements, void *array)
  {
    if (fits_write_pix(fptr, datatype, fpixel, nelements, array, &fitsstatus))
      {
        throw "rmFITS::writePix";
      }
  }
  
  
  //___________________________________________________________________________
  //                                                                   writePix	

  /*!
    \brief Write pixels to FITS image (any undefined value is replaced by nulval)

    \param datatype - Datatype of elements in array
    \param *fpixel - Starting pixel location
    \param nelements - Number of elements to write
    \param *array - Array containing data
    \param *nulval - Nullvalue to be written to file
  */
  void rmFITS::writePixNull(int datatype, long *fpixel, long nelements, void *array, void *nulval)
  {
    if (fits_write_pixnull(fptr, datatype, fpixel , nelements, array, nulval, &fitsstatus))
	 {
        throw "rmFITS::writePix";
    }
  }

	
  //___________________________________________________________________________
  //                                                                    readPix	
	
  /*!
    \brief Read pixels from an FITS image

    \param datatype - Datatype of elements in array
    \param *fpixel - Axes direction definition
    \param nelements - Number of elements to write
    \param *nulval - Nullvalue to be written to file
    \param *array - Array containing data
    \param *anynul - If any null value was encountered
  */
  void rmFITS::readPix(int datatype, long *fpixel, long nelements, void *nulval, void *array, int *anynul)
  {
    if (fits_read_pix(fptr, datatype, fpixel, nelements, nulval, array, anynul, &fitsstatus))
      {
        throw "rmFITS::readPix";
      }
  }

	
  //___________________________________________________________________________
  //                                                                    readPix
	
  /*!
    \brief Read pixels from an FITS image
	 
    \param datatype - Datatype of elements in array
    \param *fpixel - Axes direction definition
    \param nelements - Number of elements to write
    \param *array - Array containing data
	*/
	void rmFITS::readPix(int datatype, long *fpixel, long nelements, void *array)
	{
		if (fits_read_pix(fptr, datatype, fpixel, nelements, &nulval, array, &anynul, &fitsstatus))
      {
			throw "rmFITS::readPix";
      }
	}	
	

  //___________________________________________________________________________
  //                                                                 readSubset	
	
  /*!
    \brief Read a subset of a FITS image

    \param datatype - datatype contained in array
    \param *fpixel - array giving lower left corner of reading
    \param *lpixel - array giving upper right corner of reading
    \param *inc - increment step
    \param *nulval - nulval used if NULL pixels are read
    \param *array - array to read into
    \param *anynul - if any null vallue was encountered
  */
  void rmFITS::readSubset(int  datatype, long *fpixel,
                           long *lpixel, long *inc, void *nulval,  void *array,
                           int *anynul)
  {
		fits_read_subset(fptr, datatype, fpixel, lpixel, inc, nulval, array, anynul, &fitsstatus);
		if(fitsstatus) 
		{
	  		fits_get_errstatus(fitsstatus, fits_error_message);
	  		cout << fits_error_message << endl;
	  		throw "rmFITS::readSubset";
		}
  }

	
  //___________________________________________________________________________
  //                                                                 readSubset	

  /*!
    \brief Read a subset of a FITS image into a vector

    \param datatype - datatype contained in array
    \param *fpixel - array giving lower left corner of reading
    \param *lpixel - array giving upper right corner of reading
    \param *inc - increment step
    \param *nulval - nulval used if NULL pixels are read
    \param vec - vector to read into
    \param *anynul - if any null vallue was encountered
  */
  void rmFITS::readSubset(int  datatype, long *fpixel,
                           long *lpixel, long *inc, void *nulval,  vector<double> &vec,
                           int *anynul)
  {
      unsigned int nelements=1;		// compute number of elements to read
  
      if(fpixel==NULL)
			throw "rmFITS::readSubset lpixel is NULL pointer";
      if(lpixel==NULL)
			throw "rmFITS::readSubset fpixel is NULL pointer";
      //-------------------------------------------------------

		for(unsigned int i=0; i < dimensions.size(); i++)
      {
	 		nelements*=(lpixel[i]-fpixel[i]+1);
      }
      cout.flush();
  
      if(nelements > vec.size())
			throw "rmFITS::readSubset nelements to read exceeds vec.size()";
  
      cout << "nelements = " << nelements << endl;
      cout << "vev.size() = " << vec.size() << endl;
		cout << fpixel[0] << "," << fpixel[1] << ","<< fpixel[2] << endl;
		cout << lpixel[0] << "," << lpixel[1] << "," <<lpixel[2] << endl;
		cout << "nulval = " << nulval << endl;

//		for(unsigned int i=0; i < vec.size(); i++)
//			cout << vec[i] << endl;

//     fits_read_subset(fptr, datatype, fpixel, lpixel, inc, nulval, static_cast<void*>(&vec[0]), anynul, &fitsstatus);
	//	fits_read_subset_dbl(fptr, );
      if(fitsstatus) 
      {
			fits_get_errstatus(fitsstatus, fits_error_message);
			cout << fits_error_message << endl;
        throw "rmFITS::readSubset";
      }
  }

	
  //___________________________________________________________________________
  //                                                                writeSubset	
	
  /*!
      \brief Write a subset to a FITS image

      \param datatype - datatype contained in array
      \param *fpixel - array giving lower left corner of reading
      \param *lpixel - array giving upper right corner of writing
      \param *array - array containing data
  */
  void rmFITS::writeSubset(int datatype, long *fpixel, long *lpixel, double *array)
  {
    if (fits_write_subset(fptr, datatype, fpixel, lpixel, array, &fitsstatus))
      {
        throw "rmFITS::writeSubset";
      }
  }




  // ============================================================================
  //
  //  RM-Cube Input functions
  //
  // ============================================================================

/*!
	\brief Read a 2D slice from a FITS cube, is more similar to the dimensionality of arrays 
	in Fortran rather than C. For instance if a FITS image has NAXIS1 = 100 and NAXIS2 = 50, 
	then a 2-D array just large enough to hold the image should be declared as array[50][100] 
	and not as array[100][50].
	
	\param array - 2-dimensional array to read into
	\param dim1 - first dimension of array (corresponds to y-axis of image)
*/
void rmFITS::read2D(double *array, unsigned long long dim1)
{
	double nulval=0;		// null value
	int anynul=0;			// indicater if any null value was returned from image
	long group=0;			// What is this parameter?
	
	//**********************************************************
	// Check parameters
	if(array==NULL)
		throw "rmFITS::read2D array is NULL";
	if(dim1<=0)
		throw "rmFITS::read2D dim1 is <= 0";
	
	//**********************************************************
	// get naxis1 and naxis2 from image dimensions
	getImgDim();

	if(dimensions[0]<=0)
		throw "rmFITS::read2D naxis1 <= 0";
	if(dimensions[0]<=0)
		throw "rmFITS::read2D naxis2 <= 0";
		
	
	fits_read_2d_dbl(fptr, group, nulval, dim1, dimensions[0], dimensions[1], array, &anynul, &fitsstatus);
	if(fitsstatus)
	{
		fits_get_errstatus(fitsstatus, fits_error_message);		
		cout << fits_error_message << endl;
		throw "rmFITS::read2D failed";		
	}
}


/*!
	\brief Write a 2D slice to a FITS cube, is more similar to the dimensionality of arrays 
	in Fortran rather than C. For instance if a FITS image has NAXIS1 = 100 and NAXIS2 = 50, 
	then a 2-D array just large enough to hold the image should be declared as array[50][100] 
	and not as array[100][50].
	
	\param array - 2-dimensional array to write
	\param dim1 - first dimension of array (corresponds to y-axis of image)
*/
void rmFITS::write2D(double *array, const long long dim1)
{
	long group=0;			// What is this parameter?
	
	//**********************************************************
	// Check parameters
	if(array==NULL)
		throw "rmFITS::write2D array is NULL";
	if(dim1<=0)
		throw "rmFITS::write2D dim1 is <= 0";
	
	//**********************************************************	
	fits_write_2d_dbl(fptr, group, dim1, dimensions[0], dimensions[1], array, &fitsstatus);
	if(fitsstatus)
	{
		fits_get_errstatus(fitsstatus, fits_error_message);		
		cout << fits_error_message << endl;
		throw "rmFITS::write2D failed";		
	}
}

  /*!
    \brief Read a line from a FITS image

    \param line - vector holding the data read from the image
    \param x - x axis lower left corner position to read line from
    \param y - y axis lower left corner position to read line from
	 \param inc - increment of elements to skip
	 \param nulval - value used for nulls found in image
  */
  void rmFITS::readLine (vector<double> &line,
			  const unsigned long x,
			  const unsigned long y,
			  long *inc,
			  void *nulval)
  {
    long fpixel[3];
    long lpixel[3];
    int anynul=0;

    // Check if vector has right dimension, same as z dimension of cube
    if (getHDUType()!=IMAGE_HDU)	 // Check if current HDU is an image extension
    {
        throw "rmFITS::readLine CHDU is not an image";
    }

    // Define first pixel to read, read along one line of sight
    fpixel[0]=x;
    fpixel[1]=y;
    fpixel[2]=1;
    lpixel[0]=x;
    lpixel[1]=y;
    lpixel[2]=dimensions[2];

	 //-------------------------------------------------------
	 // Check consistency of pixel data
	 //
	 if(x > static_cast<unsigned int>(dimensions[0]))
		throw "rmFITS::readLine x is out of range";
	 if(y > static_cast<unsigned int>(dimensions[1]))
		throw "rmFITS::readLine y is out of range";
	 if(dimensions[2]==0)
		throw "rmFITS::readLine image is only 2-D";
    //-------------------------------------------------------
    //line.resize(dimensions[2]);
    // Read subset from FITS file
    readSubset(TDOUBLE, fpixel, lpixel, inc, nulval, line, &anynul);   	 
  	 if(fitsstatus)
    {
		fits_get_errstatus(fitsstatus, fits_error_message);		
		cout << fits_error_message << endl;
		throw "rmFITS::readLine readSubset failed";
    }	
}


 /*!
   \brief Read a line from a FITS image (specifying increment and nulval)

   \param line - double array holding the data read from the image
   \param x - x axis lower left corner position to read line from
   \param y - y axis lower left corner position to read line from
 	\param inc - increment of elements to skip
 	\param nulval - value used for nulls found in image
 */
 void rmFITS::readLine (double *line,
		  const unsigned long x,
		  const unsigned long y,
		  long *inc,
		  void *nulval)
 {
   long fpixel[3];
   long lpixel[3];
   int anynul=0;

   // Check if vector has right dimension, same as z dimension of cube
   if (getHDUType()!=IMAGE_HDU)	 // Check if current HDU is an image extension
   {
       throw "rmFITS::readLine CHDU is not an image";
   }

   // Define first pixel to read, read along one line of sight
   fpixel[0]=x;
   fpixel[1]=y;
   fpixel[2]=1;
   lpixel[0]=x;
   lpixel[1]=y;
   lpixel[2]=dimensions[2];

 //-------------------------------------------------------
 // Check consistency of pixel data
 //
 if(x > static_cast<unsigned int>(dimensions[0]))
	throw "rmFITS::readLine x is out of range";
 if(y > static_cast<unsigned int>(dimensions[1]))
	throw "rmFITS::readLine y is out of range";
 if(line==NULL)
	throw "rmFITS::readLine line is NULL pointer";
 if(inc==NULL) 
	throw "rmFITS::readLine inc is NULL pointer";


	// void rmFITS::readSubset(int  datatype, long *fpixel,
	//	                           long *lpixel, long *inc, void *nulval,  void *array,
	//                           int *anynul)

 	//-------------------------------------------------------
   // Read subset from FITS file
   readSubset(TFLOAT, fpixel, lpixel, inc, nulval, line, &anynul);
   if(fitsstatus)
   {
		fits_get_errstatus(fitsstatus, fits_error_message);		
		cout << fits_error_message << endl;
		throw "rmFITS::readLine readSubset failed";
   }	
}


/*!
    \brief Read a line from a FITS image along the Z-axis

    \param line - double array holding the data read from the image
    \param x - x axis lower left corner position to read line from
    \param y - y axis lower left corner position to read line from
*/
void rmFITS::readZLine (double *line,
			  					const unsigned long x,
			  					const unsigned long y,
								void *nulval)
{
    long fpixel[3];
    long lpixel[3];
	 long inc[3]={0,0,1};	// default increment is along Z axis
//	 double nulval=0;			// default null value is a double 0
//  int anynul=0;

    // Check if vector has right dimension, same as z dimension of cube
    if (getHDUType()!=IMAGE_HDU)	 // Check if current HDU is an image extension
    {
        throw "rmFITS::readLine CHDU is not an image";
    }
	
	 // Check if no nulval was given, use class' default nulval
	 if(nulval==NULL)
	    nulval=&this->nulval;
	
    // Define first pixel to read, read along one line of sight
    fpixel[0]=x;
    fpixel[1]=y;
    fpixel[2]=1;
    lpixel[0]=x;
    lpixel[1]=y;
    lpixel[2]=dimensions[2];

	 //-------------------------------------------------------
	 // Check consistency of pixel data
	 //
	 if(x > static_cast<unsigned int>(dimensions[0]))
		throw "rmFITS::readLine x is out of range";
	 if(y > static_cast<unsigned int>(dimensions[1]))
		throw "rmFITS::readLine y is out of range";
  	 if(line==NULL)
		throw "rmFITS::readLine NULL pointer";
   
//	 void rmFITS::readSubset(int  datatype, long *fpixel,
//		                           long *lpixel, long *inc, void *nulval,  void *array,
//	                           int *anynul)

	 //-------------------------------------------------------
    // Read subset from FITS file
    readSubset(TDOUBLE, &fpixel[0], &lpixel[0], &inc[0], &nulval, line, &anynul);
    if(fitsstatus)
    {
		fits_get_errstatus(fitsstatus, fits_error_message);		
		cout << fits_error_message << endl;
		throw "rmFITS::readLine readSubset failed";
    }	
  }

	
  //_____________________________________________________________________________
  //                                                                    readPlane
  /*
     \brief Read a 2-D plane from a FITS cube
	  \param double *plane - pointer to hold values from FITS plane
	  \param void *nulval - (optional) pointer to nulval holding NULL value substitute
  */
  void rmFITS::readPlane(double *plane, const unsigned long z, void *nulval)
  {
		long fpixel[3];		// read vector where reading starts
		int hdutype=0;			// HDU type which is checked for
		int nelements=0;		// number of elements to read
//		int anynul=0;			// indicator if any nul value has been read
		int naxis=0;			// number axes present in image
		long naxes[3];			// dimensions of these axes
		
		//-------------------------------------------------------------
		if (fits_get_hdu_type(this->fptr, &hdutype ,&this->fitsstatus)!=IMAGE_HDU)	// Check if current HDU is an image extension
		{
			throw "fitsmerge::readPlane CHDU is not an image";
		}

	   if(nulval==NULL)							// if no nulval was given as parameter...
			nulval=(void*) &this->nulval;		// .. use default one in class definition
	  
		// Read from FITS file one plane at depth z
		fpixel[0]=1;
		fpixel[1]=1;
		fpixel[2]=z;
		
		fits_get_img_dim(this->fptr, &naxis, &this->fitsstatus);
		fits_get_img_size(this->fptr, naxis, naxes, &this->fitsstatus); // get image dimensions
		
		nelements=naxes[0]*naxes[1];	// compute number of elements in plane
		
		fits_file_name(this->fptr, fits_error_message, &this->fitsstatus);
		
		if (plane!=NULL)	// only if valid pointer is given
		{
			fits_read_pix(this->fptr, TDOUBLE, fpixel, nelements, nulval, plane, &this->anynul, &fitsstatus);
		}
		else
		{
			throw "fitsmerge::readPlane pointer is NULL";
		}
  }


//_____________________________________________________________________________
//                                                                     readCube

/*!
  \brief Read a cube from a FITS image
  
  \param cube - pointer to memory to hold data
  \param nulval - NULL value used (optional)
*/
void rmFITS::readCube (double *cube,
		       void *nulval)
{
  long naxis=0;				// number of axis
  long *fpixel=NULL;		// first pixel to read
  long *lpixel=NULL;		// last pixel to read
		long *inc=NULL;			// increment
		
		//-------------------------------------------------------		
		naxis=getImgDim();
		if(naxis<=0)
			throw "rmFITS::readCube naxis <= 0";
		else
		{
			fpixel=static_cast<long *>(calloc(naxis, sizeof(long)));
			lpixel=static_cast<long *>(calloc(naxis, sizeof(long)));
			inc=static_cast<long *>(calloc(naxis, sizeof(long)));
		}
		
		for(long i=0; i<naxis; i++)
		{
			fpixel[i]=1;
			lpixel[i]=dimensions[i];
			inc[i]=1;							// default increment is 1
		}

		// Read subset from FITS file
		readSubset(TDOUBLE, &fpixel[0], &lpixel[0], &inc[0], &nulval, cube, &anynul);
		if(fitsstatus)
		{
			fits_get_errstatus(fitsstatus, fits_error_message);		
			cout << fits_error_message << endl;
			throw "rmFITS::readSubCube readSubset failed";
		}
   }

	
   //_____________________________________________________________________________
   //                                                                  readSubCube

   /*!
    \brief Read a subCube from a FITS image

    \param *subCube - pointer to array holding the data read from the image
    \param x_pos - lower left corner x position of subCube
    \param y_pos - lower left corner y position of subCube
    \param x_size - size in x direction in pixels
    \param y_size - size in y direction in pixels
  */
  void rmFITS::readSubCube (double *subCube,
                            unsigned long x_pos,
                            unsigned long y_pos,
                            unsigned long x_size,
                            unsigned long y_size,
									 void *nulvalue)
  {
    long fpixel[3];	// first pixel definition
    long lpixel[3];	// last pixel definition
	 long inc[3]={0,0,1};
	 double nulval=0.0;
	 int anynul=0;

	 //-------------------------------------------------------
	 // Check consistency of pixel data
	 //
	 if(x_pos > static_cast<unsigned int>(dimensions[0]))
		throw "rmFITS::readSubCube x_pos is out of range";
	 if(y_pos > static_cast<unsigned int>(dimensions[1]))
		throw "rmFITS::readSubCube y_pos is out of range";
	 if(x_pos+x_size > static_cast<unsigned int>(dimensions[0]))
		throw "rmFITS::readSubCube x_size is out of range";
	 if(y_pos+y_size > static_cast<unsigned int>(dimensions[1]))
		throw "rmFITS::readSubCube y_size is out of range";
  	 if(subCube==NULL)
		throw "rmFITS::readSubCube NULL pointer";

	 //-------------------------------------------------------
	 if(nulvalue==NULL)			// if no nulval was given as parameter...
		 nulval=this->nulval;	// ... use class default (double nulval=0.0)
	  
	  
    fpixel[0]=x_pos;
	 fpixel[1]=y_pos;
	 fpixel[2]=1;

	 lpixel[0]=dimensions[0]+x_size;
	 lpixel[1]=dimensions[1]+y_size;
	 lpixel[2]=dimensions[2];

    if (getHDUType()!=IMAGE_HDU)   // Check if current HDU is an image extension
    {
    	throw "rmFITS::readSubCube CHDU is not an image";
    }

	//	 void rmFITS::readSubset(int  datatype, long *fpixel,
	//		                           long *lpixel, long *inc, void *nulval,  void *array,
	//	                           int *anynul)

	 //-------------------------------------------------------
    // Read subset from FITS file
    readSubset(TDOUBLE, &fpixel[0], &lpixel[0], &inc[0], &nulval, subCube, &anynul);
    if(fitsstatus)
    {
		fits_get_errstatus(fitsstatus, fits_error_message);		
		cout << fits_error_message << endl;
		throw "rmFITS::readSubCube readSubset failed";
    }

  }


  // ============================================================================
  //
  //  RM-Cube output functions
  //
  // ============================================================================


	
	/*!
	 \brief Write an entire FITS cube (can be higher-dimensional) into a FITS file
	 
	 \param cube - pointer to float containing the cube data
	 \param naxis - number of axis
	 \param naxes - dimension of each axis
	*/
	void rmFITS::writeCube(double *cube, void *nulval)
	{
		unsigned long nelements=1;	// number of elements to write
//		int fitsstatus=0;
//		char errormessage[MAX_MESSAGE_LENGTH];		// fits error message char array
		int naxis=0;
		long *naxes=NULL;
		long *fpixel;
		
		//----------------------------------------
		if(cube==NULL)
			throw "rmFITS::writeCube cube pointer is NULL";
		
		//----------------------------------------
		
		fits_get_img_dim(this->fptr, &naxis, &this->fitsstatus);			// get image dimensions
		if(this->fitsstatus)
		{
			fits_get_errstatus(this->fitsstatus, this->fits_error_message);
			cout << this->fits_error_message << endl;
			throw "rmFITS::writeCube fits_get_img_dim failed";		
		}
		
		naxes=new long[naxis];									// get memory for naxes to hold axes sizes 
		fits_get_img_size(this->fptr, naxis, naxes, &this->fitsstatus);		// get axes sizes
		
		fpixel=(long*)malloc(sizeof(long)*naxis);
		for(int i=0; i < naxis; i++)
			fpixel[i]=1;
		
		for(int i=0; i < naxis; i++)					// loop over (hyper)cube's axes
		{
			nelements*=naxes[i];								// compute nelements in cube
			cout << nelements << endl;
		}
		if(nelements==0)
			throw "fitsmerge::writeCube nelements is 0";
		
		fits_write_pixnull(this->fptr, TFLOAT, fpixel, nelements, cube, nulval, &this->fitsstatus);
		if(this->fitsstatus)
		{
			fits_get_errstatus(this->fitsstatus, this->fits_error_message);
			//cout << this->fits_error_message << endl;
			throw "rmFITS::writeCube failed";
		}
	}
	
	
	//_____________________________________________________________________________
	//                                                                   writePlane
	
	/*!
    \param *plane - array containing imageplane data
	 \param z		- z position to write plane to
    \param nulval - pointer to data to be substituted for any 0 values encountered (optional)
	 */
	void rmFITS::writePlane (double *plane, unsigned long z, void *nulval)
	{
		long fpixel[3]; 	// first pixel position to read
		long nelements=0;	// number of elements to write

		// check if plane counter is above limit
		if (z > (unsigned long) dimensions[2]) {
			throw "rmFITS::writePlane z out of range";
		}
		
		/* Check if current HDU is an image extension */
		if (getHDUType()!=IMAGE_HDU) {
			throw "rmFITS::writePlane CHDU is not an image";
		}
		
		// Read from FITS file one plane
		fpixel[0]=1;
		fpixel[1]=1;
		fpixel[2]=z;
		
		nelements=dimensions[0]*dimensions[1];	// compute nelements in plane
		
		// Write to FITS file
		if (plane==NULL)
		{
			throw "rmFITS::writePlane NULL pointer";
		}
		else if (nulval==NULL)
		{
			writePix(TDOUBLE, fpixel, nelements, plane);
		}
		else	// write pixels with substituting null value
		{
			writePixNull(TDOUBLE, fpixel, nelements, plane, nulval);
		}
   }
	
	//_____________________________________________________________________________
  //                                                                   writePlane
  /*!
    \param *plane - array containing imageplane data
    \param x - horizontal dimension in pixels
    \param y - vertical dimension in pixels
    \param z - z position to write plane to
    \param nulval - pointer to data to be substituted for any 0 values encountered
  */
  void rmFITS::writePlane (double *plane,
									unsigned long x,
                           unsigned long y,
                           unsigned long z,
									void *nulval)
  {
    long fpixel[3]; 	// first pixel position to read
    long nelements=0;	// number of elements to write
	  
    // Check if Faraday plane has the same x-/y-dimensions as naxes dimensions of FITS
	 updateImageDimensions();
    if ((int64_t)x > dimensions[0] || (int64_t)y > dimensions[1]) {
      throw "rmFITS::writePlane dimensions do not match";
    }
    
    // check if plane counter is above limit
    if (z > (unsigned) dimensions[2]) {
      throw "rmFITS::writePlane z out of range";
    }
    
    /* Check if current HDU is an image extension */
    if (getHDUType()!=IMAGE_HDU) {
      throw "rmFITS::writePlane CHDU is not an image";
    }
    
    // Read from FITS file one plane
    fpixel[0]=1;
    fpixel[1]=1;
    fpixel[2]=z;

    nelements=dimensions[0]*dimensions[1];	// compute nelements in plane

    // Write to FITS file
    if (plane==NULL)
    {
        throw "rmFITS::writePlane NULL pointer";
    }
    else if (nulval==NULL)
    {
        writePix(TDOUBLE, fpixel, nelements, plane);
    }
    else	// write pixels with substituting null value
    {
        writePixNull(TDOUBLE, fpixel, nelements, plane, nulval);
   }
  }


  //_____________________________________________________________________________
  //                                                                    writeLine

  /*!
    \param faradayLine - contains line of sight along Faraday Depths with RM intensity
    \param x - x position in pixels to write line to
    \param y - y position in pixels to write line to
  */
  void rmFITS::writeLine(double *line,
                          const long x,
                          const long y,
                          void *nulval)
  {


  }

  //_____________________________________________________________________________
  //                                                                    writeTile

  /*!
    \param tile - buffer containing tile data
    \param x_pos - x position in pixels to write tile to
    \param y_pos - y position in pixels to write tile to
    \param x_size - horizontal size of tile
    \param y_size - vertical size of tile
  */
  void rmFITS::writeTile(double* tile,
                          const long x_pos,
                          const long y_pos,
                          const long x_size,
                          const long y_size
                         )
  {

  }

  //_____________________________________________________________________________
  //                                                                 writeSubCube

  /*!
    \brief Write a SubCube to a FITS file

    \param cube - Array that contains data
    \param x_pos - x position in pixels to write cube to
    \param y_pos - y position in pixels to write cube to
    \param x_size - x-dimension of cube in pixels
    \param y_size - y-dimension of cube in pixels
  */
  void rmFITS::writeSubCube( double* subcube,
                              long x_size,
                              long y_size,
                              long x_pos,
                              long y_pos)
  {
    /* ATTENTION: temporary assignment to avoid compiler warnings! */
    x_size = y_size = x_pos = y_pos = 0;

    // Check if position is valid

    // check if size goes beyond limits of cube

    // Write to FITS file
  }

	
  //_____________________________________________________________________________
  //                                                                   printPlane	
  /*!
    \brief For debugging purposes output an image plane
	 
	 \param *plane - buffer containing image plane
	 \param x - x dimension of plane
	 \param y - y dimension of plane
  */
  void rmFITS::printPlane(const float *plane, const unsigned int x, const unsigned int y)
  {
	  if(plane==NULL)
		  throw "fitsmerge::printPlane plane is a NULL pointer";
		
		for(unsigned int i=0; i < x*y; i++)		// loop over x dimension (rows)
		{
			cout << plane[i] << "\t";
			if((i % x)==0)						// every row print an endline
				cout << endl;
		}
  }	



  // ============================================================================
  //
  //  Header access functions (keywords etc.)
  //
  // ============================================================================

  // Methods for reading keywords and records from a rmFITS file


  /*!
    \param &keysexist - number of existing keywords (without END)
    \param &morekeys - morekeys=-1 if the header has not yet been closed
  */
  void rmFITS::getHDRspace (int &keysexist,
                             int &morekeys)
  {
    if (fits_get_hdrspace(fptr, &keysexist, &morekeys, &fitsstatus))
      {
        throw "rmFITS::getHDRspace";
      }
  }


  /*!
    \param keynum - nth key to read, first keyword in the header is at keynum=1
    \param record - write the entire 80-character header record into this string
  */
  void rmFITS::readRecord (int keynum,
                            std::string record)
  {
    if (fits_read_record(fptr, keynum, const_cast<char *>(record.c_str()), &fitsstatus))
      {
        throw "rmFITS::readRecord";
      }
  }

  /*!
	 \brief Read a header card

    \param keyname - name of key to read
    \param record - write the entire 80-character header record into this string
  */
  void rmFITS::readCard (std::string keyname,
                          std::string record)
  {
    if (fits_read_card(fptr, const_cast<char *>(keyname.c_str()), const_cast<char *>(record.c_str()), &fitsstatus))
      {
        throw "rmFITS::readCard";
      }
  }


  /*!
    \brief Read the nth header record in the CHU

    \param keynum - nth key to read, first keyword in the header is at keynum=1
    \param keyname - string that will contain keyname
    \param value - string that will contain the key's value
    \param comment - string that will contain the corresponding comment
  */
  void rmFITS::readKeyn (int keynum,
                          std::string keyname,
                          std::string value,
                          std::string comment)
  {
    if (fits_read_keyn(fptr, keynum, const_cast<char *>(keyname.c_str()), const_cast<char *>(value.c_str()), const_cast<char *>(comment.c_str()), &fitsstatus))
      {
        throw "rmFITS::readKeyn";
      }
  }


  /*!
    \brief Return the specified keyword

    \param datatype - data type of the keyword (TSTRING, TLOGICAL==int, TBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TULONG, TLONGLONG, TFLOAT, TDOUBLE, TCOMPLEX, TDBLCOMPLEX
    \param keyname -name of key to look for
    \param value - value of key to look for
    \param comment - comment of key to look for
  */
  void rmFITS::readKey (int datatype,
                         std::string keyname,
                         void *value,
                         std::string comment)
  {
    if (fits_read_key(fptr, datatype, const_cast<char *>(keyname.c_str()), value, const_cast<char *>(comment.c_str()), &fitsstatus))
      {
        throw "rmFITS::readKey";
      }
  }


  /*!
    \brief Find the next key in the header whose name matches one of the strings in inclist

    \param **inclist - array of character strings with names to be included in search
    \param ninc - number of included keys in search
    \param **exclist - array of character strings with names to be excluded in search
    \param nexc - number of excluded keys, may be set to 0 if there are no keys excluded
    \param card - string to write card to
  */
  void rmFITS::findNextKey (char **inclist,
                             int ninc,
                             char **exclist,
                             int nexc,
                             std::string card)
  {
    if (fits_find_nextkey(fptr, inclist, ninc, exclist, nexc, const_cast<char *>(card.c_str()), &fitsstatus))
      {
        throw "rmFITS::findNextKey";
      }
  }


  /*!
    \brief Read the Unit string of a key

    \param keyname - name of key
    \param unit - string to write the physical unit to
  */
  void rmFITS::readKeyUnit (std::string keyname,
                             std::string unit)
  {
    if (fits_read_key_unit(fptr, const_cast<char *>(keyname.c_str()), const_cast<char *>(unit.c_str()), &fitsstatus))
      {
        throw "rmFITS::readKeyUnit";
      }
  }


  //===============================================================
  //
  // Methods for writing keywords and records to a rmFITS file
  //
  //===============================================================

  /*!
    \brief Write a key to the header

    \param datatype - data type of key
    \param keyname - name of key
    \param *value - value to write into field
    \param comment - comment string
  */
  void rmFITS::writeKey (int datatype,
                          std::string keyname,
                          void *value,
                          std::string comment)
  {
    if (fits_write_key(fptr, datatype, const_cast<char
                       *>(keyname.c_str()), value, const_cast<char *>(comment.c_str()), &fitsstatus))
      {
        throw "rmFITS::writeKey";
      }
  }


  /*!
    \brief Update a key in the header

    \param datatype - data type of key
    \param keyname - name of key
    \param value - value to write into field
    \param comment - comment string
  */
  void rmFITS::updateKey (int datatype,
                           std::string &keyname,
                           void *value,
                           std::string &comment)
  {

    if (fits_update_key(	fptr,
                         datatype,
                         const_cast<char *>(keyname.c_str()),
                         value,
                         const_cast<char *>(comment.c_str()),
                         &fitsstatus))
      {
        throw "rmFITS::updateKey";
      }
  }


  /*!
    \brief Write a record "card" to the FITS header of the CHDU

    \param card - string containing card information
  */
  void rmFITS::writeRecord(std::string &card)
  {
    if (fits_write_record(fptr, const_cast<char *>(card.c_str()), &fitsstatus))
      {
        throw "rmFITS:writeRecord";
      }
  }


  /*!
    \brief Modify the comment of &keyname in the FITS header of the CHDU

    \param keyname - name of key to modify key of
    \param comment - new comment string
  */
  void rmFITS::modifyComment (std::string &keyname,
                               std::string &comment)
  {
    if (fits_modify_comment(fptr, const_cast<char *>(keyname.c_str()), const_cast<char *>(comment.c_str()), &fitsstatus))
      {
        throw "rmFITS::modifyComment";
      }
  }


  /*!
    \brief Write a Unit to the key corresponding to &keyname in the CHDU

    \param keyname - name of key to modify unit of
    \param unit - string with physical unit
  */
  void rmFITS::writeKeyUnit (std::string &keyname,
                              std::string &unit)
  {
    if (fits_write_key_unit(fptr, const_cast<char *>(keyname.c_str()), const_cast<char *>(unit.c_str()), &fitsstatus))
      {
        throw "rmFITS::writeKeyUnit";
      }
  }

  //_____________________________________________________________________________
  //                                                                 writeComment

  /*!
    \param &comment - write a comment to the CHU
  */
  void rmFITS::writeComment (std::string &comment)
  {
    if (fits_write_comment(fptr, const_cast<char *>(comment.c_str()),  &fitsstatus))
      {
        throw "rmFITS::writeComment";
      }
  }

  //_____________________________________________________________________________
  //                                                                 writeHistory

  /*!
    \param history - History text
  */
  void rmFITS::writeHistory(std::string &history)
  {
    if (fits_write_history(fptr, const_cast<char *>(history.c_str()),  &fitsstatus))
      {
        throw "rmFITS::writeHistory";
      }
  }

  //_____________________________________________________________________________
  //                                                                    writeDate

  void rmFITS::writeDate()
  {
    if (fits_write_date(fptr,  &fitsstatus))
      {
        throw "rmFITS::writeDate";
      }
  }

  //_____________________________________________________________________________
  //                                                                 deleteRecord

  /*!
    \param keynum - number of key to delete
  */
  void rmFITS::deleteRecord (int keynum)
  {
    if (fits_delete_record(fptr, keynum,  &fitsstatus))
      {
        throw "rmFITS::deleteRecord";
      }
  }

  //_____________________________________________________________________________
  //                                                                    deleteKey

  /*!
    \param keyname - name of key to delete
  */
  void rmFITS::deleteKey (std::string keyname)
  {
    if (fits_delete_key(fptr, const_cast<char *>(keyname.c_str()),  &fitsstatus))
      {
        throw "rmFITS::deleteKey";
      }
  }

  //_____________________________________________________________________________
  //                                                                   copyHeader

  /*!
    \param &other - other rmFITS object to copy header from
  */
  void rmFITS::copyHeader (rmFITS &other)
  {
    if (fits_copy_header(fptr, other.fptr, &fitsstatus))
      {
        throw "rmFITS::copyHeader";
      }
  }

  //_____________________________________________________________________________
  //                                                                    deleteHDU

  /*!
    \param *hdutype - Stores the HDUType of the deleted HDU (can be NULL if not needed)
  */
  void rmFITS::deleteHDU (int *hdutype=NULL)
  {
    if (fits_delete_hdu(fptr, hdutype ,&fitsstatus))
      {
        throw "rmFITS::deleteHDU";
      }
  }

  //_____________________________________________________________________________
  //                                                                writeChecksum

  void rmFITS::writeChecksum()
  {
    if (fits_write_chksum(fptr, &fitsstatus))
      {
        throw "rmFITS::writeChecksum";
      }
  }

  //_____________________________________________________________________________
  //                                                               verifyChecksum

  /*!
    \param dataok - bool indicating that data part of hdu is ok
    \param hduok - bool indicating that hdu is ok
  */
  void rmFITS::verifyChecksum (bool &dataok,
                                bool &hduok)
  {
    int dataok_int=0, hduok_int=0;

    if (fits_verify_chksum(fptr, &dataok_int, &hduok_int, &fitsstatus))
      {
        throw "rmFITS::verifyChecksum";
      }

    // convert int dataok and int hduk to bools
    if (dataok_int)
      dataok=true;
    if (hduok_int)
      hduok=true;
  }

  //_____________________________________________________________________________
  //                                                                   parseValue

  /*!
      \param card - card to be parsed
      \param &value - string to hold content of value
      \param &comment - string to hold content of comment
    */
  void rmFITS::parseValue (std::string &card,
                            std::string &value,
                            std::string &comment)
  {
    if (fits_parse_value(const_cast<char *>(card.c_str()), const_cast<char *>(value.c_str()), const_cast<char *>(comment.c_str()), &fitsstatus))
      {
        throw "rmFITS::parseValue";
      }
  }

  //_____________________________________________________________________________
  //                                                                   getKeytype

  /*!
    Parses the input 80-character header keyword value string to determine its
    datatype. It returns with a value of 'C', 'L', 'I', 'F' or 'X', for character
    string, logical integer, floating point, or complex, respectively.

    \param value - value of key to look for
    \param dtype - data type of that key/value
  */
  char rmFITS::getKeytype (std::string &value)
  {
    char dtype;

    if (fits_get_keytype(const_cast<char *>(value.c_str()), &dtype, &fitsstatus))
      {
        throw "rmFITS::getKeytype";
      }

    return dtype;
  }

  //_____________________________________________________________________________
  //                                                                  getKeyclass

  /*!
    \param card - Card to determine class of
  */
  int rmFITS::getKeyclass (std::string &card)
  {
    int keyclass=0;

    if (fits_get_keyclass(const_cast<char *>(card.c_str())))
      {
        throw "rmFITS::getKeyclass";
      }

    return keyclass;
  }

  //_____________________________________________________________________________
  //                                                                parseTemplate

  /*!
    \param templatec - Template card
    \param card -

    \return keytype - determines if the keyword is a COMMENT or not
  */
  int rmFITS::parseTemplate (std::string &templatec,
                              std::string &card)
  {
    int keytype=0;

    if (fits_parse_template(const_cast<char *>(templatec.c_str()), const_cast<char *>(card.c_str()), &keytype, &fitsstatus))
      {
        throw "rmFITS::parseTemplate";
      }

    return keytype;
  }


  // ============================================================================
  //
  //  Header access functions (specific to RM)
  //
  // ============================================================================

  /*!
    \brief Write header information specific to a RM cube

    \param hdu to write to (default 1)
  */
  void rmFITS::writeRMHeader (int hdu)
  {
    moveAbsoluteHDU(hdu);

    if(getHDUType()!=IMAGE_HDU) // Check if it is an image extension
      throw "rmFITS:writeRMHeader HDU is not an image extension";
    // Copy rmFITS image header

    // Write individual header keywords to FITS

    // faraday low
    // faraday high
    // algorithm used to create RM cube
  }


  /*!
    \brief Check if a file exists

    \param filename - Name of the file to check for its existence
  */
  bool rmFITS::fileExists (const std::string &filename)
  {
    struct stat stFileInfo;
    bool blnReturn;
    int intStat;

    // Attempt to get the file attributes
    intStat = stat(filename.c_str(),&stFileInfo);
    if (intStat == 0)
	 {
        // We were able to get the file attributes
        // so the file obviously exists.
        blnReturn = true;
      }
    else
    {
        // We were not able to get the file attributes.
        // This may mean that we don't have permission to
        // access the folder which contains this file. If you
        // need to do that level of checking, lookup the
        // return values of stat which will give you
        // more details on why stat failed.
        blnReturn = false;
    }

    return(blnReturn);
  }

  // ============================================================================
  //
  //  Class summary functions & Co.
  //
  // ============================================================================

  /*!
      \param os -- Output stream to which the summary is going to be written
  */

  void rmFITS::summary (std::ostream &os)
  {
    os << "[rmFITS] Summary of object properties" << std::endl;
    os << "-- Status of last cfitsio operation = " << fitsstatus << std::endl;
  }

}
