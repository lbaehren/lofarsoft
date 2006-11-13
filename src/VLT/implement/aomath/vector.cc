/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: vector.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  16/01/02  created 
* efedrigo  20/01/03  added copyRow and copyColumn
*/

/************************************************************************
*   NAME
*   
* 
*   SYNOPSIS
*
*   
*   PARENT CLASS
*
* 
*   DESCRIPTION
*
*
*   PUBLIC METHODS
*
*
*   PUBLIC DATA MEMBERS
*
*
*   PROTECTED METHODS
*
*
*   PROTECTED DATA MEMBERS
*
*
*   PRIVATE METHODS
*
*
*   PRIVATE DATA MEMBERS
*
*
*   FILES
*
*   ENVIRONMENT
*
*   COMMANDS
*
*   RETURN VALUES
*
*   CAUTIONS 
*
*   EXAMPLES
*
*   SEE ALSO
*
*   BUGS   
* 
*------------------------------------------------------------------------
*/

#define _POSIX_SOURCE 1

static char *rcsId="@(#) $Id: vector.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include <aomath/vector.h>
#include <aomath/memory.h>
#include <math.h>

// Memory pools

extern memoryPool VectorMemoryPool;

/********************************************************************/
/********************************************************************/
/*                                                                  */
/*                                                                  */
/* FLOAT Vector implementation                                      */
/*                                                                  */
/*                                                                  */
/********************************************************************/
/********************************************************************/

//===================================================
// Constructors / Destructors
//===================================================

floatVector::floatVector(int elements)
{
  if (Create(elements)) {
    memset(mVector,0,memSize());
  } else {
    /* this printf must be removed */
    printf("Error creating the vector %dx\n",elements);
  }
}

floatVector::floatVector(int elements,float *buffer)
{
  mVector = buffer;
  mElements = elements;
  mLinked = true;
}

bool floatVector::Create(int elements)
{
  mElements = elements; // set first for memSize()

  mVector = (float*)VectorMemoryPool.malloc(memSize());
  if (mVector) {
    
    mLinked = false;
    return true;

  } else {

    printf("Problem allocating the matrix\n");
    mElements = 0;
    return false;

  }
}

floatVector::floatVector(const floatVector &V)
{
  if (Create(V.mElements)) {
    memcpy(mVector,V.mVector,memSize());
  }
}

floatVector::~floatVector()
{
  Destroy();
}

void floatVector::Destroy()
{
  if (!mLinked) {
    if (mVector) VectorMemoryPool.free(mVector);
  }
  mVector = 0;
  mElements = 0;

}

//===================================================
// I/O operators
//===================================================

std::ostream& operator<<(std::ostream &s,const floatVector &V)
{
  for(unsigned int x=0;x<V.mElements;x++) {    
     s << V.elem(x) << " ";		
  }
  
  return s;
}


std::istream& operator>>(std::istream &s,floatVector &V)
{
  V.Destroy();
  
  // Sorry, I need the dimension first
  int elem=0;
  double d;

  while(s.good()) {
    char c;
    
    while (s.get(c),s.good()) { // skip spaces
      if (c=='\n') break;

      if (!isspace(c)) {
        s.putback(c);
        break;
      }
    }

    if (c=='\n') { // eol

      break;

    } else {

      s >> d;
      elem++;

    }
  }

  // rewind
  s.clear();
  s.seekg(0,std::ios::beg);
  if (!V.Create(elem)) {
    printf("Error creating vector\n");
    return s;
  }

  for(int x=0;x<elem;x++) {
    s >> V(x);
  }

  return s;
}

//===================================================
//===================================================

floatVector& floatVector::operator=(const floatVector &V)
{
#ifdef DEBUG
  if (mElements != V.mElements) {
    std::cerr << "Dimension mismatch" << std::endl;
    return *this;
  }
#endif

  memcpy(mVector,V.mVector,memSize());

  return *this;
}

//===================================================
//===================================================

floatVector &floatVector::Zero()
{
    memset(mVector,0,memSize());
    return *this;
}

//===================================================
// Matrix*Vector multiply
//===================================================

