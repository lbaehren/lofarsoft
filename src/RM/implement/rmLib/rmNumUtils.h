#ifndef NumUtils_H
#define NumUtils_H	

// Standard header files
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_sf_erf.h>
#include <gsl/gsl_eigen.h>
//#include <omp.h>

#define DEBUG 1
#define uint unsigned int
//! statistical limit for number of line of sights before attempting a S(i,j) iteration
#define LOSLIMIT 100
#define PI 3.1415926535897932385
#define CVAC 299792458
namespace RM {  //  namespace RM
using namespace std;
void calcMeanVarDist(vector<double> &freqs, vector<double> &result) ;
void findGaps(vector<double> &freqs, vector<uint> &result) ;
void FFT(vector<complex<double> > &input, vector<complex<double> > &result) ;
void iFFT(vector<complex<double> > &input, vector<complex<double> > &result) ;
uint maxAbs(vector<complex<double> > vekt) ;
double meanPeak(vector<complex<double> > vekt) ;
complex<double> ehoch(double phi) ;
  class cVecFunk {
    public:
    virtual complex<double> function(uint index) =0 ;
  };
  class mat ;
  class cmat ;
  class vec ;
  class permEntry {
     public:
     uint pos1;
     uint pos2;     
     permEntry(uint w1,uint w2) {
	 pos1 = w1 ;
	 pos2 = w2 ;
     }
  } ;
  /*! class for a complex vector */
  class cvec {
    public :
      /*! calculates the absolute value of a complex number */
      double Abs(complex<double> val) {
	  return sqrt((val*conj(val)).real()) ;
      }
      vector<complex<double> > data ;  // the actual data
      cvec(void ) {
      } ;
      /*! constructor which reservese the memory for size elements */
      cvec(uint size) {
	 data.resize(size) ;
      };
      cvec(uint size, complex<double> val) {
	data.resize(size) ;
	for (uint i=0; i<size; i++) {
	  data[i] = val ;
	}
      }
      cvec(cVecFunk *fu, uint dim)  {
	data.resize(dim) ;
	for (uint i=0; i<dim; i++) {
	  data[i] = fu->function(i) ;
	}
      }
      /*! create complex vector from gsl vector */
      cvec(gsl_vector_complex *gsl_vec) {
	  uint size = gsl_vec->size ;
	  data.resize(size) ;
	  for (uint i=0; i<size; i++) {
	    gsl_complex val = gsl_vector_complex_get(gsl_vec,i) ;
	    complex<double> cval(val.dat[0],val.dat[1]);
	    data[i] = cval ;
	}
      }
      cvec(cmat &matr) ;
      /*! constructor makes a cvec from a stl vector */
      cvec(vector<complex<double> > &vek) {
        data=vek ;
      } ;
      /*! resizes the dimension of the current vector */
      void set_size(uint size) {
	 data.resize(size);
      } ;
      void set_vals(uint size, complex<double> val) {
	data.resize(size) ;
	for (uint i=0; i<size; i++) {
	  data[i] = val ;
	}
      } ;
      /*! set the value of the element index ind to value val */
      void set(uint ind, complex<double> val) {
	data[ind] = val ;
      }
      /*! multiplies all elements of the current vector with the factor fak.
	\param fak factor to be multiplied to all elements of the vector */
      void mult(complex<double> fak) ;
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
      /*! procedure add the elements of a seccond vector (vek2)
	to the elements of the current vector. The componenst of the seccond 
	vector are multiplied with an additional factor fak */
      void add(cvec &vek2, complex<double> fak) {
	uint dim1 = vek2.size() ;	
	uint dim = data.size() ;
	if (dim != dim1) {
	     cerr << "imcomplatible dimensions " << endl ;
	     throw "imcomplatible dimensions \n"; 
	}
	/* loop over the components of the two vectors */
	for (uint i=0; i<dim; i++) {
	  data[i] = data[i]+fak*vek2[i] ;
	}
      }

