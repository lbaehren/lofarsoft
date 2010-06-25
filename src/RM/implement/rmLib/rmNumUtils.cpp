//  	ITPP Test program and Wiener Filter test implementation
//
//	File:			wienerfilter.cpp
//	Author:			Sven Duscha (sduscha@mpa-garching.mpg.de)
//	Date:			09-06-2009
//	Last change:		18-10-2009

/* Standard header files */
#include <complex>
#include <vector>
#include <stack>
#include <fstream>
#include <algorithm>			// for minimum element in vector
#include <limits>			// limit for S infinite
#include <iomanip>			// to set precision in cout
#include "rmNumUtils.h"
/* RM header files */

using namespace std;

namespace RM {



/*! procedure finds gaps inside the ordered double valued vector 
    \param freqs ordered double valued vector to find the gaps in */
void findGaps(vector<double> &freqs,vector<uint> &result) {
  vector<double> mean(2) ;
  double epsilon = 1+5E-8 ;
  calcMeanVarDist(freqs,mean) ;
  double meanDist = mean[0] ;
  double varDist = mean[1] ;
  result.push_back(0) ; // always put the first element into the gaps 
  /* loop over all elements of the given vector, to look at the distances
    and to find the gaps, where the positions are stored into result */
  for (uint i=0; i<freqs.size()-1; i++) {
    /* a gap is detected for intervallength more than mean length plus 3 times 
       the variance of the intervallenths */
    if ((freqs[i+1]-freqs[i]) > (epsilon*meanDist+3*varDist)) {
      result.push_back(i+1) ; 
    }
  }
  result.push_back(freqs.size()) ; // always put the first element into the gaps 
}


/*! Procedure to calculate the mean separation and varinace of the  
    separations of the element of an ordered  real valued vector 
    \param freqs ordered real valued vector (double precission ) 
    \param result vector conatains the mean [0] and the variance[1] */
void calcMeanVarDist(vector<double> &freqs, vector<double> &result) {
  double mean = 0 ;
  double var = 0 ;
  int num = freqs.size()-1 ;
  for (int i=0; i<num ; i++) {
    double diff = freqs[i+1]-freqs[i] ;
    mean = mean+diff ;
    var = var+diff*diff ;
  }
  result[0] = mean/num ;
  result[1] = sqrt((var-mean*mean/num)/num);
}



complex<double> cmat::prod(vector<complex<double> > vek1, vector<complex<double> > vek2) {
  uint s1 = vek1.size() ;
  uint s2 = vek2.size() ;
  complex<double> erg = 0 ;
  if (s1 != s2) {
   throw "imcomplatible dimensions \n"; 
  }
  else {
    for (uint i=0; i<s1; i++) {
      erg = erg + vek1[i]*vek2[i] ;
    }
  }
  return erg ;
}

cmat cmat::operator* (mat &matr) {
  uint nrow = rows() ;
  uint ncol = matr.cols() ;
  mat temp = matr.H() ;
  cmat erg(nrow,ncol) ;	
  for (uint i=0; i<nrow;i++) {
      vector<complex<double> > row = data[i] ;
      for (uint j=0;j<ncol;j++) {
        vector<double> col = temp.data[j] ;
        erg.set(i,j,mult(row,col)) ;
	
      }
  }   
  return erg ;
}

/*! procedure writing out the martix in rectangular way 
    \param fileName Name of the file to write the data into */
void cmat::write(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<complex<double> > aus = data[i] ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile << aus[j] << " " ;
    }
    outfile << endl ;
  }
  outfile.close();

}
/*! writing out absolute values of a Matrix with row and collumn index.
    Each value gets a own line, one line skip for a new row 
   \param fileName Name of the file to write the data into */
void cmat::absOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<complex<double> > aus = data[i] ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile << i << "\t" << j << "\t" << abs(aus[j]) << endl  ;
    }
    outfile << endl ;
  }
  outfile.close();

}
cmat cmat::inv() {
  cmat zw ;
  zw.data = data ;
  return zw.GaussEle() ;
}

complex<double> cmat::mult(vector<complex<double> > v1,vector<complex<double> > v2) {
  complex<double> erg(0,0) ;
  for (uint i=0; i<v1.size(); i++) {
    erg = erg+v1[i]*v2[i] ;
  }
  return erg ;
}

complex<double> cmat::mult(vector<complex<double> > v1,vector<double> v2) {
  complex<double> erg(0,0) ;
  for (uint i=0; i<v1.size(); i++) {
    erg = erg+v1[i]*v2[i] ;
  }
  return erg ;
}

double cmat::maxi() {
  double erg = 0 ;
  for (uint i=0; i<rows();i++) {
    vector<complex<double> > row = data[i] ;
    for (uint j=0; j< cols() ; j++) {
      if (erg < abs(row[j])) erg = abs(row[j]) ;
    }
  } 
  return erg;
}

