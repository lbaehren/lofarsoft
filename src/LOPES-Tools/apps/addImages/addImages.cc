
/* $Id: addImages.cc,v 1.1 2005/06/23 19:14:11 bahren Exp $ */

#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Quanta.h>
#include <images/Images/PagedImage.h>

#include <casa/namespace.h>

/*!
 \file addImages.cc
 
 \author Lars B&auml;hren
 
 \date 2005/03
 
 <h3>Prerequisites</h3>
 
 <ul type="square">
   <li>[CASA] PagedImage -- Read, store, and manipulate astronomical images.
 </ul>

*/

/*!
 \brief Update the array holding the normalization information.
 
 \param arrayNorm - Integer valued array holding the normalization information; 
                    the array will be updated within the routine.
 \param myimage   - Reference to the paged image, which will be added to the
                    master image.
*/
void updateNormalization (Array<Int>& arrayNorm,
			  PagedImage<Double>& myimage)
{  
  Bool hasPixelMask (False);

  hasPixelMask = myimage.hasPixelMask();
  
  if (!hasPixelMask) {
    cerr << "[addImages] Image object does not have a pixel mask!" << endl;
  }
  
}

// =============================================================================

int main () {

  return 0;
}