      void addSQ(cvec &vek2, complex<double> fak) {
	uint dim1 = vek2.size() ;	
	uint dim = data.size() ;
	if (dim != dim1) {
	     cerr << "imcomplatible dimensions " << endl ;
	     throw "imcomplatible dimensions \n"; 
	}
	/* loop over the components of the two vectors */
	for (uint i=0; i<dim; i++) {
	  data[i] = data[i]+fak*vek2[i]*conj(vek2[i]) ;
	}
      }
      /*! scalar multiplication of two vectors */
      const complex<double> operator* (const cvec &vec) const {
	  return vec*data ;
      } ;
      const cvec operator* (const complex<double> fak) {
        cvec result(data.size()) ;
        for (uint i=0; i<data.size(); i++) {
          result.data[i] = data[i]*fak ;
        }
        return result ;
      }
      cvec operator- (cvec &other) {
          if (data.size() != other.size()) {
	    cerr << "vector minus: imcomplatible dimensions \n" << endl ;
	    throw "imcomplatible dimensions \n"; 
	  }
   	  uint dim = size() ;
	  cvec erg(dim) ;
	  for (uint i=0; i<dim; i++) {
	    erg.set(i,data[i]-other[i]) ;
	  }		  	
	  return erg ;
	}
//       cvec& operator= (const cvec& other)  {  
//         if (this!=&other)  {
//            data = other.data;
//         }  
//         return *this;  
//       }

      /*! procedure prints the absolute values of the vector in a named file */
      void absOut(string fileName) ;
      void absOut(string fileName, uint start, uint end) ;
      void sepOut(string fileName) ;
      void valOut(string fileName) ;
      void realOut(string fileName) ;
      void imagOut(string fileName) ;
      vec real() ;
      vec imag() ;
      void flip() ;  //! flips the order of the current vector
      /*! Norm of the current complex vector */
      double Abs() {
        double erg=0 ;
        for (uint i=0; i<data.size(); i++) {
          double re = data[i].real() ;
	  double im = data[i].imag() ;
	  erg = erg + re*re+im*im;
	}
	return sqrt(erg);
      }
      /*! distance of two complex vectors */
      double Abs(cvec other) {
        double erg=0 ;
        for (uint i=0; i<data.size(); i++) {
          complex<double> diff = other[i]-data[i] ;
          double re = diff.real() ;
	  double im = diff.imag() ;
	  erg = erg + re*re+im*im;
	}
	return sqrt(erg);
      }
      /*! procedure which returns the fft of an vector */
      void fft(cvec &result) ;
      void fftshift(cvec &result) ;
      /*! procedure which returns the inverse fft of an vector */
      void ifft(cvec &result) ;

      /*! calculates the ll distance to the "other" vector */
      double Lldist(cvec other) {
	double erg = 0 ;
  	if (data.size() != other.size()) {
	   cerr << "imcomplatible dimensions \n" << endl ;
	   throw "imcomplatible dimensions \n"; 
	 }
	 else {
	   for (uint i=0; i<data.size(); i++) {
		erg = erg + Abs(data[i]-other[i]) ;
	   }
	 }
	 return erg ;
      }
      /*! scalat multiplication of a cvec whith a stl complex vector */
      const complex<double> operator* (const vector<complex<double> > &vec) const {
	 complex<double> erg = 0 ;
	 if (data.size() != vec.size()) {
	   cerr << "imcomplatible dimensions \n" << endl ;
	   throw "imcomplatible dimensions \n"; 
	 }
	 else {
	   for (uint i=0; i<data.size(); i++) {
		erg = erg + data[i]*vec[i] ;
	   }
	 }
	 return erg ;
      }   

      /*! addition of a cvec whith a stl complex vector */
      const cvec operator+ (const vector<complex<double> > &vec) const {
	 cvec erg(data.size()) ;	 
	 if (data.size() != vec.size()) {
	   cerr << "imcomplatible dimensions \n" << endl ;
	   throw "imcomplatible dimensions \n"; 
	 }
	 else {
	   for (uint i=0; i<data.size(); i++) {
		erg[i] = data[i]+vec[i] ;
	   }
	 }
	 return erg ;
      }   

	/*! scalat multiplication of a cvec whith a stl double vector */
      const complex<double> operator* (const vector<double> &vec) const {
	 complex<double> erg = 0 ;
	 if (data.size() != vec.size()) {
	   cerr << "imcomplatible dimensions \n" ; 
	   throw "imcomplatible dimensions \n"; 
	 }
	 else {
	   for (uint i=0; i<data.size(); i++) {
		erg = erg + data[i]*vec[i] ;
	   }
	 }
	 return erg ;
      };

      /*! returns the index of the vector element whith the largest 
          absolute value of the whole vector */
      uint maxAbs() {
        uint erg = 0 ;
 	double max=0 ;
        for (uint i=0; i< data.size(); i++) {
          complex<double> val = data[i] ;
	  if ((i==0) || (Abs(val)>max)) {
	    max=Abs(val) ;
	    erg = i ;
          }
        }
        return erg ;
      }

