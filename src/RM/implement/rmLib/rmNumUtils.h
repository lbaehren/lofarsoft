

// Standard header files
#include <iostream>
#include <string>
#include <vector>
# include <sys/types.h> 
// IT++ header files
#ifdef HAVE_ITPP
#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#endif
// RM header files
#include "rmNoise.h"

using namespace std;

#define DEBUG 1
//! statistical limit for number of line of sights before attempting a S(i,j) iteration
#define LOSLIMIT 100

namespace RM {  //  namespace RM
  
  void calcMeanVarDist(vector<double> &freqs, vector<double> &result) ;
  void findGaps(vector<double> &freqs, vector<uint> &result) ;
  
  // Forward declaration
  class mat ;

  /*!
    \class permEntry
    \ingroup RM
    \brief Permanent entry
  */
  class permEntry {
  public:
    uint pos1;
    uint pos2;     
    permEntry(uint w1,uint w2) {
      pos1 = w1 ;
      pos2 = w2 ;
    }
  } ;

  /*! 
    \class cvec
    \ingroup RM
    \brief Class for a complex vector */
  class cvec {
  public :
    vector<complex<double> > data ;  // the actual data
    cvec(void ) {
    } ;
    /*! constructor which reservese the memory for size elements */
    cvec(uint size) {
      data.resize(size) ;
    };
    /*! constructor makes a cvec from a stl vector */
    cvec(vector<complex<double> > &vek) {
      data=vek ;
    } ;
    /*! resizes the dimension of the current vector */
    void set_size(uint size) {
      data.resize(size);
    } ;
    /*! set the value of the element index ind to value val */
    void set(uint ind, complex<double> val) {
      data[ind] = val ;
    }
    /*! returns the size of the vector */
    uint size() {
      return data.size() ;
    } ;
    /*! returns the size of the vector */
    uint length() {
      return data.size() ;
    } ;
    /*! returns a stl vector of the same data as the current vector */
    vector<complex<double> > getData() {
      return data ;
    }
    /*! returns the n-th element of the vector */
    complex<double> &operator[](uint n) { 
      return data[n] ;
    }
    /*! returns the n-th element of the vector */
    complex<double> operator()(uint n) {
      return data[n] ;
    }
    /*! scalar multiplication of two vectors */
    const complex<double> operator* (const cvec &vec) const {
      return vec*data ;
    } ;
    /*! scalat multiplication of a cvec whith a stl complex vector */
    const complex<double> operator* (const vector<complex<double> > &vec) const {
      complex<double> erg = 0 ;
      if (data.size() != vec.size()) {
	throw "imcomplatible dimensions \n"; 
      }
      else {
	for (uint i=0; i<data.size(); i++) {
	  erg = erg + data[i]*vec[i] ;
	}
      }
      return erg ;
    };   
    /*! scalat multiplication of a cvec whith a stl double vector */
    const complex<double> operator* (const vector<double> &vec) const {
      complex<double> erg = 0 ;
      if (data.size() != vec.size()) {
	throw "imcomplatible dimensions \n"; 
      }
      else {
	for (uint i=0; i<data.size(); i++) {
	  erg = erg + data[i]*vec[i] ;
	}
      }
      return erg ;
    };    
  } ;

