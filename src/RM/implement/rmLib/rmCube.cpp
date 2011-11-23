/*! Implementation of rmCube class methods

    Author:		Sven Duscha (sduscha@mpa-garching.mpg.de)
    Date:		15-05-2009
    Last change:	27-05-2009
*/


#include <iostream>				// C++/STL iostream
#include <math.h>				// mathematics library
#include <string.h>
#include "rmCube.h"				// rmCube class declarations
// #include <fitshandle.h>
#define pi 3.1415926535897932385
using namespace std;
using namespace casa ;
namespace RM {
  
  //===============================================================================
  //
  //  Constructions / Destruction
  //
  //===============================================================================
  
  /*!
    \brief Default destructor
  */
  rmCube::~rmCube()
  {
//     cout << "Call destructor" << buffer << endl ;
//     if (buffer != 0)
//       delete[] this->buffer;	// delete memory of buffer which was created with new!!! 
//     cout << "destructor finished" << endl ;
  }
  
  /*! function to calculate a set of farradaydepths out 
  * of the set of lambda squares. The number of faradaydepths 
  * is the same as the number of lambda^2. The searched values for 
  * the faraday depths are choosen to get a nearly unitary transfomation
  * from lampda^2 to faradaydepths.
  * \param  lambda vector  of the lambda^2 values */
 
vector<double> rmCube::lambdaToFarraday(vector<double> &lambda ) {
  int N = lambda.size(); 
  /* reduce the lambdas so that lambda[0] = 0 */
  for (int i=1; i<N; i++) {
    lambda[i] = lambda[i]-lambda[0] ;
  }
  lambda[0] = 0 ;
  vector<double> out(N);  // resultvector 
  for (int i=0; i<N ; i++) {
    if (lambda[i] == 0)
        out[i] = 0;
    else
       out[i] = 2*pi*i*i/(N*lambda[i]) ;
    
  }
  return out ;
}

  /*! function to calculate a set of farradaydepths out 
  * of the set of lambda squares. The number of faradaydepths 
  * is the same as the number of lambda^2. The searched values for 
  * the faraday depths are choosen to get a nearly unitary transfomation
  * from lampda^2 to faradaydepths.
  * Procedure orientates on Phd theses of Michiel Brentjens page 43
  * \param  lambda vector  of the lambda^2 values */
 
vector<double> rmCube::lambdaToFarradayBrentjens(vector<double> &lambda ) {
  int N = lambda.size(); 
/* calculate the total difference of the lambdas */
  double DeltaLambda = (lambda[N-1]-lambda[0]) ;
  /* calculate the mean delta lambda square */
//  double deltaLambda = DeltaLambda/N ;
  /* calculate the resolution in faradaydepths */
  double deltaPhi = 2*sqrt(3.0)/DeltaLambda ;
  vector<double> out(N);  // resultvector 
  for (int i=0; i<N ; i++) {
    out[i] = i*deltaPhi ;
  }
  return out ;
}

  /*! function to calculate a set of farradaydepths out 
  * of the set of lambda squares. The number of faradaydepths 
  * is the same as the number of lambda^2. The searched values for 
  * the faraday depths are choosen to get a nearly unitary transfomation
  * from lampda^2 to faradaydepths.
  * Procedure orientates on Phd theses of Michiel Brentjens page 43
  * \param  lambda vector  of the lambda^2 values */
 
vector<double> rmCube::lambdaToFarradayBrentjensSym(vector<double> &lambda ) {
  int N = lambda.size(); 
/* calculate the total difference of the lambdas */
  double DeltaLambda = (lambda[N-1]-lambda[0]) ;
  /* calculate the mean delta lambda square */
  double deltaLambda = DeltaLambda/N ;
  /* calclulate the whole length of the faraday depths interval */
  double DeltaPhi = 2*sqrt(3)/deltaLambda ;
  /* calculate inter interval lenght for the faraday axis */
  double deltaPhi = DeltaPhi/(N-1) ;
  vector<double> out(N);  // resultvector 
  for (int i=0; i<N ; i++) {
    out[i] = i*deltaPhi-0.5*DeltaPhi ;
  }
  return out ;
}