      /*! returns the index of the vector element whith the largest 
          absolute value of the real part of the whole vector */
      uint maxReal() {
        uint erg = 0 ;
 	double max=0 ;
        for (uint i=0; i< data.size(); i++) {
          complex<double> val = data[i] ;
	  if ((i==0) || (fabs(val.real())>max)) {
	    max=fabs(val.real()) ;
	    erg = i ;
          }
        }
        return erg ;
      }

      /*! returns the index of the vector element whith the largest 
          absolute value of the imaginary part of the whole vector */
      uint maxImag() {
        uint erg = 0 ;
 	double max=0 ;
        for (uint i=0; i< data.size(); i++) {
          complex<double> val = data[i] ;
	  if ((i==0) || (fabs(val.imag())>max)) {
	    max=fabs(val.imag()) ;
	    erg = i ;
          }
        }
        return erg ;
      }
  } ;
  /*! class for handling a complex matrix */
  class cmat {     
     public:
        bool inverted ;
	int id ;
        vector<vector<complex<double> > > data ; // actual data 
	double getMaxVal() ;
	double getMaxOfDiag() ;
	double getMinDiag() ;
	uint maxCol ;
	uint maxRow ;

        cmat() {
	   id = 1 ;
	   inverted = false ;
        } ;
        /*! constructor for a new Matrix, which is generated from signal*adj(signal)
	    where adj is the complex adjoint of the vector  */
	cmat(cvec signal) {
	  uint dim = signal.size() ;
	  inverted = false ;
	id = 2  ;
	  data.resize(dim) ;
	  for (uint i=0; i<dim; i++) {
	     data[i].resize(dim) ;
	     for (uint j=0; j<dim; j++) {
		data[i][j] = signal[i]*conj(signal[j]);
	     }
	  }
	}
	/*! constructor gets the diagonal elemnts of a diagonal matrix as a 
	    vector of comple numbers and creates a full Matrix from this elements */
	cmat(vector<double> diag) {
	  uint dim = diag.size() ;
	  data.resize(dim) ;
	  for (uint i=0; i<dim; i++) {
	     data[i].resize(dim) ;
	     for (uint j=0; j<dim; j++) {
		if(i==j)
		  data[i][j] = diag[i];
		else
		  data[i][j] = 0 ;
	     }
	  } 
	}
	/*! constructor for a row x col matrix */
        cmat(uint row, uint col) {
	   inverted = false ;
	   id = 3  ;
	   data.resize(row) ;
	   for (uint i=0; i<row; i++) {
	     data[i].resize(col) ;
	   }
        }
	/*! constructor for a row x col matrix init al values with val*/
        cmat(uint row, uint col, complex<double> val) {
	   inverted = false ;
	   id = 4  ;
	   data.resize(row) ;
	   for (uint i=0; i<row; i++) {
	     data[i].resize(col) ;
	     for (uint j=0; j<col; j++) {
		data[i][j] = val ;
	     }
	   }
        }
	cmat(const cmat& other) {
	  uint ro = other.data.size() ;
	  uint co = other.data[0].size() ;
	  inverted = false ;
	  id = 5  ;
	  data.resize(ro) ;
	  for (uint i=0; i<ro; i++) {
	     data[i].resize(co) ;
	     for (uint j=0; j<co; j++) {
		data[i][j] = other.data[i][j] ;
	     }
	  }
	}
	/*! constructor for a complex matrix from a complex gsl matrix */
	cmat(gsl_matrix_complex *gsl_mat) {
	   /* gt number of rows and columns of gsl matrix */
	   uint row = gsl_mat->size1 ;
	   uint col = gsl_mat->size2 ;
	   /* reserve memory */
	   data.resize(row) ;
	   for (uint i=0; i<row; i++) {
	     data[i].resize(col) ;
	     for (uint j=0; j<col; j++) {
		/* set value */
		gsl_complex val = gsl_matrix_complex_get(gsl_mat,i,j);
		complex<double> cval(val.dat[0],val.dat[1]);
		data[i][j] = cval ;
	     }
	   }
	}
	cmat(const vector<vector<complex<double> > > &other) {
	  uint ro = other.size() ;
	  uint co = other[0].size() ;
	  inverted = false ;
	  id = 5  ;
	  data = other ;
	  data.resize(ro) ;
	  for (uint i=0; i<ro; i++) {
	     data[i].resize(co) ;
	     for (uint j=0; j<co; j++) {
		data[i][j] = other[i][j] ;
	     }
	  }
        }