  /*!
    \class cmat
    \ingroup RM
    \brief class for handling a complex matrix
  */
  class cmat {     
     public:
        bool inverted ;
        vector<vector<complex<double> > > data ; // actual data 
        cmat() {
	   inverted = false ;
        } ;
	/*! constructor for a row x col matrix */
        cmat(uint row, uint col) {
	   inverted = false ;
	   data.resize(row) ;
	   for (uint i=0; i<row; i++) {
	     data[i].resize(col) ;
	   }
        } 
	/*! preset the matrix to a unity matrix */
	void fillUnit() ;
	/*! returns the absolute maimal value of the matrix */
	double maxi() ;
	/*! returns the squaroot of the absolute squared values form anf to the end of the stl vector vek
	    \param vek vector to calculate the value for
            \param anf start index from where the calculation starts */
	double abs(vector<complex<double> > vek, uint anf) ;
	/*! performs a gauss elimination on the matrix */
	cmat GaussEle() ;
	/*! returns the row index for the maximal pivot element (not a complete pivot search)
	    \param start start index for the search searching at mat[i,start] i=start .. dimension of the matrix */
	uint getMaxPivot(uint start) ;
	/*! performs a multiplication of a row of the matrix an a complex factor
	    \param fak factor to be multiplied to the row
	    \param row index of the rwo to be treated */
	void mult(complex<double> fak, uint row) ;
	/*! performs a adding of a product of one row(index addfrom) of the matrix to another row (addto) inside this seccond one
	    \param addto index of the goal row 
	    \param addfrom index of the added row
	    \param fak factor to multiply the row addfrom before adding */ 
	void add(uint addto, uint addfrom, complex<double> fak) ;
	/*! calculate determinant of the matrix */
	complex<double> det() ;
	/*! swap two collumns of the matrix 
	    \param ind1 index of the first collumn 
	    \param ind2 index of the seccond collumn */
	void swapCols(uint ind1, uint ind2);	
	/*! scalar product of two stl complex vectors */
	complex<double> prod(vector<complex<double> > vek1, vector<complex<double> > vek2) ;
	/*! number of rows of the matrix */
	uint rows() {
	  return data.size() ;
        }
	/*! calculates the absolute value of a complex number */
	double abs(complex<double> val) {
	  return sqrt(real(val*conj(val))) ;
	}
	/*! number of collumns of the matrix */
	uint cols() {
	  return data[0].size() ;
	}
	/*! get the value of the rowindex r and collumnindex c*/
	complex<double> get(int r, int c) {
	  vector<complex<double> > u = data[r] ;
	  complex<double> erg = u[c] ;
	  return erg ;
	}
	uint size() {
	  return data[0].size() ;
	}
	/*! resize the dimension of the matrix to 
	    \param srow new number of rows
	    \param scol new number of collumns */
	void set_size(uint srow, uint scol) {
	  data.resize(srow) ;
	  vector<vector<complex<double> > >::iterator it = data.begin() ;
	  while (it != data.end()) {
	    it->resize(scol) ;
	    it++ ;
	  }
	};
	/*! set an element of the matrix 
	  \param posR row index of the element 
	  \param posC col index of the element 
	  \param val new value of the element */
	void set(uint posR, uint posC,complex<double> val) {
	  data[posR][posC] = val ;
	} ;
	/*! product of the matrix with an cvek object result also an cvec object */
	cvec operator* (cvec &vec) {
	  uint nCol = rows() ;
	  cvec erg(nCol) ;
	  for (uint i=0; i<nCol; i++) {
	    vector<complex<double> > zw = data[i] ;
	    erg.set(i,vec*zw) ;
	  }
	  return erg ;
	} ;

        /*! product of the matrix with an cvek object result also an cvec object */
	vector<complex<double> > operator* (vector<complex<double> > &vec) {
	  uint nrows = rows() ;
	  uint ncols = cols() ;
	  vector<complex<double> > erg(nrows) ;
	  for (uint i=0; i<nrows; i++) {
	    vector<complex<double> > zw = data[i] ;
 	    complex<double> res(0,0) ;
	    for (uint j=0; j<ncols;j++) {
		res += zw[j]*vec[j] ;
	    }
	    erg[i]=res;
	  }
	  return erg ;
	} ;
	/*! matrix poduct of two complex matrices */
	cmat operator* (cmat &matr) {
   	  uint nrow = rows() ;
          uint ncol = matr.cols() ;
	  cmat temp = matr.t() ;
	  cmat erg(nrow,ncol) ;	
	  for (uint i=0; i<nrow;i++) {
	    vector<complex<double> > row = data[i] ;
	    for (uint j=0;j<ncol;j++) {
	      vector<complex<double> > col = temp.data[j] ;
	      erg.set(i,j,mult(row,col)) ;	      
	    }
	  }	
	  return erg ;
	}
	/*! Sum of two complex matrices */
	cmat operator+ (cmat &matr) {
   	  uint nrow = rows() ;
          uint ncol = cols() ;	  
	  cmat erg(nrow,ncol) ;
	  for (uint i=0; i<nrow; i++) {
	    vector<complex<double> > u1 = data[i] ;
	    vector<complex<double> > u2 = matr.data[i] ;
	    for (uint j=0; j<u1.size() ; j++) {
		erg.set(i,j,u1[j]+u2[j]) ;
	    }
	  }		  	
	  return erg ;
	}
	/*! procedure returns the pivot value of a
	    complex vector from a specified index */
	double getPivot(vector<complex<double> > &vek, uint start) ;
	cmat operator* (mat &matr) ;
	complex<double> operator()(uint row, uint col) {
           vector<complex<double> >  u = data[row] ;
	   return u[col] ;
	}
        void absOut(string fileName) ;
	void write(string fileName) ;
        /*! inverse of the current matrix */
	cmat inv() ;
	/*! complex adjunct of the current matrix */
	cmat H() ;
	cmat t() ;
	complex<double> mult(vector<complex<double> > v1,vector<complex<double> > v2) ;
	complex<double> mult(vector<complex<double> > v1,vector<double> v2) ;
	cmat* inverse ;
  };
  /*! class for a real matrix */
  class mat {    
    public:
	bool inverted ;
        vector<vector<double> > data ; // actual data 
        mat() {
	    inverted = false ;
        } ;
	/*! constructor for a row x col matrix */
        mat(uint row, uint col) {
	   inverted = false ;
	   data.resize(row) ;
	   for (uint i=0; i<row; i++) {
	     data[i].resize(col) ;
	   }
        } 