void cmat::swapCols(uint ind1, uint ind2) {
  if (ind1 != ind2) {
    if(max(ind1,ind2)>rows()) {
	 throw "index out of Range in swapCols\n" ;
    }
    for (uint i=0; i<rows(); i++) {
	swap(data[i][ind1],data[i][ind2]);
    }
  }
}

/*! Procedure performs an complete elimination schema including the following steps:
    1. Forward elimination
    2. backward elimination
    3. normalisation */

cmat cmat::GaussEle() {
  if (rows() != cols()) {
    throw "no Elimination implemented for non quadratic matrixes\n" ;
  }
  stack<permEntry> perm ;
  cmat unity(rows(),rows()) ;
  unity.fillUnit() ;
  /* forward elemination */
  for (uint i=0; i<rows()-1; i++) {
    uint ro = getMaxPivot(i) ;
    if (ro != i) {
      data[ro].swap(data[i]);
      permEntry eint(ro,i) ;
      perm.push(eint) ;
    }
    if (data[i][i] == 0.0 ) {
      cout << "index " << i << "  diagonal " << data[i][i] << endl ;
      throw "Matrix is not invertible \n";
    }
    for (uint j= i+1; j< rows(); j++ ) {
      complex<double> fak = -data[j][i]/data[i][i];
      add(j,i,fak);
      unity.add(j,i,fak);
    }
  }
  /* backward elemination */
    for (uint i=rows()-1; i>0; i--) {    
      for (uint j= 0; j< i; j++ ) {
        complex<double> fak = -data[j][i]/data[i][i];
        add(j,i,fak);
        unity.add(j,i,fak);
      }
    }
    /* normalisation */
    for (uint i=0; i<rows(); i++) {
	complex<double> fak = 1.0/data[i][i] ;
	unity.mult(fak,i) ;
    }
    /* swap collumns of result */
    while(!perm.empty()) {
	    
      permEntry per = perm.top() ;
      perm.pop() ;
      unity.swapCols(per.pos1,per.pos2) ;
    }
    return unity;
}

void cmat::mult(complex<double> fak, uint row) {
  for (uint i=0; i<data.size(); i++) {
    data[row][i] = fak*data[row][i] ;
  }
}

void mat::mult(double fak, uint row) {
  for (uint i=0; i<data.size(); i++) {
    data[row][i] = fak*data[row][i] ;
  }
}

void mat::swapCols(uint ind1, uint ind2) {
  if (ind1 != ind2) {
    if(max(ind1,ind2)>rows()) {
	 throw "index out of Range in swapCols\n" ;
    }
    for (uint i=0; i<rows(); i++) {
	swap(data[i][ind1],data[i][ind2]);
    }
  }
}

/*! writing out absolute values of a Matrix with row and collumn index.
    Each value gets a own line, one line skip for a new row 
   \param fileName Name of the file to write the data into */
void mat::absOut(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<double> aus = data[i] ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile << i << "\t" << j << "\t" << fabs(aus[j]) << endl  ;
    }
    outfile << endl ;
  }
  outfile.close();

}

void cmat::fillUnit() {
  uint nCol = cols() ;
  uint nRow = rows() ;
  if (nRow != nCol) {
    throw "no unit for non quadratic matrixes\n" ;
  }
  for (uint i=0; i< nRow ; i++) {
     for (uint j=0; j<nRow; j++ ) {
	 data[i][j] =(i == j) ? 1 : 0; // kronecer delta
     }
  }
}

void cmat::add(uint addto, uint addfrom, complex<double> fak) {
  for (uint i=0; i<data.size(); i++) {
    data[addto][i] = data[addto][i]+fak*data[addfrom][i] ;
  }	
}

void mat::add(uint addto, uint addfrom, double fak) {
  for (uint i=0; i<data.size(); i++) {
    data[addto][i] = data[addto][i]+fak*data[addfrom][i] ;
  }	
}

cmat cmat::H() {
  cmat erg(cols(),rows()) ;
  for (uint i =0; i<rows(); i++) {
    vector<complex<double> > u = data[i] ;
    for (uint j=0; j<cols(); j++) {
       erg.set(j,i,conj(u[j])) ;
    }
  }
  return erg ;
}

cmat cmat::t() {
  cmat erg(cols(),rows()) ;
  for (uint i =0; i<rows(); i++) {
    vector<complex<double> > u = data[i] ;
    for (uint j=0; j<cols(); j++) {
       erg.set(j,i,u[j]) ;
    }
  }
  return erg ;
}

complex<double> cmat::det() {
  GaussEle() ;
  complex<double> erg = 1.0 ;
  for (uint i=0; i<rows();i++) {
     erg = erg*get(i,i) ;
  }
  return erg ;
}

mat mat::inv() {
  mat zw ;
  zw.data = data ;
  return zw.GaussEle() ;
}

double mat::det() {
  GaussEle() ;
  double erg = 1.0 ;
  for (uint i=0; i<rows();i++) {
     erg = erg*get(i,i) ;
  }
  return erg ;
}