floatVector& floatVector::multiply(const floatMatrix &M,const floatVector &V)
{
  register unsigned int x,y;

#ifdef DEBUG
  if (M.columns() != V.mElements ||
      mElements != M.rows() ) {
    printf("floatVector::multiply(Matrix,Vector) dimension mismatch\n");
    return *this;
  }
#endif

  Zero();
  for(x=0;x<M.rows();x++) {
    for(y=0;y<M.columns();y++) {
      elem(x) += M(x,y) * V(y);
    }
  }

  return *this;
}

//===================================================
// Inner product
//===================================================

float floatVector::multiply(const floatVector &V1,const floatVector &V2)
{
  register unsigned int x;
  float res = 0;

#ifdef DEBUG
  if (V1.mElements != V2.mElements) {
    printf("floatVector::multiply(Vector,Vector) dimension mismatch\n");
    return res;
  }
#endif

  const float *tmp1 = V1.data();
  const float *tmp2 = V2.data();

  for(x=0;x<V1.mElements;x++) {
      res += *tmp1++ * *tmp2++;
  }

  return res;
}

floatVector &floatVector::copyRow(const floatMatrix &M,unsigned int row)
{
  register unsigned int x;

#ifdef DEBUG
  if (M.columns() != mElements) {
    printf("floatVector::copyRow(Matrix,int) dimension mismatch\n");
    return *this;
  }
  if (row >= M.rows()) {
    printf("floatVector::copyRow(Matrix,int) requested row %d not available (%d)\n",row,M.rows());
    return *this;
  }
#endif
    
  for(x=0;x<mElements;x++) {
      elem(x) = M(row,x);
  }
  return *this;

}

floatVector &floatVector::copyColumn(const floatMatrix &M,unsigned int column)
{
  register unsigned int x;

#ifdef DEBUG
  if (M.rows() != mElements) {
    printf("floatVector::copyColumns(Matrix,int) dimension mismatch\n");
    return *this;
  }
  if (column >= M.columns()) {
    printf("floatVector::copyColumn(Matrix,int) requested column %d not available (%d)\n",column,M.columns());
    return *this;
  }
#endif
    
  for(x=0;x<mElements;x++) {
      elem(x) = M(x,column);
  }
  return *this;

}

float floatVector::mean()
{
    float sum = 0;
    for(unsigned int x=0;x<mElements;x++) {
      sum += elem(x);
    }
    return sum/(float)mElements;
}

float floatVector::var()
{
    float sum = 0;
    float square = 0;
    for(unsigned int x=0;x<mElements;x++) {
      sum += elem(x);
      square += elem(x)*elem(x);
    }
    float mean = sum/(float)mElements;
    return square-mean*mean/(float)mElements;
}

float floatVector::std()
{
    return sqrt(var()/(float)mElements);
}

/********************************************************************/
/********************************************************************/
/*                                                                  */
/*                                                                  */
/* DOUBLE Vector implementation                                     */
/*                                                                  */
/*                                                                  */
/********************************************************************/
/********************************************************************/

//===================================================
// Constructors / Destructors
//===================================================

doubleVector::doubleVector(int elements)
{
  if (Create(elements)) {
    memset(mVector,0,memSize());
  } else {
    /* this printf must be removed */
    printf("Error creating the vector %dx\n",elements);
  }
}

doubleVector::doubleVector(int elements,double *buffer)
{
  mVector = buffer;
  mElements = elements;
  mLinked = true;
}

bool doubleVector::Create(int elements)
{
  mElements = elements; // set first for memSize()

  mVector = (double*)VectorMemoryPool.malloc(memSize());
  if (mVector) {
    
    mLinked = false;
    return true;

  } else {

    printf("Problem allocating the matrix\n");
    mElements = 0;
    return false;

  }
}

doubleVector::doubleVector(const doubleVector &V)
{
  if (Create(V.mElements)) {
    memcpy(mVector,V.mVector,memSize());
  }
}

doubleVector::~doubleVector()
{
  Destroy();
}

void doubleVector::Destroy()
{
  if (!mLinked) {
    if (mVector) VectorMemoryPool.free(mVector);
  }
  mVector = 0;
  mElements = 0;

}

//===================================================
// I/O operators
//===================================================

std::ostream& operator<<(std::ostream &s,const doubleVector &V)
{
  for(unsigned int x=0;x<V.mElements;x++) {    
     s << V.elem(x) << " ";		
  }
  
  return s;
}