  /*! function to calculate a set of farradaydepths out 
  * of the set of lambda squares. The number of faradaydepths 
  * is the same as the number of lambda^2. The searched values for 
  * the faraday depths are choosen to get a nearly unitary transfomation
  * from lampda^2 to faradaydepths.
  * \param  lambda vector  of the lambda^2 values */
 
vector<double> rmCube::freqsToFarraday(vector<double> freqs, double cq ) {
  int N = freqs.size(); 
  /* calculate lambda squares */
  for (int i=0; i<N; i++) {
    freqs[i] = cq/(freqs[i]*freqs[i]) ;
  }
  /* reverse direction */
  for(int i=0; 2*i<N; i++) {
    swap(freqs[i],freqs[N-i-1]) ;
  }
  /* reduce the lamdas so that lambda[0] = 0 */
 
  return lambdaToFarraday(freqs);
}

/*!
  \brief Convert from frequencies to lambda squared
  \param frequency - frequency vector 
  \return lambda_sq - converted lambda squared vector
*/
vector<double> rmCube::freqToLambdaSq(const vector<double> &frequency)
{
  // lambda squared to be calculated, same size as frequency vector
  vector<double> lambda_sq(frequency.size());	
  // constants
  const double csq=89875517873681764.0;	// c^2  
  // loop over frequency vector and compute lambda squared values
  for(unsigned int i=0; i<frequency.size(); i++)
  {
     lambda_sq[frequency.size()-i-1]=csq/(frequency[i]*frequency[i]);
  }
  
  return lambda_sq;	// return computed lambda squareds
}

/*! Procedure performs a simple RM-Synthesis, which just makes an diskrete 
  * "innverse Fourier" transformation. The matrix for this transformation is \
  * intended to be maximal similar to an unitary matrix. 
  * \param	QU		: QU values of lambdadependece which must be transformed
  * \param	lambdas		: lambda^2-lambda0^2 of the QU values
  * \param	faradays	: faraday depths for which the new QU values are calculated */
vector<complex<double> > rmCube::performRMSynthesis(vector<complex<double> > &QU, vector<double> &lambdas, vector<double> &faradays, vector<uint> &gaps, double nu_0, double alpha, double epsilon) {
  uint N = faradays.size() ;  
  vector<complex<double> > res(N) ;	
  /* loop over all faraday depths, which is the dimension of the result.
     in every periode of the loop one component of the result is written */
complex<double> zero(0,0) ;
  for (uint i=0; i<N ; i++ ) {
    complex<double> erg = integrateLamdaSq(QU,lambdas,faradays[i],gaps,nu_0,alpha, epsilon) ;
    res[i] = erg ;
  }  
  return res ;
}

/*! integration over the lamda squared of the instrument. In the case of gaps the lamda 
    squared intervals are splited into two separat intervals */

complex<double> rmCube::integrateLamdaSq(vector<complex<double> > &QU,vector<double> &lambdas, double faraday, vector<uint> gaps, double nu_0, double alpha, double epsilon) {
    double lam_a ;  // start of the integration interval for phi integration 
    double lam_b ;  // end of the integration interval for phi integration 
//     uint Nlambdas = lambdas.size() ;
    complex<double> exp_lambdafactor;
    complex<double> erg = 0.0 ;
    /* loop over all seperate intervals of faradydepths, which are provided by the 
       vector gaps, where for each interval the first index is stored (plus the 
       first index of an non existing last interval */
    for (uint i=0; i<gaps.size()-1;i++) { // intervals form gaps[i] to gaps[i+1]-1 last index interval end
   // calculate the interval in faradaydepths
    /* loop for the porduct of one row of the transformation matrix with the vector
     * which must be transformed */
      uint anf = gaps[i] ;
      uint ende = gaps[i+1];
      for	(uint j=anf; j<ende; j++) { 
        /* get the boarders of the current lamda interval */
        if (j==anf)  lam_a = lambdas[anf] ;
        else lam_a = 0.5*(lambdas[j-1]+lambdas[j]) ;
        if (j==ende-1) lam_b = lambdas[ende-1] ;
        else lam_b= 0.5*(lambdas[j]+lambdas[j+1]) ;
        double arg = -2.0*faraday*lambdas[j] ;
        exp_lambdafactor=complex<double>(cos(arg), sin(arg));	
        erg = erg+exp_lambdafactor*QU[j]*(lam_b-lam_a) ;
      }
    }
    return erg ;
}
  /*!
    \brief Empty constructor
  */
  rmCube::rmCube():rmIO()
  {
    // Initialize all buffers with NULL
    buffer=NULL;
    
    //   cout << "empty constructor" << endl;
  }

