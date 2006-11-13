#ifndef MATRIX_H
#define MATRIX_H
/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: matrix.h,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  16/01/02  created
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


class floatMatrix;
class floatVector;

std::ostream& operator<<(std::ostream &,const floatMatrix &);
std::istream& operator>>(std::istream &,floatMatrix &);

class floatMatrix {

friend std::ostream& operator<<(std::ostream &,const floatMatrix &);
friend std::istream& operator>>(std::istream &,floatMatrix &);

public:

  floatMatrix(int rows,int columns);
  floatMatrix(const floatMatrix &M);
  floatMatrix(int rows,int columns,float *buffer);
  ~floatMatrix();

  unsigned int rows()            const { return mRows;    }
  unsigned int columns()         const { return mColumns; }
  float *data()                  const { return mMatrix;  }

  int memSize()                  const { return sizeof(float)*mRows*mColumns; }
  floatMatrix &Zero();

  float& elem(unsigned int x,unsigned int y)       const { 
    if (y>=mColumns || x>=mRows) {
      //      printf("Wrong element (%d,%d)\n",x,y);
      return mMatrix[0];
    }
    return  mMatrix[y*mRows+x];   }
  float& operator()(unsigned int x,unsigned int y) const { 
    if (y>=mColumns || x>=mRows) {
      //      printf("Wrong element (%d,%d)\n",x,y);
      return mMatrix[0];
    }
    return  mMatrix[y*mRows+x];   
  }

  floatMatrix& operator=(const floatMatrix & );
  floatMatrix& multiply(const floatMatrix &,const floatMatrix &);
  floatMatrix& transpose(const floatMatrix &);
  floatMatrix& pinv(floatMatrix &);

protected:

  void pinv2(floatMatrix &,floatVector &,floatMatrix &);
  unsigned int  mColumns;
  unsigned int  mRows;
  float        *mMatrix;

private:

  bool Create(int rows,int columns);
  void Destroy();
  bool mLinked;

};


class doubleMatrix;
class doubleVector;

std::ostream& operator<<(std::ostream &,const doubleMatrix &);
std::istream& operator>>(std::istream &,doubleMatrix &);

class doubleMatrix {

friend std::ostream& operator<<(std::ostream &,const doubleMatrix &);
friend std::istream& operator>>(std::istream &,doubleMatrix &);

public:

  doubleMatrix(int rows,int columns);
  doubleMatrix(const doubleMatrix &M);
  doubleMatrix(int rows,int columns,float *buffer);
  ~doubleMatrix();

  unsigned int rows()            const { return mRows;    }
  unsigned int columns()         const { return mColumns; }
  float *data()                  const { return mMatrix;  }

  int memSize()                  const { return sizeof(float)*mRows*mColumns; }
  doubleMatrix &Zero();

  float& elem(unsigned int x,unsigned int y)       const { 
    if (y>=mColumns || x>=mRows) {
      //      printf("Wrong element (%d,%d)\n",x,y);
      return mMatrix[0];
    }
    return  mMatrix[y*mRows+x];   }
  float& operator()(unsigned int x,unsigned int y) const { 
    if (y>=mColumns || x>=mRows) {
      //      printf("Wrong element (%d,%d)\n",x,y);
      return mMatrix[0];
    }
    return  mMatrix[y*mRows+x];   
  }

  doubleMatrix& operator=(const doubleMatrix & );
  doubleMatrix& multiply(const doubleMatrix &,const doubleMatrix &);
  doubleMatrix& transpose(const doubleMatrix &);
  doubleMatrix& pinv(doubleMatrix &);

protected:

  void pinv2(doubleMatrix &,doubleVector &,doubleMatrix &);
  unsigned int  mColumns;
  unsigned int  mRows;
  float        *mMatrix;

private:

  bool Create(int rows,int columns);
  void Destroy();
  bool mLinked;

};


#endif /*!MATRIX_H*/