	cmat &operator= (const cmat& other)
        {
          id = 6;
	  inverted = false ;
          uint ro = other.data.size() ;
	  uint co = other.data[0].size() ;
	  inverted = false ;
	  data.resize(ro) ;
	  for (uint i=0; i<ro; i++) {
	     data[i].resize(co) ;
	     for (uint j=0; j<co; j++) {
		data[i][j] = other.data[i][j] ;
	     }
	  }
        return *this;
        }

	~cmat() {
	}
	void print() {
	  cout << "constructor "<< id << "  " << this << endl ;
	}
	gsl_matrix_complex *createGSL_Mat() ;
	gsl_matrix *createRealGSL_Mat() ;
	gsl_matrix *createGSL_Mat(bool real) ;
	void svd() ;
	/*! copies the diagonal element of the Matrix to a given vector */
	void copyToVec(cvec &vec) ;
	/*! multplies a vector of values to the diagonal of the current matrix
	    all other elements are not touched, the diagonal elements of the matrix
	    are changed, no new matrix is created */ 
        void multToDia(cvec fak) {
	  if ((data.size() != fak.size())|| (data[0].size() < data.size())) {
	    cerr << "cmat: imcomplatible dimensions \n" ; 
	    throw "imcomplatible dimensions \n"; 
	  }
	  else {
	    for (uint i=0; i<data.size(); i++) {
		data[i][i] = data[i][i]*fak[i] ;
	    }
	  }
	}
	/*! preset the matrix to a unity matrix */
	void fillUnit() ;
	void fillSymRand() ;
        cmat calcPosEig() ;
	/*! returns the absolute maimal value of the matrix */
	double maxi() ;
	/*! procedure checks, whether a matrix is hermitian, to an accuracy of eps.*/
	bool isHermitian(double eps) ;
	/*! returns the squaroot of the absolute squared values form anf to the end of the stl vector vek
	    \param vek vector to calculate the value for
            \param anf start index from where the calculation starts */
	double Abs(vector<complex<double> > &vek, uint anf) ;
	/*! performs a gauss elimination on the matrix */
	cmat GaussEle() ;
	/*! returns the row index for the maximal pivot element (not a complete pivot search)
	    \param start start index for the search searching at mat[i,start] i=start .. dimension of the matrix */
	/*! performs a full Jacobistep to the current matrix object, and changes the current Matrix in this way 
	    by multiplying it with UH*cmat*U where U is defined by the two paramters phi and psi
	    \param phi argument of the trigonometric part of the untrary transformation U
	    \param psi argument of the exponential part of the unitary Matrix U */
	void fullJacobiStep(double phi,double psi, uint j, uint k) ;
	bool JacobiIteration(cmat &trans, double aim, uint maxite) ;
	void HouseholderTrans(cmat &trans) ;
	double sigmaHouseholder(uint index) ;
	double sigmaHouseholder2(uint index) ;
	double betaHouseholder(uint index, double sig) ;
	double betaHouseholder2(uint index,double sig) ;
	void multTFromLeftToCol(vector<complex<double> > &u, double beta, uint ind) ;
	void multTFromRightToRow(vector<complex<double> > &u, double beta, uint ind) ;
	void multTFromLeft(vector<complex<double> > &u, double beta, bool all) ;
	void multTFromRight(vector<complex<double> > &u, double beta, bool all);
	void multTFromLeft2(vector<complex<double> > &u, double beta, bool all) ;
	void multTFromRight2(vector<complex<double> > &u, double beta, bool all) ;
	void performHouseholder(cmat &unity) ;
	bool performQR(cmat &unity, double aim, uint maxite) ;
	void performQR_step(cmat &unity) ;
	void singularValueDecompose() ;
	void copyGSLMat2data(gsl_matrix *gsl_mat, vector<vector<double> > &dat) ;
	void copyGSLcomplMat2data(gsl_matrix *gsl_mat, vector<vector<complex<double> > > &dat, vector<vector<int> > &test) ;
	void copyGSLMat2data(gsl_matrix *gsl_mat, vector<vector<complex<double> > > &dat) ;
	void copyGSLVec2data(gsl_vector *gsl_vec, vector<double>  &dat) ;
	complex<double> phaseHouseholder(uint index) ;
	complex<double> phaseHouseholder2(uint index) ;
	vector<complex<double> > uHouseholder(uint index, double sig) ;
	vector<complex<double> > uHouseholder2(uint index, double sig) ;
	mat real() ;
        mat imag() ;
	/*! half of the Jacobi step cmat*U */
	void rightJacobi(double phi,double psi, uint j, uint k) ;
	/*! half of the Jacobi step UH*cmat */
	void leftJacobi(double phi,double psi, uint j, uint k) ;
	uint getMaxPivot(uint start) ;
	/*! performs a multiplication of a row of the matrix an a complex factor
	    \param fak factor to be multiplied to the row
	    \param row index of the rwo to be treated */
	void mult(complex<double> fak, uint row) ;
	/*! multiplies a complex number to all entries of the matrix */
	void mult(complex<double> fak) ;
	/*! performs a adding of a product of one row(index addfrom) of the matrix to another row (addto) inside this seccond one
	    \param addto index of the goal row 
	    \param addfrom index of the added row
	    \param fak factor to multiply the row addfrom before adding */ 
	void add(uint addto, uint addfrom, complex<double> fak) ;
	/*! adds the tensor productct of the two vector to the current matrix 
	   \param vec1 first vector
	   \param vec2 seccond vector */
	void add(cvec &vec1, cvec &vec2) ;
	/*! calculate determinant of the matrix */
	complex<double> det() ;
	/*! calculate the determinant of the matrix assuming it is a diagonal */
	complex<double> diaDet() ;
	/*! calculate the determinant of the matrix assuming it is a diagonal hermitian matrix */
	double hermDiaDet() ;
	/*! swap two collumns of the matrix 
	    \param ind1 index of the first collumn 
	    \param ind2 index of the seccond collumn */
	void swapCols(uint ind1, uint ind2);	
	/*! scalar product of two stl complex vectors */
	complex<double> prod(vector<complex<double> > &vek1, vector<complex<double> > &vek2) ;
	/*! number of rows of the matrix */
	uint rows() {
	  return data.size() ;
        }
	/*! calculates the absolute value of a complex number */
	double Abs(complex<double> val) {
	  return sqrt((val*conj(val)).real()) ;
	}
	/*! number of collumns of the matrix */
	uint cols() {
	  return data[0].size() ;
	}
	/*! get the value of the rowindex r and collumnindex c*/
	complex<double> get(uint r, uint c) {
	  vector<complex<double> > u = data[r] ;
	  complex<double> erg = u[c] ;
	  return erg ;
	}
	uint size() {
	  return data[0].size() ;
	}