  /*!
    \brief rmCube constructor, creates a RM cube with given dimensions
    
    \param x - Horizontal dimension in pixels
    \param y - Vertical dimension in pixels
    \param faradaySize - total extension in Faraday depth
    \param stepsize - Step size between Faraday depths
    \param axis 1= freqs, 2= lambda squared 3= faraday depth
  */
  rmCube::rmCube(int faradaySize, int x, int y, int axis, int pix)
  {
//     int steps=0;	// number of steps=faradaySize/stepsize
//     int i=0;	// loop variable
    this->axis = axis ;
    this->xSize=x;
    this->ySize=y;
    this->faradaySize=faradaySize;
    allocMemory(true) ;
    this->currentX=0;
    this->currentY=0;
    this->currentFaradayDepth=0;
    this->ra=0;
    this->dec=0;
    this->ra_low=0;
    this->ra_high=0;
    this->dec_low=0;
    this->dec_high=0;
    this->rmType= axis ;
    this->pix=pix;
    //   cout << "rmCube::rmCube(int x, int y, int faradaySize, double stepsize) constructor" << endl;
  }

/*! procedure removes those names out of the list of file names which are 
    not the names of fits files 
    \param names vector of file names */

void rmCube::removeNoFits(vector<string> &names) {
  vector<string>::iterator it ;
  int i = 0 ;
  for ( it=names.begin() ; it<names.end(); it++ ) {
    i++ ;
    int indx = it->find(".fits") ;
    if (indx < 0) {
       names.erase(it) ;
       it-- ;
    }
  }
}

void rmCube::getFitsSize(string &path, vector<string> &names, uint &x, uint &y, int &pix) {
  uint pos = 0 ;
  int indx = -1 ;

  /* search for first fits file name */
  while ((indx<0) && (pos < names.size())) {
    pos++ ;
    indx = names[pos].find(".fits") ;
  }
  if (indx >=0) {
    string name = names[pos];
    string dat = path+"/"+name ;
    cout << pos << " " << indx << " " << name << " " << dat << endl ;
//     fitshandle fits(dat.c_str()) ;
//     fits.goto_hdu (2) ;
//     arr2<double> test ;
//     fits.read_image(test) ;
//     x=test.size1() ;
//     y=test.size2() ;
//     pix = fits.bitpix() ;
  }
}

/*! procedure writes the rmcube into a set of fitsfiles into two different directories.
    The two directories are needed for the q and the u part of the polarised signal.
    The cube is written in image planes of either different frequencies or different 
    faraday depths.
    \param qpath path where the q component images are stored 
    \param upath path where the u component images are stored */

void rmCube::writeRM_Cube(string &qpath, string &upath, string &fileNames) {
//   uint x=xSize ;
//   uint y=ySize ;
  /* loop over all frequencies or faraday depths to write each of them into two images */
  for (int i=0; i<getFaradaySize(); i++) {
      char buffer [50];
      sprintf(buffer,"%s_%d.fits", fileNames.c_str(),i) ;
      /* create basic name for files */
      string name(buffer) ;
      /* create names for q and u images including directory name */
      string qall = qpath+"/"+name ;
      string uall = upath+"/"+name ;
//       arr2<double> qimag(y,x) ;
//       arr2<double> uimag(y,x) ;
      /* loop over the size of the images to write the fitsimages on file */
//       for (uint k=0; k<y;k++) {
//         for (uint l=0; l<x; l++) {
// 	   qimag[k][l] = getVal(i,l,k).real() ;
//            uimag[k][l] = getVal(i,l,k).imag() ;
//         }
//       }
      /* create fits files for writing the new images */
//       fitshandle qfits ;
//       fitshandle ufits ;
//       qfits.create(qall);
//       ufits.create(uall);
//       qfits.insert_image(pix, qimag) ;
//       ufits.insert_image(pix, uimag) ;
  }
}

  /*! constructor from two directories of fits files, which 
      \param qpath path to the fits files for the q-part of the polarized signal 
      \param upath path to the fits files for the u-part of the polarized signal 
      \param axis type of the first axis 1= freqs, 2= lambda squared 3= faraday depth 
      \param f_min minimal value for the first axis, the physical meaning depends on parameter axis
      \param f_max maximal value for the first axis, the physical meaning depends on parameter axis  
*/
    rmCube::rmCube(string qpath, string upath, int axis, double f_min, double f_max) {
    /* define vectors for file names */
    vector<string> u_files;
    vector<string> q_files;
    /* read directories where to find the file names */
    readDir(upath,u_files);
    readDir(qpath,q_files);
    /* remove all names wich do not belong to fits files, at least "." and ".." */
    removeNoFits(u_files) ;
    removeNoFits(q_files) ;
    /* deremine the number of fitsfiles for the first dimension of the cube */
    uint f = q_files.size() ;
    /* get the dimension for a representive of the images to determine the
       two other dimensions of the cube */
    uint x,y; 
    int pix ; 
    getFitsSize(qpath, q_files, x, y, pix) ;  // get the size of the fits images
    /* set the dimensions of the intern variables of the cube */ 
    this->axis = axis ;
    this->xSize=x;
    this->ySize=y;
    this->faradaySize=f;
    this->pix = pix ;
    allocMemory(true) ;
    fillRM_Cube(qpath, upath, q_files, u_files, f, x, y, f_min, f_max);
    this->rmType= axis ;
  }

  /*! procedure finds the lowest coordinate system index for the coordinate,
      with a given type.
      \param coors coordinate system to search for the coordinate object
      \param typ type of the coordinate to be searched 
      \param considerTyp indicates wether only each coordinate has to be taken into account of 
            each coordinate is weighted by its number of world axes
      \return index of the first coordinate axis, which has the given typ, if no is found -1 is returned */

  int findCoorIndex(CoordinateSystem coors, Coordinate::Type typ, bool considerCoorTyp) {
    bool found = false ;
    int num = 0 ;
    int erg = -1 ;
    for(uint j=0; j<coors.nCoordinates( ) ; j++) {
      Coordinate const &coor = coors.coordinate(j) ;
      if (coor.type() == typ) {
        erg=num ;
      }
      if (considerCoorTyp) {
        num+=coor.nWorldAxes() ;
      }
      else {
        num++ ;
      }
    }
    return erg ;
  }

  /*! find the index of the subrecord object of a record, which has a given name.
      \param rekord rekord to search the sub records inside
      \param name name of the record to be searched
      \param countAll if count all is false,the procedure only counts subrecords for deremine 
                    the index of the sub record, otherwise it count all elemets of the record 
      \return index of the found record, if no one is found, the procedure returns -1 */

