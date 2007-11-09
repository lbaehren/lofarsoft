/*-------------------------------------------------------------------------*
| $Id: template-class.h,v 1.20 2007/06/13 09:41:37 bahren Exp           $ |
*-------------------------------------------------------------------------*
***************************************************************************
*   Copyright (C) 2007                                                    *
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

#ifndef DATAITERATOR_H
#define DATAITERATOR_H

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class DataIterator
    
    \ingroup IO

    \brief A file stream position interator for file I/O
    
    \author Lars B&auml;hren
    
    \date 2005/11/08
    
    \test tDataIterator.cc
    
    <h3>Prerequisite</h3>
    
    <h3>Synopsis</h3>
    
    The following equation is used to relate the blocknumber to the absolute
    position within the filestream:
    
    \code
    position_p = dataStart_p + shift_p + (block_p - 1)*(blocksize_p+stride_p)*sizeof(dataType_p);
    \endcode
    
    \f[ n_{\rm Position} = N_{\rm Start} + N_{\rm Shift} + \Bigl( n_{\rm Block} - 1
    \Bigr) \cdot \Bigl( N_{\rm Blocksize} + N_{Stride} \Bigr) \f]
    
    where \f$ N_{\rm Start} \f$ is the absolute position within the filestream
    where the data segment starts (remember that header information might be
    stored within the file as well). \f$ N_{\rm Blocksize} \f$ is the number of
    samples per data block, \f$ N_{Stride} \f$ is the stride between two subsequent 
    blocks of data.
    \f$ N_{\rm Shift} \f$ is a shift w.r.t. \f$ N_{\rm Start} \f$, enabling to start
    reading data offset to the start of the data segment. \f$ n_{\rm Block} \f$ is
    the number of the currently selected data block (note that counting is
    1-based, as we rather speak of the first instead of the 0-th data block).
    
    The above described access scheme is illustrated in the figure below (note
    that \f$ N_{Stride} > 0 \f$):
    
    \image html DataIterator.png
  */
  
  class DataIterator {
    
    //! Number of samples per block of data
    unsigned int blocksize_p;
    //! Number of channels in the FFT of a block of data
    unsigned int fftLength_p;
    //! Start of the data segment inside the data file/volume
    unsigned int dataStart_p;
    //! Offset w.r.t. to the start of the data segment
    unsigned int shift_p;
    //! Stride between two subsequent blocks of data
    long int stride_p;
    // Number of the data block
    unsigned int block_p;
    //! Stepwidth for navigation through the data (samples)
    unsigned int stepWidth_p;
    //! Position (in the data stream)
    unsigned int position_p;
    
  public:
    
    // --- Construction ---------------------------------------------------------
    
    /*!
      \brief Default constructor
      
      This will set \f$ N_{\rm Start} \equiv N_{Stride} \equiv N_{\rm Shift}
      \equiv 0 \f$ and \f$ N_{\rm Blocksize} \equiv n_{\rm Block} \equiv 1 \f$,
      thus \f$ n_{\rm Position} = 0 \f$.
    */
    DataIterator ();
    
    /*!
      \brief Argumented constructor
      
      \param blocksize -- Size of a datablock (in units of <tt>T</tt>).
    */
    DataIterator (unsigned int const &blocksize);
    
    /*!
      \brief Argumented constructor
      
      \param blocksize -- Size of a datablock, \f$ N_{\rm Blocksize} \f$, in units
                          of <tt>T</tt>
      \param dataStart -- Start of the data segment, \f$ N_{\rm Start} \f$.
    */
    DataIterator (unsigned int const &blocksize,
		  unsigned int const &dataStart);
    
    /*!
      \brief Argumented constructor
      
      \param blocksize -- Size of a datablock, \f$ N_{\rm Blocksize} \f$, in units
                          of <tt>T</tt>
      \param stride    -- Stride between two subsequent blocks of data,
                          \f$ N_{Stride} \f$, in units of <tt>T</tt>
      \param shift     -- \f$ N_{\rm Shift} \f$, a shift w.r.t. \f$ N_{\rm Start} \f$
    */
    DataIterator (unsigned int const &blocksize,
		  long int const &stride,
		  unsigned int const &shift);
    
    /*!
      \brief Argumented constructor
      
      \param blocksize -- Size of a datablock, \f$ N_{\rm Blocksize} \f$, in units
                          of <tt>T</tt>
      \param dataStart -- Start of the data segment, \f$ N_{\rm Start} \f$.
      \param stride    -- Stride between two subsequent blocks of data,
                          \f$ N_{Stride} \f$, in units of <tt>T</tt>
      \param shift     -- \f$ N_{\rm Shift} \f$, a shift w.r.t. \f$ N_{\rm Start} \f$
    */
    DataIterator (unsigned int const &blocksize,
		  unsigned int const &dataStart,
		  long int const &stride,
		  unsigned int const &shift);
    
    /*!
      \brief Copy constructor
    */
    DataIterator (DataIterator const &other);
    
    // --- Destruction ----------------------------------------------------------
    
    /*!
      \brief Destructor
    */
    ~DataIterator ();
    
    // --- Operators ------------------------------------------------------------
    
    /*!
      \brief Copy operator
    */
    DataIterator &operator= (DataIterator const &other);
    
    // --- Parameters -----------------------------------------------------------
    
    /*!
      \brief Get the step width
      
      \return stepWidth - The step width
    */
    unsigned int stepWidth () {
      return stepWidth_p;
    }
    
    /*!
      \brief Set the new step width from the size of a variable type
      
      \param var -- Variable of type T
    */
    template <class T> void setStepWidth (T const &var);
    
    /*!
      \brief Set the new step width to one 
      
      While the original conception for the DataIterator class was to allow the 
      navigation within a datastream or a datafile stored on this. the offered
      navigation method in principle also can be used for addressing the contents
      of an array stored in memory. However in such a case the conversion between
      samples and bytes no longer is required -- therefore <tt>stepwidth</tt>
      is set to one here.
    */
    void setStepWidthToSample ();
    
    /*!
      \brief Get the current position within the filestream.
      
      \return position - The current position within the filestream.
    */
    unsigned int position () const {
      return position_p;
    }
    
    /*!
      \brief Get the start position of the data segment within the file
      
      \return dataStart -- Start of the data segment inside the data file/volume
    */
    unsigned int dataStart () const {
      return dataStart_p;
    }
    
    /*!
      \brief Set the start position of the data segment
      
      \param dataStart -- Start of the data segment inside the data file/volume
    */
    void setDataStart (unsigned int const &dataStart);
    
    /*!
      \brief Get the size of a datablock
      
      \return blocksize - Size of a datablock (in units of <tt>T</tt>).
    */
    unsigned int blocksize () const {
      return blocksize_p;
    }
    
    /*!
      \brief Set the size of a datablock
      
      \param blocksize - Size of a datablock (in units of <tt>T</tt>).
    */
    void setBlocksize (unsigned int const &blocksize);
    
    /*!
      \brief Get the output length of the FFT
      
      \return fftlength -- Output length of the FFT performed on a block of data
    */
    unsigned int fftLength () const {
      return fftLength_p;
    }
    
    /*!
      \brief Get the stride between two subsequent data blocks.
      
      \return stride -- Stride between two subsequent blocks of data,
                        \f$ N_{Stride} \f$, in units of <tt>T</tt>
    */
    long int stride () {
      return stride_p;
    }
    
    /*!
      \brief Set the stride between two subsequent data blocks.
      
      <ul type="square">
        <li><tt>stride < 0</tt> : overlapping data blocks
	<li><tt>stride = 0</tt> : continuous data blocks
	<li><tt>stride > 0</tt> : non-continuous data block
      </ul>
      
      \param stride -- Stride between two subsequent blocks of data,
                       \f$ N_{Stride} \f$, in units of <tt>T</tt>
    */
    void setStride (long int const &stride);
    
    /*!
      \brief Get the shift between the start of the data segment and the first block.
      
      \return shift 
    */
    unsigned int shift () {
      return shift_p;
    }
    
    /*!
      \brief Set the shift between the start of the data segment and the first block.
      
      \param shift - Shift w.r.t. the start of the data segment (\f$ N_{\rm Start} \f$).
    */
    void setShift (unsigned int const &shift);
    
    /*!
      \brief Set the shift between the start of the data segment and the first block.
      
      The input will be forwarded to DataIterator::setShift.
      
      \param shift - Shift in units of data samples.
    */
    void setShiftInSamples (unsigned int const &shift);
    
    /*!
      \brief Set the shift between the start of the data segment and the first block.
      
      The input will be forwarded to DataIterator::setShiftInSamples.
      
      \param shift - Shift in units of blocks.
    */
    void setShiftInBlocks (unsigned int const &shift);
    
    /*!
      \brief Get the current block number 
      
      \return block - The current block number.
    */
    unsigned int block () {
      return block_p;
    }
    
    /*!
      \brief Got to a specific block.
      
      \param block - Number of the block to go to.
    */
    void setBlock (unsigned int const &block);
    
    /*!
      \brief Advance the blocknumber by one.
    */
    void nextBlock ();
    
    // --- Methods --------------------------------------------------------------
    
    /*!
      \brief Compute the maximum number of blocks that can be read
      
      \param filesize -- The size of the data file.
      
      \return maxBlocks -- The maximum number of block that can be read from the
                           data file.
      
      The function checks for validity of the condition
      \f[ n_{\rm Position} (n_{\rm Block}=N_{\rm Blocks}) \leq N_{\rm Filesize} \f]
      where \f$ N_{\rm Filesize} \f$ is the total size of the data file and
      \f$ N_{\rm Blocks} \f$ is the maximum number of blocks that can be read from
      data file without exceeding the end of the data file. Full expansion of the 
      above expression yields
      \f[ \frac{N_{\rm Filesize} - N_{\rm Start} - N_{\rm Shift}}{N_{\rm Blocksize}
      + N_{\rm Stride}} + 1 \leq N_{\rm Blocks} \f]
    */
    unsigned int maxNofBlocks (unsigned int const &filesize);
    
    /*!
      \brief Provide a summary of the internal status
    */
    void summary ();
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (DataIterator const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
    /*!
      \brief Initialize internal buffers.
    */
    void init ();
    
    /*!
      \brief Set the current position within the filestream.
    */
    void setPosition ();
    
  };
  
}  // Namespace CR -- end

#endif /* _DATAITERATOR_H_ */
  