	bool isUnitary(double eps) ;
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
	/*! set an element of the matrix, handling the colindex as periodic 
	  \param posR row index of the element 
	  \param posC col index of the element 
	  \param val new value of the element */
	void set_peropdic(uint posR, uint posC,complex<double> val) {
	  uint num = data[posR].size() ;
	  uint col = (posC+num) % num ;
	  data[posR][col] = val ;
	} ;
	/*! product of the matrix with an cvek object result also an cvec object */
	cvec operator* (cvec &vec) {
	  uint nCol = rows() ;
	  cvec erg(nCol) ;
	  for (uint i=0; i<nCol; i++) {
	    vector<complex<double> > &zw(data[i]) ;
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
	    vector<complex<double> > &zw(data[i]) ;
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
	    vector<complex<double> > &row(data[i]) ;
//             double t0 = omp_get_wtime() ;
	    for (uint j=0;j<ncol;j++) {
	      vector<complex<double> > &col(temp.data[j]) ;
	      erg.set(i,j,mult(row,col)) ;
	    }
//             double t1 = omp_get_wtime() ;
// 	    if (i%17==0) cout << "iteration: " << i << "  Zeit: " << (t1-t0)/ncol << endl ;
	  }	
	  return erg ;
	}
	/*! Sum of two complex matrices */
	cmat operator+ (cmat &matr) {
   	  uint nrow = rows() ;
          uint ncol = cols() ;	  
	  cmat erg(nrow,ncol) ;
	  for (uint i=0; i<nrow; i++) {
	    vector<complex<double> > &u1(data[i]) ;
	    vector<complex<double> > &u2(matr.data[i]) ;
	    for (uint j=0; j<u1.size() ; j++) {
		erg.set(i,j,u1[j]+u2[j]) ;
	    }
	  }		  	
	  return erg ;
	}