	/*! returns the squaroot of the absolute squared values form anf to the end of the stl vector vek
	    \param vek vector to calculate the value for
            \param anf start index from where the calculation starts */
	double abs(vector<double> vek, uint anf) ;
	double prod(vector<double> vek1, vector<double>  vek2) ;
	double getPivot(vector<double> &vek, uint start) ;
	void add(uint addto, uint addfrom, double fak) ;
	void swapCols(uint ind1, uint ind2) ;
	double maxi() ;
	mat GaussEle() ;
	void write(string fileName) ;
	void fillUnit() ;
	uint getMaxPivot(uint start);
	void mult(double fak, uint row) ;
	uint rows() {
	  return data.size() ;
        }		    	     
	uint cols() {
	  return data[0].size() ;
	}
	void set_size(uint srow, uint scol) {
	  data.resize(srow) ;
	  vector<vector<double> >::iterator it = data.begin() ;
	  while (it != data.end()) {
	    it->resize(scol) ;
	    it++ ;
	  }
	}
	void zeros() {
	  vector<vector<double> >::iterator it1 = data.begin() ;
	  while (it1 != data.end()) {
	    vector<double>  uv = *it1 ;
	    vector<double>::iterator it2 = uv.begin() ;
	    while (it2 != uv.end()) {
	 	*it2 = 0.0 ;
		it2++;
	    }
	    it1++ ;
	  }	
	}
	
	double &operator() (uint row, uint col) {
	  vector<double> r = data[row];
	  return r[col];
	}
	
	cvec operator* (cvec &vec) {
	  uint nCol = cols() ;
	  cvec erg(nCol) ;
	  for (uint i=0; i<nCol; i++) {
	    vector<double> zw = data[i] ;
	    erg[i] = vec*zw ;
	  }
	  return erg ;
	} ;

        /*! matrix poduct of two complex matrices */
	mat operator* (mat &matr) {
   	  uint nrow = rows() ;
          uint ncol = matr.cols() ;
	  mat temp = matr.H() ;
	  mat erg(nrow,ncol) ;	
	  for (uint i=0; i<nrow;i++) {
	    vector<double>  row = data[i] ;
	    for (uint j=0;j<ncol;j++) {
	      vector<double>  col = temp.data[j] ;
	      erg.set(i,j,prod(row,col)) ;
	    }
	  }	
	  return erg ;
	}
	
	void set(uint posR, uint posC,double val) {
	  data[posR][posC] = val ;
	}
	double get(int r, int c) {
	  vector<double> u = data[r] ;
	  double erg = u[c] ;
	  return erg ;
	}
	mat inv() ;
	void absOut(string fileName) ;
	mat H() ;
	double det() ;
	mat* inverse;
  };
  
  class vec {
    vector<double> data ;
    public :
      void set_size(uint size) {
	 data.resize(size);
      } ;
      double &operator[](uint n) { 
	  return data[n] ;
      }
      double operator()(uint n) {
	  return data[n] ;
      }
      uint size() {
	return data.size() ;
      }
      uint length() {
	return data.size() ;
      }
  };
}  // end of the namespace RM