  int findNamedRecord(TableRecord &rekord, string &name, bool countAll) {
    int res = -1 ;
    int counter=0;
    RecordDesc descr = rekord.description() ;
    for (uint i=0; (i<rekord.nfields()) &&(res == -1); i++) {
      if (rekord.type(i)==TpRecord) {
        if (descr.name(i)==name) {
          if (countAll)
            res = i ;
          else 
            res = counter ;
        }
        counter++ ;
      }
    }
    return res ;
  }

  /*! procedure identifies the different directions of the coordinate system.
      It returns a vector which covers the different directions.
      0,1 indexes of the direction coordinates
      2   index of the spektral axis
      3   index of the stokes axis */
  Vector<int> indexes(CoordinateSystem coors ) {
    Vector<int> res(4) ;
    int directIndex = findCoorIndex(coors,Coordinate::DIRECTION, true) ;
    int specIndex = findCoorIndex(coors,Coordinate::SPECTRAL,true) ;
    int stokIndex = findCoorIndex(coors,Coordinate::STOKES,true) ;
    res[0]= directIndex ;
    res[1]= directIndex+1;
    res[2]= specIndex ;
    res[3]= stokIndex ;
    return res ;
  }

void rmCube::copyMetaData(PagedImage<Float> &tarImage) {
  Table &tabTar = tarImage.table() ;
  TableRecord &recTar = tabTar.rwKeywordSet() ;
  for (uint i=0; i<recSrc.nfields(); i++) {
    cout << "i= " << i << " typ = " << recSrc.name(i) << "  " << recTar.fieldNumber(recSrc.name(i))<< endl ;
    /* add additional meta data to the target image */
    if (recTar.fieldNumber(recSrc.name(i))) {
      recTar.mergeField(recSrc,i,RecordInterface::OverwriteDuplicates) ;
    }
    if(recSrc.type(i)== TpString) {
      cout << "data type String found " << i << endl ;
    }
    else if (recSrc.type(i)== TpTable) {
      if ((i<recTar.nfields()) && (recTar.type(i)==TpTable) ) {
        cout << "common datatype table at position " << i << endl ;        
      }
    }
    else if (recSrc.type(i)== TpRecord) {
      cout << "data type Record found " << i << endl ;
    }
    else {
      cout << "unexpected data type" << endl ;
    }
  }
}

  /*! procdure reads all entries of a directory, which are not "." and ".."
      \param dirName name of the directory to be listed
      \return vector of strings, which represent the entries of the directory */