	cmat operator- (cmat &matr) {
   	  uint nrow = rows() ;
          uint ncol = cols() ;	  
	  cmat erg(nrow,ncol) ;
	  for (uint i=0; i<nrow; i++) {
	    vector<complex<double> > &u1(data[i]) ;
	    vector<complex<double> > &u2(matr.data[i]) ;
	    for (uint j=0; j<u1.size() ; j++) {
		erg.set(i,j,u1[j]-u2[j]) ;
	    }
	  }		  	
	  return erg ;
	}

	/*! procedure returns the pivot value of a
	    complex vector from a specified index */
	double getPivot(vector<complex<double> > &vek, uint start) ;
	cmat operator* (mat &matr) ;
	complex<double> operator()(uint row, uint col) {
           vector<complex<double> >  &u(data[row]) ;
	   return u[col] ;
	}
        void absOut(string fileName) ;
	void valOut(string fileName) ;
	void realValOut(string fileName) ;
	void imagValOut(string fileName) ;
	void write(string fileName) ;
        /*! inverse of the current matrix */
	cmat inv() ;
	cmat pseudoinverse();
// 	void singulValueDecomp(uint QR) ;	
	/*! complex adjunct of the current matrix */
	cmat H() ;
	cmat t() ;
	complex<double> mult(vector<complex<double> > &v1,vector<complex<double> > &v2) ;
	complex<double> mult(vector<complex<double> > &v1,vector<double> &v2) ;
	vector<vector<complex<double> > > mult_M2MH(vector<vector<complex<double> > > &m1, vector<vector<complex<double> > > &m2) ;
	vector<vector<complex<double> > > mult_MH2M(vector<vector<complex<double> > > &m1, vector<vector<complex<double> > > &m2) ;
	vector<vector<double> > mult_M2MH(vector<vector<double> > &m1, vector<vector<double> > &m2) ;
	vector<vector<double> > mult_MH2M(vector<vector<double> > &m1, vector<vector<double> > &m2) ;
        vector<vector<complex<double> > > mult_MH2M(vector<vector<complex<double> > > &m1, vector<vector<double> > &m2) ;
	vector<vector<complex<double> > > mult_M2MH(vector<vector<complex<double> > > &m1, vector<vector<double> > &m2) ;
        vector<vector<complex<double> > > mult(vector<vector<complex<double> > > &m1, vector<vector<double> > &m2) ;
        vector<vector<double> > mult(vec &diag, vector<vector<double> > &mat) ;
	vector<vector<double> > mult(vector<vector<double> > &mat, vec &diag) ;
        vector<vector<complex<double> > > mult(vector<vector<double> >&m1 , vector<vector<complex<double> > > &m2) ;
        cvec solveWithSVD(cvec rs);
        void creatPseudoInverse() ;
	cmat* inverse ;
        cmat fft() ; 
  };
  /*! class for a real matrix */
  class mat {    
    public:
	bool inverted ;
	uint maxCol ;
	uint maxRow ;
        vector<vector<double> > data ; // actual data 
	vector<vector<double> > unitary1; // first unitary matrix used for the singular value decomposition
	vector<vector<double> > unitary2; // seccond unitary matrix used for the singular value decomposition
	vector<double> singVals; // seccond unitary matrix used for the singular value decomposition
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
	/*! constructor gets the diagonal elemnts of a diagonal matrix as a 
	    vector of comple numbers and creates a full Matrix from this elements */
	mat(vector<double> diag) {
	  uint dim = diag.size() ;
	  data.resize(dim) ;
	  for (uint i=0; i<dim; i++) {
	     data[i].resize(dim) ;
	     for (uint j=0; j<dim; j++) {
		if(i==j)
		  data[i][j] = diag[i];
		else
		  data[i][j] = 0 ;
	     }
	  } 
	}

