#ifndef _DATAIO_H_
#define _DATAIO_H_

// standard library
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

// external libraries
#include <fitsio.h>

using namespace std;

namespace lopestools {

/*!
  \class DataIO

  \brief I/O of the data generated and read by the lensing reconstruction

  \author Lars B&auml;hren

  \date 2005/12/14

  \section synopsis Synopsis

  Since I/O will be required over and over again, it makes sense to
  implement the required functionality once. Instead of just providing
  a simple set of functions an implementation as class allows to hide a
  number of details behind a few higher level well defined interfaces.

  <table border="0" cellpadding="2">
    <tr bgcolor="dddddd">
      <td>Domain</td>
      <td colspan="8">Columns of data</td>
    </tr>
    <tr>
      <td rowspan="3" valign="top" bgcolor="eeeeee">SKY</td>
      <td>\f$x_{1}\f$</td>
      <td>\f$x_{2}\f$</td>
      <td>\f$I(x_{1},x_{2})\f$</td>
    </tr>
    <tr>
      <td>\f$x_{1}\f$</td>
      <td>\f$x_{2}\f$</td>
      <td>\f$f(x_{1},x_{2})\f$</td>
      <td>\f$n(x_{1},x_{2})\f$</td>
    </tr>
    <tr>
      <td>\f$x_{1}\f$</td>
      <td>\f$x_{2}\f$</td>
      <td>\f$I(x_{1},x_{2})\f$</td>
      <td>\f$f(x_{1},x_{2})\f$</td>
      <td>\f$n(x_{1},x_{2})\f$</td>
    </tr>
    <tr>
      <td rowspan="3" valign="top" bgcolor="eeeeee">UV</td>
      <td>\f$k_{1}\f$</td>
      <td>\f$k_{2}\f$</td>
      <td>\f${\rm Re} \left\{ \tilde I(k_{1},k_{2}) \right\}\f$</td>
      <td>\f${\rm Im} \left\{ \tilde I(k_{1},k_{2}) \right\}\f$</td>
    </tr>
    <tr>
      <td>\f$k_{1}\f$</td>
      <td>\f$k_{2}\f$</td>
      <td>\f${\rm Re} \left\{ \tilde f(k_{1},k_{2}) \right\}\f$</td>
      <td>\f${\rm Im} \left\{ \tilde f(k_{1},k_{2}) \right\}\f$</td>
      <td>\f${\rm Re} \left\{ n(k_{1},k_{2}) \right\}\f$</td>
      <td>\f${\rm Im} \left\{ n(k_{1},k_{2}) \right\}\f$</td>
    </tr>
    <tr>
      <td>\f$k_{1}\f$</td>
      <td>\f$k_{2}\f$</td>
      <td>\f${\rm Re} \left\{ \tilde I(k_{1},k_{2}) \right\}\f$</td>
      <td>\f${\rm Im} \left\{ \tilde I(k_{1},k_{2}) \right\}\f$</td>
      <td>\f${\rm Re} \left\{ \tilde f(k_{1},k_{2}) \right\}\f$</td>
      <td>\f${\rm Im} \left\{ \tilde f(k_{1},k_{2}) \right\}\f$</td>
      <td>\f${\rm Re} \left\{ n(k_{1},k_{2}) \right\}\f$</td>
      <td>\f${\rm Im} \left\{ n(k_{1},k_{2}) \right\}\f$</td>
    </tr>
  </table>
*/
template <typename T> class DataIO {

  //! Name of the input data file
  string infile_p;
  //! Shape of the data array (including the coordinate axes)
  vector<int> shape_p;

 public:
  
  /*!
    \brief Default constructor
  */
  DataIO ();

  /*!
    \brief Argumented constructor

    \param infile -- Name/Path of the input data file
  */
  DataIO (const string& infile);

  /*!
    \brief Destructor
  */
  ~DataIO ();

  // --- Parameters -------------------------------------------------------------

  string infile () {
    return infile_p;
  }

  vector<int> shape () {
    return shape_p;
  }

  // --- Input methods ----------------------------------------------------------

  /*!
    \brief Read in data from ASCII table

    \param infile -- Name/Path of the input data file
  */
  vector<T> fromASCII (const string& infile);

  // --- Output methods ---------------------------------------------------------

  /*!
    \brief Export the data to a Root script

    \verbatim
    TGraph2D *g = new TGraph2D(n,x,y,z);
    \endverbatim
    A 2D graph also can be constructed directly form an ASCII table; here is a
    simple example:
    \verbatim
    {
      TCanvas *c = new TCanvas("c","Graph2D example",0,0,700,600);
      TGraph2D *g = new TGraph2D("graph.data");
      g->Draw("surf1");
    }
    \endverbatim

    \param filename -- Name of the output file.
  */
  void toRootScript (const string& filename);
  
 private:
  
  /*!
    \brief Read in data from ASCII table
    
    This of course only works, if the object has been created by providing the
    name of the file.
  */
  vector<T> fromASCII ();
  
};

}

#endif