std::istream& operator>>(std::istream &s,doubleVector &V)
{
  V.Destroy();
  
  // Sorry, I need the dimension first
  int elem=0;
  double d;

  while(s.good()) {
    char c;
    
    while (s.get(c),s.good()) { // skip spaces
      if (c=='\n') break;

      if (!isspace(c)) {
        s.putback(c);
        break;
      }
    }

    if (c=='\n') { // eol

      break;

    } else {

      s >> d;
      elem++;

    }
  }

  // rewind
  s.clear();
  s.seekg(0,std::ios::beg);
  if (!V.Create(elem)) {
    printf("Error creating vector\n");
    return s;
  }

  for(int x=0;x<elem;x++) {
    s >> V(x);
  }

  return s;
}

//===================================================
//===================================================

doubleVector& doubleVector::operator=(const doubleVector &V)
{
#ifdef DEBUG
  if (mElements != V.mElements) {
    std::cerr << "Dimension mismatch" << std::endl;
    return *this;
  }
#endif

  memcpy(mVector,V.mVector,memSize());

  return *this;
}

//===================================================
//===================================================

doubleVector &doubleVector::Zero()
{
    memset(mVector,0,memSize());
    return *this;
}

//===================================================
// Matrix*Vector multiply
//===================================================

doubleVector& doubleVector::multiply(const doubleMatrix &M,const doubleVector &V)
{
  register unsigned int x,y;

#ifdef DEBUG
  if (M.columns() != V.mElements ||
      mElements != M.rows() ) {
    printf("doubleVector::multiply(Matrix,Vector) dimension mismatch\n");
    return *this;
  }
#endif

  Zero();
  for(x=0;x<M.rows();x++) {
    for(y=0;y<M.columns();y++) {
      elem(x) += M(x,y) * V(y);
    }
  }

  return *this;
}

//===================================================
// Inner product
//===================================================

double doubleVector::multiply(const doubleVector &V1,const doubleVector &V2)
{
  register unsigned int x;
  double res = 0;

#ifdef DEBUG
  if (V1.mElements != V2.mElements) {
    printf("doubleVector::multiply(Vector,Vector) dimension mismatch\n");
    return res;
  }
#endif

  const double *tmp1 = V1.data();
  const double *tmp2 = V2.data();

  for(x=0;x<V1.mElements;x++) {
      res += *tmp1++ * *tmp2++;
  }

  return res;
}

doubleVector &doubleVector::copyRow(const doubleMatrix &M,unsigned int row)
{
  register unsigned int x;

#ifdef DEBUG
  if (M.columns() != mElements) {
    printf("doubleVector::copyRow(Matrix,int) dimension mismatch\n");
    return *this;
  }
  if (row >= M.rows()) {
    printf("doubleVector::copyRow(Matrix,int) requested row %d not available (%d)\n",row,M.rows());
    return *this;
  }
#endif
    
  for(x=0;x<mElements;x++) {
      elem(x) = M(row,x);
  }
  return *this;

}

doubleVector &doubleVector::copyColumn(const doubleMatrix &M,unsigned int column)
{
  register unsigned int x;

#ifdef DEBUG
  if (M.rows() != mElements) {
    printf("doubleVector::copyColumns(Matrix,int) dimension mismatch\n");
    return *this;
  }
  if (column >= M.columns()) {
    printf("doubleVector::copyColumn(Matrix,int) requested column %d not available (%d)\n",column,M.columns());
    return *this;
  }
#endif
    
  for(x=0;x<mElements;x++) {
      elem(x) = M(x,column);
  }
  return *this;

}

double doubleVector::mean()
{
    double sum = 0;
    for(unsigned int x=0;x<mElements;x++) {
      sum += elem(x);
    }
    return sum/(double)mElements;
}

double doubleVector::var()
{
    double sum = 0;
    double square = 0;
    for(unsigned int x=0;x<mElements;x++) {
      sum += elem(x);
      square += elem(x)*elem(x);
    }
    double mean = sum/(double)mElements;
    return square-mean*mean/(double)mElements;
}

double doubleVector::std()
{
    return sqrt(var()/(double)mElements);
}
/*___oOo___*/