  vector<string> readDirectory(string dirName) {
    vector<string> erg ;
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dirName.c_str())) == NULL) {
      cerr << "readDirectory: Error, can not open open " << dirName << endl;
      throw "Directory not found" ;
    }

    while ((dirp = readdir(dp)) != NULL) {
      string name(dirp->d_name) ;
      if ((name != ".") && (name != "..")) {
        erg.push_back(name);
      }
    }
    closedir(dp);
    sort(erg.begin(),erg.end());
    return erg ;
  }
  /*! constructor reads a casa measuerement. The data are images for multiple
      frequncies. The images are grouped into many subband, which are stored
      inside a own data directory, usually called name.image.
      \param imagePath path of the drectory, where the image is written to 
      \param name of the imagedirectory
      \param query string for searchin the data inside the table
  */
  rmCube::rmCube(string imagePath, string imageName, string query, Vector<int> &inds ) {
      /* Definition einiger konstanten */
  /* Definition einiger konstanten */
    string coords("coords") ;
    string logtab("logtable") ;
    string imaginf("imageinfo") ;
    string units("units") ;
    string spektral("spectral2") ;
    axis=1 ; // set axis flag to frequency axis 
    /* Ende der Definition einiger Konstanten */
    vector<string> files = readDirectory(imagePath) ;
    double last=0;
    uint nFreqs=0 ;
    freqSize=0 ;
    double freqDiff ;
//     Vector<int> inds ;
    /* read main metadata of the current data set to be able to 
       allocate the needed memory */
    for (uint i=0; i<files.size(); i++) {
      if ((files[i] != ".") && (files[i] != "..")) {
        string fileName=imagePath+files[i] ; // ("/scratch/thriller/lofar/andreas/L23648_SB002_uv.MS.dppp.dppp.dppp.img.image") ;
        PagedImage<Float> imag1(fileName) ;
        Table tab1=imag1.table() ;
        /* store keywords */
        if (i==0) {
          recSrc = tab1.keywordSet() ;
        }
        CoordinateSystem coors = imag1.coordinates() ;
        /* just now, store only one coordinates system, later may store a big one */
        if (i==0) {
          cs = coors ;
          log = imag1.logger() ;
          shape = imag1.shape() ;
        }
        Vector<double> pixel(4,0) ;
        Vector<double> world(4,0) ;
        inds =  indexes(coors) ;
        ROArrayColumn<Float> arr2Col (tab1, query);
        IPosition iPos = arr2Col.shape(0) ;
        if (i==0) {
          xSize=iPos[inds[0]];
          ySize=iPos[inds[1]];
        }
        int specIndex=inds[2] ;
        freqSize+=iPos[specIndex] ;
        /* just to avoid segmentation faults */
        if (specIndex != -1) {
          for (uint j=0; j<iPos[specIndex]; j++) {
            pixel[specIndex]=j;
            coors.toWorld(world,pixel) ;  
            freqs.push_back(world[specIndex]) ;
          }
        }
      }
      else {
//         cout << "nicht verwendet bei index: " << i << endl ;
      }
    }
    /* loop to find the frequencies differnce as the smallest difference of all frequencies */
    for (uint i=1; i<freqs.size(); i++) {
      if (i==1) freqDiff = freqs[1]-freqs[0] ;
      else freqDiff = min(freqDiff,freqs[i]-freqs[i-1]) ;
    }
    /* fill the frequency interval vector with this value */
    freqsInter.resize(freqs.size()) ;
    for (uint i=0; i<freqsInter.size(); i++) {
      freqsInter[i]=freqDiff ;
    }
//     cout << "TEST: " << freqs.size()  << " " << freqSize << " " << inds<< endl ;
    allocMemory(false) ;
    uint curIndx = 0 ;
    /* loop for reading the actual data into the rmcube */
    for (uint i=0; i<files.size(); i++) {
      if ((files[i] != ".") && (files[i] != "..")) {
        string fileName=imagePath+files[i] ; // ("/scratch/thriller/lofar/andreas/L23648_SB002_uv.MS.dppp.dppp.dppp.img.image") ;
        PagedImage<Float> imag1(fileName) ;
        Table &tab1=imag1.table() ;
        ROArrayColumn<Float> arr2Col (tab1, query);
        IPosition iPos = arr2Col.shape(0) ;
        Array<Float> feld = arr2Col.getColumn( );
        IPosition pos(iPos.size()+1) ;
        for (uint j=0; j<pos.size(); j++) {
          pos[j] =0 ;
        }
        IPosition ipos = feld.shape() ;
    //   loop over the first component of the Array
        int index = inds[2];
        /* loop over the subbands of the current image */
        for (uint j=0; j<ipos[index] ; j++) {
          pos[index] = j ;  // set the frequency coordinate inside the current image
          /* loop over the first dimension of the current image */
          for (uint x=0; x<xSize; x++) {
            pos[inds[0]]=x ;  // set the position for the first spartial image dimension
            /* loop over the seccond dimension of the current image */
            for (uint y=0; y<ySize; y++) {
              pos[inds[1]]=y ; // set the position for the seccond spartial image dimension
              pos[inds[3]]=1 ; // select pos component for Stokes Q
              double Q = feld(pos) ;
              pos[inds[3]]=2 ; // select pos component for Stokes U
              double U = feld(pos) ;
              complex<double> val(Q,U) ;
              vals(curIndx,x,y) = val ;
            }
          }
          curIndx++ ;
        }
      }
    }
  }
  
  /*! procedure to allocate memory for the current rmCube.
      The procedure requires, that the int values for the size 
      of the images xsize and ysize are already set.
      Thurther more it assumes that either number of frequnecies of the 
      number of faraday depths is set.
      \param boolean which decides, whether the memory is allocated
          for the faraday depths or for the frequencies. */

 void rmCube::allocMemory(bool faraday) {
       /* create vector for frequencies, lambd squared or faradaydepths */
    switch(axis) {
       case 1:
// 	freqs.resize(freqSize) ;
       break ;
       case 2:
	lambdaSqs.resize(freqSize) ;
       break ;
       case 3:
	faradayDepths.resize(faradaySize) ;
       break ;
    } 
    /* end differnt cases for the meaning of the firt axis */
    this->buffer=NULL;	// set buffer to NULL (no buffer associated, yet)
    if (faraday) {
      vals.resize(faradaySize,xSize,ySize) ;
    }
    else {
      vals.resize(freqSize,xSize,ySize) ;
    }
 }

  vec rmCube::getFreqsCenters() {
    vec res(freqs) ;
    return res ;
  }

  vec rmCube::getFreqsLengths() {
    vec res(freqsInter) ;
    return res ;
  }

 void rmCube::setFirstCoord(uint index, double val) {
    switch(axis) {
       case 1:
	freqs[index] = val ;
       break ;
       case 2:
	lambdaSqs[index] = val ;
       break ;
       case 3:
	faradayDepths[index] = val ;
       break ;
    }
 }
 complex<double> rmCube::getVal(uint f, uint x, uint y) {
   return vals(f,x,y) ;
 }

 void rmCube::setVal(uint f, uint x, uint y, complex<double> val) {
     vals(f,x,y) = val ;
 }
  
  /*!
    \brief Constructor with setting individual Faraday depth vector
    
    \param x - Horizontal dimension in pixels
    \param y - Vertical dimension in pixels
    \param faradayDepths - total extension in Faraday depth
  */
  rmCube::rmCube(int x, int y, vector<double> faradayDepths)
  {
    // Check parameters for consistency
    if(x<=0)
      throw "rmCube::rmCube x dimension is <=0";
    if(y<=0)
      throw "rmCube::rmCube y dimension is <=0";
    if(faradayDepths.size()==0)
      throw "rmCube::rmCube faradayDepths has size 0";
    
    this->xSize=x;
    this->ySize=y;
    this->faradaySize=faradayDepths.size();
    
    this->buffer=NULL;	// set buffer to NULL (no buffer associated, yet)
    
    // Set remaining attributes to defaults
    this->currentX=0;
    this->currentY=0;
    this->currentFaradayDepth=0;
    this->ra=0;
    this->dec=0;
    this->ra_low=0;
    this->ra_high=0;
    this->dec_low=0;
    this->dec_high=0;
    vals.resize(faradayDepths.size(),x,y) ;
    for(unsigned int i=0; i<faradayDepths.size(); i++)		// loop over faradayDepths vector given
      {
	faradayDepths[i]=faradayDepths[i];			// write Faraday depth into list to probe for
      }
    
    //   cout << "rmCube::rmCube(int x, int y, vector<double> faradayDepths) constructor" << endl;  
  }
  
  
  //===============================================================================