	/* create a matrix object from an gsl matrix */
	mat(gsl_matrix *gsl_mat) {
	   /* gt number of rows and columns of gsl matrix */
	   uint row = gsl_mat->size1 ;
	   uint col = gsl_mat->size2 ;
	   /* reserve memory */
	   data.resize(row) ;
	   for (uint i=0; i<row; i++) {
	     data[i].resize(col) ;
	     for (uint j=0; j<col; j++) {
		/* set value */
		double val = gsl_matrix_get(gsl_mat,i,j);
		data[i][j] = val ;
	     }
	   }
	}
	mat(const vector<vector<double> > &other) {
	  uint ro = other.size() ;
	  uint co = other[0].size() ;
	  inverted = false ;
	  data = other ;
	  data.resize(ro) ;
	  for (uint i=0; i<ro; i++) {
	     data[i].resize(co) ;
	     for (uint j=0; j<co; j++) {
		data[i][j] = other[i][j] ;
	     }
	  }
        }
	/*! constructor for a row x col matrix init al values with val*/
        mat(uint row, uint col, double val) {
	   inverted = false ;
	   data.resize(row) ;
	   for (uint i=0; i<row; i++) {
	     data[i].resize(col) ;
	     for (uint j=0; j<col; j++) {
		data[i][j] = val ;
	     }
	   }
        }

	double getMaxVal() ;
  	double getMaxOfDiag() ;
	double getMinDiag() ;
	void copyGSLMat2data(gsl_matrix *gsl_mat, vector<vector<double> > &dat) ;
	void copyGSLVec2data(gsl_vector *gsl_vec, vector<double>  &dat) ;
	/*! returns the squaroot of the absolute squared values form anf to the end of the stl vector vek
	    \param vek vector to calculate the value for
            \param anf start index from where the calculation starts */
	double Abs(vector<double> &vek, uint anf) ;
	double prod(vector<double> &vek1, vector<double>  &vek2) ;
	double getPivot(vector<double> &vek, uint start) ;
	void add(uint addto, uint addfrom, double fak) ;
	void swapCols(uint ind1, uint ind2) ;
	void singularValueDecompose() ;
	void svd() ;
	gsl_matrix *createGSL_Mat() ;
	double maxi() ;
	mat GaussEle() ;
	/*! performs a full Jacobistep to the current matrix object, and changes the current Matrix in this way 
	 by multiplying it with OT*mat*O where O is defined by the paramter phi
	 \param phi argument of the trigonometric part of the orthogonal transformation O */
	void fullJacobiStep(double phi, uint j, uint k) ;
	/*! half of the Jacobi step mat*O */
	void rightJacobi(double phi, uint j, uint k) ;
	/*! half of the Jacobi step OT*mat */
	void leftJacobi(double phi, uint j, uint k) ;
	/*! performs a compleate step of the Jacobi iteration, which means is looks for the 
	    parameter of the next orthogonal transformation,*/
	bool JacobiIteration(mat &trans, double aim, uint maxite) ;
	void write(string fileName) ;
	void fillUnit() ;
	void fillSymRand() ;
	uint getMaxPivot(uint start);
	void mult(double fak, uint row) ;
	void mult(double fak) ;
        vec operator* (vec &vek) ;
	/*! copies the diagonal element of the Matrix to a given vector */
	void copyToVec(vec &vals) ;
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
	  vector<double> &r(data[row]);
	  return r[col];
	}
	
	cvec operator* (cvec &vec) {
	  uint nRows = rows() ;
	  cvec erg(nRows) ;
	  for (uint i=0; i<nRows; i++) {
	    vector<double> &zw(data[i]) ;
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
	    vector<double>  &row(data[i]) ;
	    for (uint j=0;j<ncol;j++) {
	      vector<double>  &col(temp.data[j]) ;
	      erg.set(i,j,prod(row,col)) ;
	    }
	  }	
	  return erg ;
	}
        /* implementation of the minus ofperator for two real matrices */
	mat operator- (mat &matr) {
   	  uint nrow = rows() ;
          uint ncol = cols() ;	
          if ((matr.rows()!=rows()) && (matr.cols() != cols())) {
            cerr << "Error in operator '-' of mat: The dimensions of the two matrixes mut be identic" << endl  ;
 	    throw  "Error in operator '-' of mat:" ;
	  }
	  mat erg(nrow,ncol) ;
	  for (uint i=0; i<nrow; i++) {
	    vector<double> &u1(data[i]) ;
	    vector<double> &u2(matr.data[i]) ;
	    for (uint j=0; j<u1.size() ; j++) {
		erg.set(i,j,u1[j]-u2[j]) ;
	    }
	  }		  	
	  return erg ;
	}

