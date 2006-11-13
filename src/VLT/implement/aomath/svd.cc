/*******************************************************************************
* E.S.O. - VLT project
*
* "@(#) $Id: svd.cc,v 1.2 2004/05/10 15:41:32 loose Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* efedrigo  14/04/00  created
* efedrigo  14/11/02  added support for floatVector for eigenvalues computation
*/

/**** SVD implementation for the Matrix class *****/

#include <aomath/aomath.h>

#ifndef MAX
# define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

static float pythag(double a,double b)
{
  float at,bt,ct;

  return (float)( (at=(float)fabs(a)) > (bt=(float)fabs(b)) ? 
	   (ct=bt/at,at*sqrt(1.0+ct*ct)) : 
	   (bt ? (ct=at/bt,bt*sqrt(1.0+ct*ct)): 0.0));
}

int svd(floatMatrix &U,   // a
	floatVector &S,   // w
	floatMatrix &V,
	int maxIterations)   // v
{

  float c,f,h,s,x,y,z;
  float anorm=0.0,g=0.0,scale=0.0;
  float *rv1;

  int flag,i,its=0,j,jj,k,t,nm;
  int m = U.rows();
  int n = U.columns();
  
  int ret = 0;

  if (m < n) {
    printf("SVDCMP: You must augment A with extra zero rows\n"); 
    return -1;
  }

  rv1 = new float[n];

  t=0;
  for (i=0;i<n;i++) {
    t=i+1;
    rv1[i]=scale*g;
    g=s=scale=0.0;
    if (i < m) {
      for (k=i;k<m;k++) {
	scale += (float)fabs(U(k,i));
      }
      if (scale) {
	for (k=i;k<m;k++) {
	  U(k,i) /= scale;
	  s += U(k,i)*U(k,i);
	}
	f=U(i,i);
	g = (float)(-SIGN(sqrt(s),f));
	h=f*g-s;
	U(i,i)=f-g;
	if (i < n) {
	  for (j=t;j<n;j++) {
	    for (s=0.0,k=i;k<m;k++) {
	      s += U(k,i)*U(k,j);
	    }
	    f=s/h;
	    for (k=i;k<m;k++) {
	      U(k,j) += f*U(k,i);
	    }
	  }
	}
	for (k=i;k<m;k++) { 
	  U(k,i) *= scale;
	}
      }
    }
    S(i)=scale*g;
    g=s=scale=0.0;
    if (i < m && i != n-1) {
      for (k=t;k<n;k++) {
	scale += (float)fabs(U(i,k));
      }
      if (scale) {
	for (k=t;k<n;k++) {
	  U(i,k) /= scale;
	  s += U(i,k)*U(i,k);
	}
	f=U(i,t);
	g = (float)(-SIGN(sqrt(s),f));
	h=f*g-s;
	U(i,t)=f-g;
	for (k=t;k<n;k++) {
	  rv1[k]=U(i,k)/h;
	}
	if (i < m) {
	  for (j=t;j<m;j++) {
	    for (s=0.0,k=t;k<n;k++) {
	      s += U(j,k)*U(i,k);
	    }
	    for (k=t;k<n;k++) {
	      U(j,k) += s*rv1[k];
	    }
	  }
	}
	for (k=t;k<n;k++) {
	  U(i,k) *= scale;
	}
      }
    }
    anorm=(float)MAX(anorm,(fabs(S(i))+fabs(rv1[i])));
  }

  for (i=n-1;i>=0;i--) {
    if (i < n-1 ) {
      if (g) {
	for (j=t;j<n;j++) {
	  V(j,i)=(U(i,j)/U(i,t))/g;
	}
	for (j=t;j<n;j++) {
	  for (s=0.0,k=t;k<n;k++) {
	    s += U(i,k)*V(k,j);
	  }
	  for (k=t;k<n;k++) {
	    V(k,j) += s*V(k,i);
	  }
	}
      }
      for (j=t;j<n;j++) {
	V(i,j)=V(j,i)=0.0;
      }
    }
    V(i,i)=1.0;
    g=rv1[i];
    t=i;
  }

  for (i=n-1;i>=0;i--) {
    t=i+1;
    g=S(i);
    if (i < n-1)
      for (j=t;j<n;j++) {
	U(i,j)=0.0;
      }
    if (g) {
      g=1.0/g;
      if (i != n-1 ) {
	for (j=t;j<n;j++) {
	  for (s=0.0,k=t;k<m;k++) {
	    s += U(k,i)*U(k,j);
	  }
	  f=(s/U(i,i))*g;
	  for (k=i;k<m;k++) {
	    U(k,j) += f*U(k,i);
	  }
	}
      }
      for (j=i;j<m;j++) {
	U(j,i) *= g;
      }
    } else {
      for (j=i;j<m;j++) {
	U(j,i)=0.0;
      }
    }
    ++U(i,i);
  }

  for (k=n-1;k>=0;k--) {
    for (its=0;its<maxIterations;its++) {
      flag=1;
      nm=k-1;
      for (t=k;t>=0;t--) {
	nm=t-1;
	if ((fabs(rv1[t])+anorm) == anorm) {

	  flag=0;
	  break;
	}
	if ((fabs(S(nm))+anorm) == anorm){

         break;
        }
      }
      if (flag) {
	c=0.0;
	s=1.0;
	for (i=t;i<=k;i++) {
	  f=s*rv1[i];
	  if (fabs(f)+anorm != anorm) {
	    g=S(i);
	    h=pythag(f,g);
	    S(i)=h;
	    h=1.0/h;
	    c=g*h;
	    s=(-f*h);
	    for (j=0;j<m;j++) {
	      y=U(j,nm);
	      z=U(j,i);
	      U(j,nm)=y*c+z*s;
	      U(j,i)=z*c-y*s;
	    }
	  }
	}
      }
      z=S(k);
      if (t == k) {
	if (z < 0.0) {
	  S(k) = (-z);
	  for (j=0;j<n;j++) {
	    V(j,k)=(-V(j,k));
	  }
	}

	break;
      }

      if (its == maxIterations){
	/* printf("svdcmp: No convergence in %d SVD iterations\n",maxIterations); */
	ret = -2;
      }
      x=S(t);
      nm=k-1;
      y=S(nm);
      g=rv1[nm];
      h=rv1[k];
      f=(float)(((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y));
      g=pythag(f,1.0);
      f=(float)((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
      c=s=1.0;
      for (j=t;j<=nm;j++) {
	i=j+1;
	g=rv1[i];
	y=S(i);
	h=s*g;
	g=c*g;
	z=pythag(f,h);
	rv1[j]=z;
	c=f/z;
	s=h/z;
	f=x*c+g*s;
	g=g*c-x*s;
	h=y*s;
	y=y*c;
	for (jj=0;jj<n;jj++) {
	  x=V(jj,j);
	  z=V(jj,i);
	  V(jj,j)=x*c+z*s;
	  V(jj,i)=z*c-x*s;
	}
	z=pythag(f,h);
	S(j)=z;
	if (z) {
	  z=1.0/z;
	  c=f*z;
	  s=h*z;
	}
	f=(c*g)+(s*y);
	x=(c*y)-(s*g);
	for (jj=0;jj<m;jj++) {
	  y=U(jj,j);
	  z=U(jj,i);
	  U(jj,j)=y*c+z*s;
	  U(jj,i)=z*c-y*s;
	}
      }
      rv1[t]=0.0;
      rv1[k]=f;
      S(k)=x;
    }
  }

  delete[] rv1;
  svdSort(U,S,V);
  return ret;
}

struct singularValues {
  float value;
  unsigned int   index;
};

static int compareSingularValues(const void *A,const void *B)
{
  singularValues *a = (singularValues*)A;
  singularValues *b = (singularValues*)B;
  if (a->value < b->value) return 1;
  if (a->value > b->value) return -1;
  return 0;
}

void svdSort(floatMatrix &U, floatVector &S, floatMatrix &V)  
{
  // number of elements in S
  unsigned int m = V.rows();
  
  singularValues *sv = new singularValues[m];
  for(unsigned int x=0;x<m;x++) {
    sv[x].value = S(x);
    sv[x].index = x;
  }
  
  qsort(sv,m,sizeof(singularValues),compareSingularValues);

  for(unsigned int y=0;y<m;y++) {

    if (sv[y].index != y) {
      float tmp;

      /* swap columns in V */
      for (unsigned int x=0;x<V.rows();x++) {
	tmp = V(x,y);
	V(x,y)=V(x,sv[y].index);
	V(x,sv[y].index)=tmp;
      }

      /* swap columns in U */
      for (unsigned int x=0;x<U.rows();x++) {
	tmp = U(x,y);
	U(x,y)=U(x,sv[y].index);
	U(x,sv[y].index)=tmp;
      }

      tmp = S(y);
      S(y)=S(sv[y].index);
      S(sv[y].index)=tmp;

      /* find references to y in the next elements and replace it with sv[y].index */
      for (unsigned int k=y;k<m;k++) {
	if (sv[k].index == y)
	  sv[k].index = sv[y].index;
      }

    }
  }

  //  /* swap S */
  //  for(unsigned int x=0;x<m;x++) {
  //    S[x] = sv[x].value;
  //  }

  delete[] sv;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// DOUBLE
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int svd(doubleMatrix &U,   // a
	doubleVector &S,   // w
	doubleMatrix &V,
	int maxIterations)   // v
{

  double c,f,h,s,x,y,z;
  double anorm=0.0,g=0.0,scale=0.0;
  double *rv1;

  int flag,i,its=0,j,jj,k,t,nm;
  int m = U.rows();
  int n = U.columns();
  
  int ret = 0;

  if (m < n) {
    printf("SVDCMP: You must augment A with extra zero rows\n"); 
    return -1;
  }

  rv1 = new double[n];

  t=0;
  for (i=0;i<n;i++) {
    t=i+1;
    rv1[i]=scale*g;
    g=s=scale=0.0;
    if (i < m) {
      for (k=i;k<m;k++) {
	scale += (double)fabs(U(k,i));
      }
      if (scale) {
	for (k=i;k<m;k++) {
	  U(k,i) /= scale;
	  s += U(k,i)*U(k,i);
	}
	f=U(i,i);
	g = (double)(-SIGN(sqrt(s),f));
	h=f*g-s;
	U(i,i)=f-g;
	if (i < n) {
	  for (j=t;j<n;j++) {
	    for (s=0.0,k=i;k<m;k++) {
	      s += U(k,i)*U(k,j);
	    }
	    f=s/h;
	    for (k=i;k<m;k++) {
	      U(k,j) += f*U(k,i);
	    }
	  }
	}
	for (k=i;k<m;k++) { 
	  U(k,i) *= scale;
	}
      }
    }
    S(i)=scale*g;
    g=s=scale=0.0;
    if (i < m && i != n-1) {
      for (k=t;k<n;k++) {
	scale += (double)fabs(U(i,k));
      }
      if (scale) {
	for (k=t;k<n;k++) {
	  U(i,k) /= scale;
	  s += U(i,k)*U(i,k);
	}
	f=U(i,t);
	g = (double)(-SIGN(sqrt(s),f));
	h=f*g-s;
	U(i,t)=f-g;
	for (k=t;k<n;k++) {
	  rv1[k]=U(i,k)/h;
	}
	if (i < m) {
	  for (j=t;j<m;j++) {
	    for (s=0.0,k=t;k<n;k++) {
	      s += U(j,k)*U(i,k);
	    }
	    for (k=t;k<n;k++) {
	      U(j,k) += s*rv1[k];
	    }
	  }
	}
	for (k=t;k<n;k++) {
	  U(i,k) *= scale;
	}
      }
    }
    anorm=(double)MAX(anorm,(fabs(S(i))+fabs(rv1[i])));
  }

  for (i=n-1;i>=0;i--) {
    if (i < n-1 ) {
      if (g) {
	for (j=t;j<n;j++) {
	  V(j,i)=(U(i,j)/U(i,t))/g;
	}
	for (j=t;j<n;j++) {
	  for (s=0.0,k=t;k<n;k++) {
	    s += U(i,k)*V(k,j);
	  }
	  for (k=t;k<n;k++) {
	    V(k,j) += s*V(k,i);
	  }
	}
      }
      for (j=t;j<n;j++) {
	V(i,j)=V(j,i)=0.0;
      }
    }
    V(i,i)=1.0;
    g=rv1[i];
    t=i;
  }

  for (i=n-1;i>=0;i--) {
    t=i+1;
    g=S(i);
    if (i < n-1)
      for (j=t;j<n;j++) {
	U(i,j)=0.0;
      }
    if (g) {
      g=1.0/g;
      if (i != n-1 ) {
	for (j=t;j<n;j++) {
	  for (s=0.0,k=t;k<m;k++) {
	    s += U(k,i)*U(k,j);
	  }
	  f=(s/U(i,i))*g;
	  for (k=i;k<m;k++) {
	    U(k,j) += f*U(k,i);
	  }
	}
      }
      for (j=i;j<m;j++) {
	U(j,i) *= g;
      }
    } else {
      for (j=i;j<m;j++) {
	U(j,i)=0.0;
      }
    }
    ++U(i,i);
  }

  for (k=n-1;k>=0;k--) {
    for (its=0;its<maxIterations;its++) {
      flag=1;
      nm=k-1;
      for (t=k;t>=0;t--) {
	nm=t-1;
	if ((fabs(rv1[t])+anorm) == anorm) {

	  flag=0;
	  break;
	}
	if ((fabs(S(nm))+anorm) == anorm){

         break;
        }
      }
      if (flag) {
	c=0.0;
	s=1.0;
	for (i=t;i<=k;i++) {
	  f=s*rv1[i];
	  if (fabs(f)+anorm != anorm) {
	    g=S(i);
	    h=pythag(f,g);
	    S(i)=h;
	    h=1.0/h;
	    c=g*h;
	    s=(-f*h);
	    for (j=0;j<m;j++) {
	      y=U(j,nm);
	      z=U(j,i);
	      U(j,nm)=y*c+z*s;
	      U(j,i)=z*c-y*s;
	    }
	  }
	}
      }
      z=S(k);
      if (t == k) {
	if (z < 0.0) {
	  S(k) = (-z);
	  for (j=0;j<n;j++) {
	    V(j,k)=(-V(j,k));
	  }
	}

	break;
      }

      if (its == maxIterations){
	/* printf("svdcmp: No convergence in %d SVD iterations\n",maxIterations); */
	ret = -2;
      }
      x=S(t);
      nm=k-1;
      y=S(nm);
      g=rv1[nm];
      h=rv1[k];
      f=(double)(((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y));
      g=pythag(f,1.0);
      f=(double)((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
      c=s=1.0;
      for (j=t;j<=nm;j++) {
	i=j+1;
	g=rv1[i];
	y=S(i);
	h=s*g;
	g=c*g;
	z=pythag(f,h);
	rv1[j]=z;
	c=f/z;
	s=h/z;
	f=x*c+g*s;
	g=g*c-x*s;
	h=y*s;
	y=y*c;
	for (jj=0;jj<n;jj++) {
	  x=V(jj,j);
	  z=V(jj,i);
	  V(jj,j)=x*c+z*s;
	  V(jj,i)=z*c-x*s;
	}
	z=pythag(f,h);
	S(j)=z;
	if (z) {
	  z=1.0/z;
	  c=f*z;
	  s=h*z;
	}
	f=(c*g)+(s*y);
	x=(c*y)-(s*g);
	for (jj=0;jj<m;jj++) {
	  y=U(jj,j);
	  z=U(jj,i);
	  U(jj,j)=y*c+z*s;
	  U(jj,i)=z*c-y*s;
	}
      }
      rv1[t]=0.0;
      rv1[k]=f;
      S(k)=x;
    }
  }

  delete[] rv1;
  svdSort(U,S,V);
  return ret;
}

struct singularValuesDouble {
  double value;
  unsigned int   index;
};

static int compareSingularValuesDouble(const void *A,const void *B)
{
  singularValuesDouble *a = (singularValuesDouble*)A;
  singularValuesDouble *b = (singularValuesDouble*)B;
  if (a->value < b->value) return 1;
  if (a->value > b->value) return -1;
  return 0;
}

void svdSort(doubleMatrix &U, doubleVector &S, doubleMatrix &V)  
{
  // number of elements in S
  unsigned int m = V.rows();
  
  singularValuesDouble *sv = new singularValuesDouble[m];
  for(unsigned int x=0;x<m;x++) {
    sv[x].value = S(x);
    sv[x].index = x;
  }
  
  qsort(sv,m,sizeof(singularValuesDouble),compareSingularValuesDouble);

  for(unsigned int y=0;y<m;y++) {

    if (sv[y].index != y) {
      double tmp;

      /* swap columns in V */
      for (unsigned int x=0;x<V.rows();x++) {
	tmp = V(x,y);
	V(x,y)=V(x,sv[y].index);
	V(x,sv[y].index)=tmp;
      }

      /* swap columns in U */
      for (unsigned int x=0;x<U.rows();x++) {
	tmp = U(x,y);
	U(x,y)=U(x,sv[y].index);
	U(x,sv[y].index)=tmp;
      }

      tmp = S(y);
      S(y)=S(sv[y].index);
      S(sv[y].index)=tmp;

      /* find references to y in the next elements and replace it with sv[y].index */
      for (unsigned int k=y;k<m;k++) {
	if (sv[k].index == y)
	  sv[k].index = sv[y].index;
      }

    }
  }

  //  /* swap S */
  //  for(unsigned int x=0;x<m;x++) {
  //    S[x] = sv[x].value;
  //  }

  delete[] sv;
}