//
//  Methods
//
//===============================================================================
//
// Reading info from the object
//
//===============================================================================

/*! Procedure reads the complete directory, which is adressed by the path stored inside 
    the strin path. 
    \param path of the directory to load all containing file names from
    \param dats vector for string the names of all files of the directory */

void rmCube::readDir(string path, vector<string> &dats) {
  DIR *pdir = NULL; 
  pdir = opendir(path.c_str()) ;
  if (pdir == NULL) {
    cerr << "open path failed" << endl ;
    throw ("open path failed") ;
  }
  struct dirent *pent = NULL;
  while ((pent = readdir (pdir))) {// while there is still something in the directory to list
    if (pent == NULL) {// if pent has not been initialised correctly
         // print an error message, and exit the program
         cerr <<"file not initialized" << endl ;
	 throw("file not initialized") ;
    }
    // otherwise, it was initialised correctly. let's print it on the console:
//     cout <<  pent->d_name << endl ;
    string name(pent->d_name) ;
    dats.push_back(name);
  }
  sort(dats.begin(), dats.end()) ;	 
// finally, let's close the directory
  closedir (pdir);
}
/*! fills the datas of the fits images, found inside the given directories into the 
    value array of the rmCube object
    \param qimag vector of the names of the fits images of the q part of the image
    \param uimag vector of the names of the fits images of the u part of the image
    \param f    number of frequencies or faraday depths
    \param x    size of each image in x-direction 
    \param y    size of each image in y-direction */

void rmCube::fillRM_Cube(string &qpath, string &upath, vector<string> &qimag, vector<string> &uimag, uint f, uint x, uint y, double fmin, double fmax) {
  if (qimag.size() == uimag.size()) { // only read if the number of files is the same 
    const string freqName("FREQ");
    for (uint i=0; i<qimag.size(); i++) {
      /* create the hall path for the file names */
      string qall = qpath+"/"+qimag[i] ;
      string uall = upath+"/"+uimag[i] ;
      /* open current fits files for q- and u- component of polarised signal */
//       fitshandle qfits(qall.c_str()) ;
//       fitshandle ufits(uall.c_str()) ;
//       /* goto hdu 2 where the image is */
//       qfits.goto_hdu (2) ;
//       ufits.goto_hdu (2) ;
//       if (qfits.key_present(freqName)) {
// 	double freq ; 
//         qfits.get_key(freqName,freq) ;
// 	if (i%10 == 0) cout <<"=" ;
//       }
//       /* read the tow images */
//       arr2<double> qvals ;
//       arr2<double> uvals ;
//       qfits.read_image(qvals) ;
//       ufits.read_image(uvals) ;
//       /* check the dimensions of the file compared to the tested one */
//       if ((qvals.size1() != x) ||  (uvals.size1() != x) || (qvals.size2() != y) ||  (uvals.size2() != y)) {
//         cerr << "dimensions of the different images are incompatible" << endl ;
// 	throw("dimensions of the different images are incompatible") ;
//       }
//       /* loops for reading the real values form the */
//       for (uint k=0; k<y; k++) {
// 	for (uint l=0; l<x; l++) {
// 	   double real = qvals[k][l] ;
// 	   double imag = uvals[k][l] ;
// 	   complex<double> val(real,imag) ;
// 	   setVal(i,l,k,val) ;
// 	}
//       }
    }
    /* fill the first coordinate axis equdistant from fmin to fmax */
    double delta = (fmax-fmin)/(f-1) ;  // stepsize of the interval
    /* loop over the number of faradaydepths frequencies of lambda squares */
    for (uint i=0; i<f ; i++) {
	double val = fmin+i*delta ;
	setFirstCoord(i, val) ;
    }
    cout << endl ;
  }
  else {
    cerr << "number of q_files and u_files ist not the same" << endl ;
    throw ("number of q_files and u_files ist not the same") ;
  }
}

int rmCube::getXSize()
{
  return xSize;
}

uint rmCube::getfreqSize() {
  return freqSize ;
}
int rmCube::getYSize()
{
  return ySize;
}


int rmCube::getFaradaySize()
{
  return faradaySize;
}


int rmCube::getCurrentX()
{
  return currentX;
}


int rmCube::getCurrentY()
{
  return currentY;
}


//===============================================================================
//
// Writing info to the object
//
//===============================================================================

