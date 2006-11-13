#ifndef VECTOR_H
#define VECTOR_H
/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: vector.h,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  16/01/02  created
* efedrigo  14/11/02  added floatVector data type
* efedrigo  20/01/03  added copyRow and copyColumn
*/

/************************************************************************
 *
 *----------------------------------------------------------------------
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#ifdef HAVE_VXWORKS
# include <iostream.h>
#else
# include <iostream>
#endif

#include <aomath/matrix.h>

class floatVector;

std::ostream& operator<<(std::ostream &,const floatVector &);
std::istream& operator>>(std::istream &,floatVector &);

class floatVector {

friend std::ostream& operator<<(std::ostream &,const floatVector &);
friend std::istream& operator>>(std::istream &,floatVector &);

public:

  floatVector(int elements);
  floatVector(const floatVector &V);
  floatVector(int elements,float *buffer);
  ~floatVector();

  unsigned int elements()        const { return mElements;    }
  unsigned int length()          const { return mElements;    }
  float *data()                  const { return mVector;  }
  int memSize()                  const { return sizeof(float)*mElements; }
  floatVector &Zero();

  float& elem(unsigned int x)       const { 
#ifdef DEBUG
    if (x>=mElements) {
      cerr << "Wrong element (" << x << ")" << endl;
      return mVector[0];
    }
#endif
    return  mVector[x];   }
  float& operator()(unsigned int x) const { 
#ifdef DEBUG
    if (x>=mElements) {
      cerr << "Wrong element (" << x << ")" << endl;
      return mVector[0];
    }
#endif
    return  mVector[x];   
  }

  floatVector& operator=(const floatVector & );
  floatVector& multiply(const floatMatrix &,const floatVector &);
  floatVector& copyRow(const floatMatrix &,unsigned int row);
  floatVector& copyColumn(const floatMatrix &,unsigned int column);
  float multiply(const floatVector &,const floatVector &);
  float mean();
  float std();
  float var();
  
protected:

  unsigned int  mElements;
  float        *mVector;

private:

  bool Create(int elements);
  void Destroy();
  bool mLinked;

};

class doubleVector;

std::ostream& operator<<(std::ostream &,const doubleVector &);
std::istream& operator>>(std::istream &,doubleVector &);

class doubleVector {

friend std::ostream& operator<<(std::ostream &,const doubleVector &);
friend std::istream& operator>>(std::istream &,doubleVector &);

public:

  doubleVector(int elements);
  doubleVector(const doubleVector &V);
  doubleVector(int elements,double *buffer);
  ~doubleVector();

  unsigned int elements()        const { return mElements;    }
  unsigned int length()          const { return mElements;    }
  double *data()                  const { return mVector;  }
  int memSize()                  const { return sizeof(double)*mElements; }
  doubleVector &Zero();

  double& elem(unsigned int x)       const { 
#ifdef DEBUG
    if (x>=mElements) {
      cerr << "Wrong element (" << x << ")" << endl;
      return mVector[0];
    }
#endif
    return  mVector[x];   }
  double& operator()(unsigned int x) const { 
#ifdef DEBUG
    if (x>=mElements) {
      cerr << "Wrong element (" << x << ")" << endl;
      return mVector[0];
    }
#endif
    return  mVector[x];   
  }

  doubleVector& operator=(const doubleVector & );
  doubleVector& multiply(const doubleMatrix &,const doubleVector &);
  doubleVector& copyRow(const doubleMatrix &,unsigned int row);
  doubleVector& copyColumn(const doubleMatrix &,unsigned int column);
  double multiply(const doubleVector &,const doubleVector &);
  double mean();
  double std();
  double var();
  
protected:

  unsigned int  mElements;
  double        *mVector;

private:

  bool Create(int elements);
  void Destroy();
  bool mLinked;

};

#endif /*!VECTOR_H*/