	void set(uint posR, uint posC,double val) {
	  data[posR][posC] = val ;
	}
	double get(int r, int c) {
	  vector<double> &u(data[r]) ;
	  double erg = u[c] ;
	  return erg ;
	}
	mat inv() ;
	mat pseudoinverse();
	void absOut(string fileName) ;
	void valOut(string fileName) ;
	mat H() ;
	double det() ;
	double diaDet() ;
	mat* inverse;
  };
  
  class vec {
    public :
    vector<double> data ;    
      void set_size(uint size) {
	 data.resize(size);
      } ;

      vec(void ) {
      } ;
      /*! constructor which reservese the memory for size elements */
      vec(uint size) {
	 data.resize(size) ;
      };
      vec(vector<double> &other) {
        data = other ;
      }
      vec(uint size, double val) {
	data.resize(size) ;
	for (uint i=0; i<size; i++) {
	  data[i] = val ;
	}
      }
      /* create vector from gsl vector */
      vec(gsl_vector *gsl_vec) {
	  uint size = gsl_vec->size ;
	  data.resize(size) ;
	  for (uint i=0; i<size; i++) {
	    double val = gsl_vector_get(gsl_vec,i) ;
	    data[i] = val ;
	}
      }
      /*! set the value of the element index ind to value val */
      void set(uint ind, double val) {
	data[ind] = val ;
      }
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
      /*! scalat multiplication of a cvec whith a stl complex vector */
      const double operator* (const vector<double> &vec) const {
	 double erg = 0 ;
	 if (data.size() != vec.size()) {
	   cerr << "vector mult: imcomplatible dimensions \n" << endl ;
	   throw "imcomplatible dimensions \n"; 
	 }
	 else {
	   for (uint i=0; i<data.size(); i++) {
		erg = erg + data[i]*vec[i] ;
	   }
	 }
	 return erg ;
      }   
      vec operator- (vec &other) {
          if (data.size() != other.size()) {
	    cerr << "vector minus: imcomplatible dimensions \n" << endl ;
	    throw "imcomplatible dimensions \n"; 
	  }
   	  uint dim = size() ;
	  vec erg(dim) ;
	  for (uint i=0; i<dim; i++) {
	    erg.set(i,data[i]-other[i]) ;
	  }		  	
	  return erg ;
	}

      vec operator+ (vec &other) {
          if (data.size() != other.size()) {
	    cerr << "vector minus: imcomplatible dimensions \n" << endl ;
	    throw "imcomplatible dimensions \n"; 
	  }
   	  uint dim = size() ;
	  vec erg(dim) ;
	  for (uint i=0; i<dim; i++) {
	    erg.set(i,data[i]+other[i]) ;
	  }		  	
	  return erg ;
	}
      /*! procedure prints the absolute values of the vector in a named file */
      void absOut(string fileName) ;
      void valOut(string fileName) ;
      void flip() ;
  };

  class grid {
    public:
    double dphi ;
    uint nphi ;
    double l2_nonuni ;
    vec points ;
    cvec fgrid ;
    double start ;
    double stop ;
    double step ;
    double gcf_expsinc(double x, double dx) ;
    void arange(double start, double stop, double step) ;
    void make_x_grid(vec &x, uint m, double dy, uint ny) ;
    void pre_fft_gridding(cvec &f, vec &x, double dy, uint ny) ;
    void compute_dirty_image(cvec &pol, vec &x, double delta, uint num) ;
    /*! constructor for regular grid with 
        given start point given end point*/
    grid(double start, double stop, double step) {
      arange(start, stop, step) ;
    }
    grid() {
    }
  } ;
  double getVarForGauss(cvec data, vector<double> &faraday, int useClean, uint &lo, uint &index, uint &hi, double &max) ;
  void createPeak(vector<complex<double> > &data, double max, double sigma, vector<double> &faras,uint lo, uint index, uint hi, cvec &peak, uint useClean) ;
  void smoothPeak(cvec &peak, uint index, vector<double> &lcenter,vector<double> &linterv,vector<double> &faras, uint useClean) ;
  complex<double> calcFakt(cvec &peak,vector<double> &faras, vector<double> &lcenter,vector<double> linterv,double nu_0,double alpha,double epsilon, uint method, uint index) ;
  void freqToLambdaSq(const vector<double> &center, const vector<double> &intervals, vector<double> &lcenter, vector<double> &linterv) ;
  vector<complex<double> > performRMSynthesis(vector<complex<double> > &QU, vector<double> &lambC,vector<double> &lambI, vector<double> &faradays,  double nu_0, double alpha, double epsilon) ;
  vector<complex<double> > performFarraday(vector<complex<double> > &peak, vector<double> &lambC, vector<double> &faradays,  double nu_0, double alpha, double epsilon) ;
}  // end of the namespace RM


#endif