/*! Procedure performs an complete elimination schema including the following steps:
    1. Forward elimination
    2. backward elimination
    3. normalisation */
mat mat::GaussEle() {
  if (rows() != cols()) {
    throw "no Elimination implemented for non quadratic matrixes\n" ;
  }
  stack<permEntry> perm ;
  mat unity(rows(),rows()) ;
  unity.fillUnit() ;
  /* forward elemination */
  for (uint i=0; i<rows()-1; i++) {
    uint ro = getMaxPivot(i) ;
    if (ro != i) {
	data[ro].swap(data[i]);
	permEntry eint(ro,i) ;
	perm.push(eint) ;
    }    
    if (data[i][i] == 0 ) {
      throw "Matrix is not invertible \n";
    }
    for (uint j= i+1; j< rows(); j++ ) {
      double fak = -data[j][i]/data[i][i];
      add(j,i,fak);
      unity.add(j,i,fak);
    }
  }
  /* backward elemination */
    for (uint i=rows()-1; i>0; i--) {    
      for (uint j= 0; j< i; j++ ) {
        double fak = -data[j][i]/data[i][i];
        add(j,i,fak);
        unity.add(j,i,fak);
      }
    }
    /* noralisation */
    for (uint i=0; i<rows(); i++) {
	double fak = 1.0/data[i][i] ;
	unity.mult(fak,i) ;
    }
    /* swap collumns of result */
    while(!perm.empty()) {
      permEntry per = perm.top() ;
      perm.pop() ;
      unity.swapCols(per.pos1,per.pos2) ;
    }
  return unity;
}

void mat::write(string fileName) {
  ofstream outfile (fileName.c_str());
  /* loop over all rows of the current signal covariance matrix */
  for (uint i=0; i<rows(); i++) {
    vector<double> aus = data[i] ;
    for (uint j=0; j<aus.size() ; j++) {
      outfile << aus[j] << " " ;
    }
    outfile << endl ;
  }
  outfile.close();

}

void mat::fillUnit() {
  uint nCol = cols() ;
  uint nRow = rows() ;
  if (nRow != nCol) {
    throw "no unit for non quadratic matrixes\n" ;
  }
  for (uint i=0; i< nRow ; i++) {
     for (uint j=0; j<nRow; j++ ) {
	 data[i][j] =(i == j) ? 1 : 0; // kronecer delta
     }
  }
}

double mat::maxi() {
  double erg = 0 ;
  for (uint i=0; i<rows();i++) {
    vector<double>  row = data[i] ;
    for (uint j=0; j< cols() ; j++) {
      if (erg < fabs(row[j])) erg = fabs(row[j]) ;
    }
  } 
  return erg;
}

mat mat::H() {
  mat erg(cols(),rows()) ;
  for (uint i =0; i<rows(); i++) {
    vector<double>  u = data[i] ;
    for (uint j=0; j<cols(); j++) {
       erg.set(j,i,u[j]) ;
    }
  }
  return erg ;
}

double mat::prod(vector<double> vek1, vector<double> vek2) {
  uint s1 = vek1.size() ;
  uint s2 = vek2.size() ;
  double erg = 0 ;
  if (s1 != s2) {
   throw "imcomplatible dimensions \n"; 
  }
  else {
    for (uint i=0; i<s1; i++) {
      erg = erg + vek1[i]*vek2[i] ;
    }
  }
  return erg ;
}

double mat::getPivot(vector<double> &vek, uint start) {
  double erg = fabs(vek[start]) ;
  
  erg = erg/abs(vek,start) ;
  return erg ;
}

uint mat::getMaxPivot(uint start) {
  double pivot = 0 ;
  uint erg = start ;
  for (uint i=start; i<data.size(); i++) {
    double test = getPivot(data[i],start) ;
    if (test>pivot) {
	pivot = test ;
	erg = i ;
    }
  }
  return erg ;
}

uint cmat::getMaxPivot(uint start) {
  double pivot = 0 ;
  uint erg = start ;  
  for (uint i=start; i<data.size(); i++) {
    double test = getPivot(data[i],start) ;
    if (test>pivot) {
	pivot = test ;
	erg = i ;
    }
  }
  return erg ;
}

double mat::abs(vector<double> vek, uint anf) {
  double erg = 0 ;
  for (uint i=anf; i<vek.size(); i++) {
    erg = erg + vek[i]*vek[i] ;
  }
  return sqrt(erg) ;
}

double cmat::abs(vector<complex<double> > vek, uint anf) {
  double erg = 0 ;
  for (uint i=anf; i<vek.size(); i++) {
    erg = erg + real(vek[i]*conj(vek[i])) ;
  }
  return sqrt(erg) ;
}

double cmat::getPivot(vector<complex<double> > &vek, uint start) {
  double erg = abs(vek[start]) ;
  erg = erg/abs(vek,start) ;
  return erg ;
}

}