int rmCube::setCurrentX(uint currentX)
{
  if(currentX >= this->xSize)
  {
    throw "rmCube::setCurrentX, value out of range";
    return -1;
  }
  else
  {
    this->currentX=currentX;
  }
  
  return 0;
}


int rmCube::setCurrentY(uint currentY)
{

  if(currentY >= this->ySize)
  {
    throw "rmCube::setCurrentY, value out of range";
    return -1;
  }
  else
  {
    this->currentY=currentY;
  }

  return 0;
}


double rmCube::getRA()
{
  return this->ra;
}


double rmCube::getDec()
{
  return this->dec;
}


void rmCube::setRA(double ra)
{
  if(ra>=0)
    this->ra=ra;
  else
    throw "rmCube::setRA: out of range";
}


void rmCube::setDec(double dec)
{
  if(dec>=0)
    this->dec=dec;
  else
    throw "rmCube::setDec: out of range";
}


double rmCube::getRaLow()
{
  return this->ra_low;
}


double rmCube::getDecLow()
{
  return this->dec_low;
}


double rmCube::getRaHigh()
{
  return this->ra_high;
}


double rmCube::getDecHigh()
{
  return this->dec_high;
}


double rmCube::getFaradayLow()
{
  return this->faraday_low;
}


double rmCube::getFaradayHigh()
{
  return this->faraday_high;
}


vector<double> rmCube::getFaradayDepths()
{
  return this->faradayDepths;
}


void rmCube::setFaradayDepths(vector<double> &depths)
{
  this->faradayDepths=depths;
}


void rmCube::setFaradayDepths(double low, double high, double stepsize)
{   
  this->faraday_low=low;
  this->faraday_high=high;
  this->faradaySize=abs(high-low)*stepsize;
}


void rmCube::setFaradayLow(double faradayLow)
{
  this->faraday_low=faradayLow;
}
    

void rmCube::setFaradayHigh(double faradayHigh)
{
  this->faraday_high=faradayHigh;
}
    
    
void rmCube::createBuffer(long long size)
{    
  if(buffer==NULL)	// check if we have already a buffer
  {
 //  this->buffer=(double *) calloc(size, sizeof(double)); // allocate memory of size Bytes
    this->buffer=new double[size];		// allocate memory for No. size of type double

    if(this->buffer==NULL)
    {
      throw "rmCube::createBuffer memory allocation failed";
    }
  }
  else
  {
    throw "rmCube::createBuffer buffer exists";
  }
}


void rmCube::deleteBuffer()
{
  if(this->buffer!=NULL)
  {
//    free(this->buffer); 
    delete buffer;
  }
  else
  {
    throw "rmCube::deleteBuffer";
  }
}


void rmCube::createBufferPlane()
{
  if(buffer==NULL)	// check if we have already a buffer
  {
//    this->buffer=(double *) calloc(this->xSize*this->ySize, sizeof(double)); // allocate memory for one Faraday plane
    this->buffer= new double[xSize*ySize];

    if(this->buffer==NULL)
    {
      throw "rmCube::createBufferPlane memory allocation failed";
    } 
  }
  else
  {
    throw "rmCube::createBufferPlane buffer already exists";
  }
}


void rmCube::createBufferCube()
{
  vector<int> dimensions(3);

  if(this->buffer==NULL)	// check if we have already a buffer
  {
   if(this->xSize > 0 && this->ySize > 0 && this->faradaySize > 0)
   {
//       this->buffer=(double *) calloc(xSize*ySize*faradaySize, sizeof(double)); // allocate memory of size Bytes
      this->buffer=new double[xSize*ySize*faradaySize];
   }

   if(this->buffer==NULL)
   {
    throw "rmCube::createBufferCube memory allocation failed";
   }
   
   // Set parameters
   if(this->xSize > 0)
    dimensions[0]=this->xSize;
   if(this->ySize > 0)
    dimensions[1]=this->ySize;
   if(this->faradaySize > 0)
    dimensions[2]=this->faradaySize;
   
   this->setBufferDimensions( dimensions );
  }
  else
  {
    throw "rmCube::createBufferCube buffer already exists";
  }
}

/*! procedure returns the polarised values the point of the images along one line
   of sight. The points can blong to different frequencies of to diffenrent 
   faraday depths 
   \param x index of the positions insight each plane image
   \param y index of the positions insight each plane image
   \param line vector to return the complex values of the line of sight */

void rmCube::getLineOfSight(uint x, uint y, vector<complex<double> > &line) {
   /* loop over the number of images */
  if (line.size() != faradaySize) {
    line.resize(faradaySize) ;
//     cerr << "LineOfSight: vector line has  incompatible length" <<endl ;
//     throw ("LineOfSight: vector line has  incompatible length") ;
  }
  for (uint i=0; i<line.size(); i++) {
    line[i] = getVal(i,x,y) ;
  }
}

/*! procedure returns the polarised values the point of the images along one line
   of sight. The points can blong to different frequencies of to diffenrent 
   faraday depths 
   \param x index of the positions insight each plane image
   \param y index of the positions insight each plane image
   \param line vector to return the complex values of the line of sight */

