/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: matrix.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  16/01/02  created 
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

static char *rcsId="@(#) $Id: matrix.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

#include <aomath/matrix.h>
#include <aomath/vector.h>
#include <aomath/memory.h>
#include <aomath/svd.h>

// Memory pools

extern memoryPool MatrixMemoryPool;

/********************************************************************/
/********************************************************************/
/*                                                                  */
/*                                                                  */
/* Matrix implementation                                            */
/*                                                                  */
/*                                                                  */
/********************************************************************/
/********************************************************************/

//===================================================
// Constructors / Destructors
//===================================================

floatMatrix::floatMatrix(int rows,int columns)
{
  if (Create(rows,columns)) {
    memset(mMatrix,0,memSize());
  } else {
    /* this printf must be removed */
    printf("Error creating the matrix %dx%d\n",rows,columns);
  }
}

floatMatrix::floatMatrix(int rows,int columns,float *buffer)
{
  mMatrix = buffer;
  mColumns = columns;
  mRows = rows;
  mLinked = true;
}

bool floatMatrix::Create(int rows,int columns)
{
  mColumns = columns; // set first for memSize()
  mRows = rows;

  mMatrix = (float*)MatrixMemoryPool.malloc(memSize());
  if (mMatrix) {
    
    mLinked = false;
    return true;

  } else {

    printf("Problem allocating the matrix\n");
    mColumns = 0;
    mRows = 0;
    return false;

  }
}

floatMatrix::floatMatrix(const floatMatrix &M)
{
  if (Create(M.mRows,M.mColumns)) {
    memcpy(mMatrix,M.mMatrix,memSize());
  }
}

floatMatrix::~floatMatrix()
{
  Destroy();
}

void floatMatrix::Destroy()
{
  if (!mLinked) {
    if (mMatrix) MatrixMemoryPool.free(mMatrix);
  }
  mMatrix = 0;
  mRows = 0;
  mColumns = 0;

}

floatMatrix &floatMatrix::Zero()
{
    memset(mMatrix,0,memSize());
    return *this;
}

//===================================================
// I/O operators
//===================================================

std::ostream& operator<<(std::ostream &s,const floatMatrix &M)
{
  for(unsigned int x=0;x<M.mRows;x++) {    
    for(unsigned int y=0;y<M.mColumns;y++) {
      s << M.elem(x,y) << " ";		
    }
    s << std::endl;    
  }
  
  return s;
}


std::istream& operator>>(std::istream &s,floatMatrix &M)
{
  M.Destroy();
  
  // Sorry, I need the dimension first
  int x=0,y,row=0,col=0;
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
      row++;
      if (col==0) {
        col = x;
      } else {
        if (col!=x) {
          
          if (x!=0) {
            printf("Error reading matrix. Expected %d columns, found %d\n",
                   col,x);
            return s;
          } else {
            row--;
          }
        }
      }

      x=0;

    } else {

 
      s >> d;
      x++;

    }
  }

  // rewind
  s.clear();
  s.seekg(0,std::ios::beg);
  if (!M.Create(row,col)) {
    printf("Error creating matrix\n");
    return s;
  }

  for(x=0;x<row;x++)
    for(y=0;y<col;y++) {
      s >> M(x,y);
    }

  return s;
}

//===================================================
// Assignment
//===================================================

floatMatrix& floatMatrix::operator=(const floatMatrix &M)
{
  if (mRows != M.mRows || mColumns != M.mColumns) {
    std::cerr << "Dimension mismatch" << std::endl;
    return *this;
  }

  memcpy(mMatrix,M.mMatrix,memSize());

  return *this;
}

//===================================================
// Mathematical operations
//===================================================

floatMatrix& floatMatrix::transpose(const floatMatrix &M)
{
  register unsigned int x,y;

#ifdef DEBUG
  if (mRows != M.mColumns || mColumns != M.mRows) {
    std::cerr << "Dimension mismatch" << endl;
    return *this;
  }
#endif

  for(x=0;x<mRows;x++)
    for(y=0;y<mColumns;y++)
      elem(x,y)=M(y,x);

  return *this;
}

/* 
 * Matrices are stored by column!
 */

floatMatrix& floatMatrix::multiply(const floatMatrix &M1,const floatMatrix &M2)
{
  register unsigned int x,y,z;

#ifdef DEBUG
  if (M1.mColumns != M2.mRows) {
    printf("floatMatrix::multiply(Matrix,Matrix) dimension mismatch\n");
    return *this;
  }
#endif

  for(x=0;x<mRows;x++) {
    for(y=0;y<mColumns;y++) {
      elem(x,y)=0;
      for(z=0;z<M1.mColumns;z++) {
	elem(x,y) += M1(x,z) * M2(z,y);
      }
    }
  }

  return *this;
}



void floatMatrix::pinv2(floatMatrix &U,floatVector &S,floatMatrix &V)
{

    svd(U,S,V);

    // Invert
    for(unsigned int x=0;x<S.length();x++) {
      if (S(x)!=0) S(x)=1.0/S(x);
    }

    // ===================================================
    // multiply V with a matrix which has S on the diagonal 
    // and 0 elsewhere 
    // ===================================================

    for(unsigned int x=0;x<V.rows();x++) {
      for(unsigned int y=0;y<V.columns();y++) {
	V(x,y)=V(x,y)*S(y);
      }
    }
    
}

floatMatrix& floatMatrix::pinv(floatMatrix &in)
{
    if (in.rows() != columns() ||
	in.columns() != rows()) {
      std::cerr << "Wrong dimensions" << std::endl;
      return *this;
    }
    
    if (in.rows()<in.columns()) {
      floatMatrix U(in.columns(),in.rows());
      U.transpose(in);
      floatVector S(in.rows());
      floatMatrix V(in.rows(),in.rows());
      floatMatrix V2(in.rows(),in.rows());

      pinv2(U,S,V);

      V2.transpose(V);
      multiply(U,V2);

    } else {
      floatMatrix U(in);
      floatMatrix UT(in.columns(),in.rows());
      floatVector S(in.columns());
      floatMatrix V(in.columns(),in.columns());

      pinv2(U,S,V);

      UT.transpose(U);
      multiply(V,UT);
    }

    return *this;
}


/*___oOo___*/