void rmCube::getLineOfSight(uint x, uint y, cvec &line) {
   /* loop over the number of images */
  if (line.size() != freqSize) {
    line.data.resize(freqSize) ;
//     cerr << "LineOfSight: vector line has  incompatible length" <<endl ;
//     throw ("LineOfSight: vector line has  incompatible length") ;
  }
  for (uint i=0; i<line.size(); i++) {
    line[i] = getVal(i,x,y) ;
  }
}

/*! procedure set the polarised values the point of the images along one line
   of sight. The points can blong to different frequencies of to diffenrent 
   faraday depths 
   \param x index of the positions insight each plane image
   \param y index of the positions insight each plane image
   \param line vector to return the complex values of the line of sight */

void rmCube::setLineOfSight(uint x, uint y, vector<complex<double> > &line) {
   /* loop over the number of images */
  if (line.size() != faradaySize) {
    cerr << "LineOfSight: vector line has  incompatible length" <<endl ;
    throw ("LineOfSight: vector line has  incompatible length") ;
  }
  for (uint i=0; i<line.size(); i++) {
    setVal(i,x,y,line[i]) ;
  }
}
vector<int> rmCube::getBufferDimensions()
{
  return this->bufferDimensions;
}


void rmCube::setBufferDimensions(vector<int> &dimensions)
{
  this->bufferDimensions=dimensions;
}


vector<double> rmCube::getLambdaSqs()
{
  return this->lambdaSqs;
}

vector<double> rmCube::getFreqs()
{
  return this->freqs;
}


void rmCube::setLambdaSqs(vector<double> &lambdaSqs)
{
  if(lambdaSqs.size())			// only if valid vector
    this->freqs=lambdaSqs;
}

void rmCube::setFreqs(vector<double> &freqs)
{
  if(freqs.size())			// only if valid vector
    this->freqs=freqs;
}


vector<double> rmCube::getDeltaLambdaSqs()
{
  return this->deltaLambdaSqs;
}


void rmCube::setDeltaLambdaSqs(vector<double> &deltaLambdaSqs)
{
  if(deltaLambdaSqs.size()!=0)
    this->deltaLambdaSqs=deltaLambdaSqs;
  else
    throw "rmCube::setDeltaLambdaSqs size 0";
}


std::string rmCube::getWeightingAlgorithm()
{
  return this->weightingAlgorithm;
}


void rmCube::setWeightingAlgorithm(std::string &weightingAlgorithm)
{
  this->weightingAlgorithm=weightingAlgorithm;
}


vector<double> rmCube::getWeights()
{
  return this->weights;
}


void rmCube::setWeights(vector<double> &weights)
{
  if(weights.size() != 0)
    this->weights=weights;
  else
    throw "rmCube::setWeights size=0";
}


std::string rmCube::getRMAlgorithm()
{
  return this->rmAlgorithm;
}


void rmCube::setRMAlgorithm(const std::string &algorithm)
{
  this->rmAlgorithm=algorithm;
}


vector<complex<double> > rmCube::getRMSF()
{
  return this->rmsf;
}


void rmCube::computeRMSF(const vector<double> &lambdaSqs, const vector<double> &deltaLambdaSqs, bool freq=true)
{
  if(faradayDepths.size()!=0)
  {
    // use rmCube attributes (must be set) class rm method to compute RMSF
    this->rmsf=rm::RMSF(faradayDepths, lambdaSqs, weights, deltaLambdaSqs, freq);
  }
}


//****************************************************
//
// High-level RM computing functions
//
//****************************************************


/*!
	\brief Compute one Faraday plane with algorithm given in class attribute
*/
void rmCube::computePlane(double faradayDepth)
{
	if(rmAlgorithm=="")		// if algorithm was not set...
		throw "rmCube::computePlane algorithm is not set";
   else if(rmAlgorithm!="rmsynthesis" || rmAlgorithm!="wienerfilter" || rmAlgorithm!="wavelet")
		throw "rmCube::computePlane unknown algorithm in attribute";
  else if(faradayDepths.size()==0)
	   throw "rmCube::computePlane faradayDepths attribute is not set";

  // loop over x and y to compute one plane

  // Problem need access to whole image Data, this is not abstracted yet
}


/*!
	\brief Compute the whole cube with algorithm given in class attribute
*/
void rmCube::computeCube()
{
	if(rmAlgorithm=="")		// if algorithm was not set...
		throw "rmCube::computeCube algorithm is not set";
   else if(rmAlgorithm!="rmsynthesis" || rmAlgorithm!="wienerfilter" || rmAlgorithm!="wavelet")
		throw "rmCube::computeCube unknown algorithm in attribute";
   else if(faradayDepths.size()==0)
		throw "rmCube::computeCube faradayDepths attribute is not set";



	// loop over x and y for each plane in faradayDepths

  // Problem need access to whole image Data, this is not abstracted yet
}



/*!
  \brief Compute RM with parameters from rmCube attributes
  
  \return RM along line of sight at Faraday Depths as defined in rmCube attribute
*/
/*
vector<complex<double> > rmCube::inverseFourier()
{
  return rm::RMSF(faradayDepths, lambdaSqs, weights, deltaLambdaSqs, false);
}
*/

}  //  END -- namespace RM
