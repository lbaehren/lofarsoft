"""This module contains templates and classes for generating type
specific versions of various sort functions.

WARNING: This module exists solely as a mechanism to generate a
portion of numarray and is not intended to provide any
post-installation functionality.  
"""

from basecode import CodeGenerator, template, all_types, _HEADER

SORT_HEADER      = _HEADER + \
'''
#define STDC_LT(a,b) ((a) < (b))
#define STDC_LE(a,b) ((a) <= (b))
#define STDC_EQ(a,b) ((a) == (b))

#define SWAP(a,b) { SWAP_temp = a; a = b; b = SWAP_temp;}

#define swap(i, j)   { temp=v[i]; v[i]=v[j]; v[j]=temp; }
#define wswap(i, j)   { temp=v[i];  v[i]=v[j]; v[j]=temp; wtemp=w[i];  w[i]=w[j]; w[j]=wtemp; }
'''

SORT_TEMPLATE = \
'''

void  quicksort0<typename>(<typename> *pl, <typename> *pr)
{
	<typename> vp, SWAP_temp;
	<typename> *stack[100], **sptr = stack, *pm, *pi, *pj, *pt;
        
	for(;;) {
		while ((pr - pl) > 15) {
			/* quicksort partition */
			pm = pl + ((pr - pl) >> 1);
			if (<lessthan>(*pm,*pl)) SWAP(*pm,*pl);
			if (<lessthan>(*pr,*pm)) SWAP(*pr,*pm);
			if (<lessthan>(*pm,*pl)) SWAP(*pm,*pl);
			vp = *pm;
			pi = pl;
			pj = pr - 1;
			SWAP(*pm,*pj);
			for(;;) {
				do ++pi; while (<lessthan>(*pi,vp));
				do --pj; while (<lessthan>(vp,*pj));
				if (pi >= pj)  break;
				SWAP(*pi,*pj);
			}
			SWAP(*pi,*(pr-1));
			/* push largest partition on stack */
			if (pi - pl < pr - pi) {
				*sptr++ = pi + 1;
				*sptr++ = pr;
				pr = pi - 1;
			}else{
				*sptr++ = pl;
				*sptr++ = pi - 1;
				pl = pi + 1;
			}
		}
		/* insertion sort */
		for(pi = pl + 1; pi <= pr; ++pi) {
			vp = *pi;
			for(pj = pi, pt = pi - 1; pj > pl && <lessthan>(vp, *pt);) {
				*pj-- = *pt--;
			}
			*pj = vp;
		}
		if (sptr == stack) break;
		pr = *(--sptr);
		pl = *(--sptr);
	}
}

static int quicksort<typename>(long niter, long ninargs, long noutargs, void **buffers, long *bsizes)
{
	<typename> *pl = (<typename> *) buffers[0]; 
	<typename> *pr = pl + niter - 1;

	quicksort0<typename>(pl, pr);
	return 0;
}

UFUNC_DESCR1(quicksort<typename>, sizeof(<typename>));


void  aquicksort0<typename>(long *pl, long *pr, <typename> *v)
{
	<typename> vp;
	long SWAP_temp;
	long *stack[100], **sptr = stack, *pm, *pi, *pj, *pt, vi;
        
	for(;;) {	
		while ((pr - pl) > 15) {
			/* quicksort partition */
			pm = pl + ((pr - pl) >> 1);
			if (<lessthan>(v[*pm],v[*pl])) SWAP(*pm,*pl);
			if (<lessthan>(v[*pr],v[*pm])) SWAP(*pr,*pm);
			if (<lessthan>(v[*pm],v[*pl])) SWAP(*pm,*pl);
			vp = v[*pm];
			pi = pl;
			pj = pr - 1;
			SWAP(*pm,*pj);
			for(;;) {
				do ++pi; while (<lessthan>(v[*pi],vp));
				do --pj; while (<lessthan>(vp,v[*pj]));
				if (pi >= pj)  break;
				SWAP(*pi,*pj);
			}
			SWAP(*pi,*(pr-1));
			/* push largest partition on stack */
			if (pi - pl < pr - pi) {
				*sptr++ = pi + 1;
				*sptr++ = pr;
				pr = pi - 1;
			}else{
				*sptr++ = pl;
				*sptr++ = pi - 1;
				pl = pi + 1;
			}
		}
		/* insertion sort */
		for(pi = pl + 1; pi <= pr; ++pi) {
			vi = *pi;
			vp = v[vi];
			for(pj = pi, pt = pi - 1; pj > pl && <lessthan>(vp, v[*pt]);) {
				*pj-- = *pt--;
			}
			*pj = vi;
		}
		if (sptr == stack) break;
		pr = *(--sptr);
		pl = *(--sptr);
	}
}

static int aquicksort<typename>(long niter, long ninargs, long noutargs, void **buffers, long *bsizes)
{
	<typename> *v = (<typename> *) buffers[0];
	long *pl = (long *) buffers[1]; 
	long *pr = pl + niter - 1;

	aquicksort0<typename>(pl, pr, v);
	return 0;
}

UFUNC_DESCR2(aquicksort<typename>, sizeof(<typename>), sizeof(long));


void mergesort0<typename>(<typename> *pl, <typename> *pr, <typename> *pw)
{
	<typename> vp, *pi, *pj, *pk, *pm;
	
	if (pr - pl > 20) {
		/* merge sort */
		pm = pl + ((pr - pl + 1)>>1);
		mergesort0<typename>(pl,pm-1,pw);
		mergesort0<typename>(pm,pr,pw);
		for(pi = pw, pj = pl; pj < pm; ++pi, ++pj) {
			*pi = *pj;
		}
		for(pk = pw, pm = pl; pk < pi && pj <= pr; ++pm) {
			if (<lessequal>(*pk,*pj)) {
				*pm = *pk;
				++pk;
			}else{
				*pm = *pj;
				++pj;
			}
		}
		for(; pk < pi; ++pm, ++pk) {
			*pm = *pk;
		}
	}else{
		/* insertion sort */
		for(pi = pl + 1; pi <= pr; ++pi) {
			vp = *pi;
			for(pj = pi, pk = pi - 1; pj > pl && <lessthan>(vp, *pk); --pj, --pk) {
				*pj = *pk;
			}
			*pj = vp;
		}
        }
}

static int mergesort<typename>(long niter, long ninargs, long noutargs, void **buffers, long *bsizes)
{
	<typename> *pl = (<typename> *) buffers[0];
	<typename> *pr = pl + niter - 1;
	<typename> *pw = (<typename> *) malloc(((1 + niter/2))*sizeof(<typename>));

	if (pw != NULL) {
		mergesort0<typename>(pl, pr, pw);
		free(pw);
		return 0;
	}else{
		return -1;
	}
}

UFUNC_DESCR1(mergesort<typename>, sizeof(<typename>));


void amergesort0<typename>(long *pl, long *pr, <typename> *v, long *pw)
{
        <typename> vp;
	long vi, *pi, *pj, *pk, *pm;
        
	if (pr - pl > 20) {
		/* merge sort */
		pm = pl + ((pr - pl + 1)>>1);
		amergesort0<typename>(pl,pm-1,v,pw);
		amergesort0<typename>(pm,pr,v,pw);
		for(pi = pw, pj = pl; pj < pm; ++pi, ++pj) {
			*pi = *pj;
		}
		for(pk = pw, pm = pl; pk < pi && pj <= pr; ++pm) {
			if (<lessequal>(v[*pk],v[*pj])) {
				*pm = *pk;
				++pk;
			}else{
				*pm = *pj;
				++pj;
			}	
		}
		for(; pk < pi; ++pm, ++pk) {
			*pm = *pk;
		}
	}else{
		/* insertion sort */
		for(pi = pl + 1; pi <= pr; ++pi) {
			vi = *pi;
			vp = v[vi];
			for(pj = pi, pk = pi - 1; pj > pl && <lessthan>(vp, v[*pk]); --pj, --pk) {
				*pj = *pk;
			}
			*pj = vi;
		}
	}
}

static int amergesort<typename>(long niter, long ninargs, long noutargs, void **buffers, long *bsizes)
{
	<typename> *v  = (<typename> *) buffers[0];
	long *pl = (long *) buffers[1]; 
	long *pr = pl + niter - 1;
	long *pw = (long *) malloc(((1 + niter/2))*sizeof(long));

	if (pw != NULL) {
		amergesort0<typename>(pl, pr, v, pw);
		free(pw);
		return 0;
	}else{
		return -1;
       }
}

UFUNC_DESCR2(amergesort<typename>, sizeof(<typename>), sizeof(long));


void heapsort0<typename>(<typename> *a, long n)
{
	<typename> tmp;
	long i,j,l;

	/* The array needs to be offset by one for heapsort indexing */
        a -= 1;
        
	for (l = n>>1; l > 0; --l) {
		tmp = a[l];
		for (i = l, j = l<<1; j <= n;) {
			if (j < n && <lessthan>(a[j], a[j+1])) 
				j += 1;
			if (<lessthan>(tmp, a[j])) {
				a[i] = a[j];
				i = j;
				j += j;
			}else
				break;
		}
		a[i] = tmp;
	} 

	for (; n > 1;) {
		tmp = a[n];
		a[n] = a[1];
		n -= 1;
		for (i = 1, j = 2; j <= n;) {
			if (j < n && <lessthan>(a[j], a[j+1]))
				j++;
			if (<lessthan>(tmp, a[j])) {
				a[i] = a[j];
				i = j;
				j += j;
			}else
				break;
		}
		a[i] = tmp;
	}
}

static int heapsort<typename>(long niter, long ninargs, long noutargs, void **buffers, long *bsizes)
{
	<typename> *pl = (<typename> *) buffers[0];
        
	heapsort0<typename>(pl, niter);
	return 0;
}

UFUNC_DESCR1(heapsort<typename>, sizeof(<typename>));


void aheapsort0<typename>(long *a, long n, <typename> *v)
{
	long i,j,l, tmp;

	/* The arrays need to be offset by one for heapsort indexing */
        a -= 1;
        
	for (l = n>>1; l > 0; --l) {
		tmp = a[l];
		for (i = l, j = l<<1; j <= n;) {
			if (j < n && <lessthan>(v[a[j]], v[a[j+1]])) 
				j += 1;
			if (<lessthan>(v[tmp], v[a[j]])) {
				a[i] = a[j];
				i = j;
				j += j;
			}else
				break;
		}
		a[i] = tmp;
	} 

	for (; n > 1;) {
		tmp = a[n];
		a[n] = a[1];
		n -= 1;
		for (i = 1, j = 2; j <= n;) {
			if (j < n && <lessthan>(v[a[j]], v[a[j+1]]))
				j++;
			if (<lessthan>(v[tmp], v[a[j]])) {
				a[i] = a[j];
				i = j;
				j += j;
			}else
				break;
		}
		a[i] = tmp;
	}
}

static int aheapsort<typename>(long niter, long ninargs, long noutargs, void **buffers, long *bsizes)
{
	<typename> *v = (<typename> *) buffers[0];
	long *pl = (long *) buffers[1]; 

	aheapsort0<typename>(pl, niter, v);
	return 0;
}

UFUNC_DESCR2(aheapsort<typename>, sizeof(<typename>), sizeof(long));


void sort0<typename>(<typename> *v, long left, long right)
{
  <typename> temp;
  long i, last, lastl, lastr, diff;

  diff = right - left;
  if (diff <= 0)
     return;

  /* choose the pivot randomly. */
  i = left + (int) (((right-left)*1.0*rand())/(RAND_MAX+1.0));

  /* Split array into values < pivot, and values > pivot. */
  swap(left, i);
  last = left;
  for(i=left+1; i<=right; i++)
    if (<lessthan>(v[i], v[left]))
    {
      ++ last;
      swap(last, i);
    }
  swap(left, last);
  
  lastl = last-1;
  lastr = last+1;

  /* Exclude values equal to pivot from recursion */
  while((left < lastl) && <equals>(v[last],v[lastl]))
     --lastl;
  while((lastr < right) && <equals>(v[last],v[lastr]))
     ++lastr;

  sort0<typename>(v, left, lastl);
  sort0<typename>(v, lastr, right);
}

static int sort<typename>(long niter, long ninargs, long noutargs,
           void **buffers, long *bsizes)
{
    BEGIN_THREADS
    sort0<typename>( (<typename> *) buffers[0], 0, niter-1);
    END_THREADS
    return 0;
}

UFUNC_DESCR1(sort<typename>, sizeof(<typename>));

void asort0<typename>(<typename> *v, long *w, long left, long right)
{
  <typename> temp;
  long  wtemp, i, last, lastl, lastr;

  if (left >= right)
    return;

  /* choose the pivot randomly. */
  i = left + (int) (((right-left)*1.0*rand())/(RAND_MAX+1.0));

  wswap(left, i);
  last = left;
  for(i=left+1; i<=right; i++)
    if (<lessthan>(v[i], v[left]))
    {
      ++ last;
      wswap(last, i);
    }
  wswap(left, last);

  lastl = last-1;
  lastr = last+1;

  /* Exclude values equal to pivot from recursion */
  while((left < lastl) && <equals>(v[last],v[lastl]))
     --lastl;
  while((lastr < right) && <equals>(v[last],v[lastr]))
     ++lastr;

  asort0<typename>(v, w, left, lastl);
  asort0<typename>(v, w, lastr, right);
}

static int asort<typename>(long niter, long ninargs, long noutargs, void **buffers, long *bsizes)
{
    BEGIN_THREADS
    asort0<typename>( (<typename> *) buffers[0], (long *) buffers[1],
       0, niter-1);
    END_THREADS
    return 0;
}

CFUNC_DESCR(asort<typename>, CFUNC_UFUNC, CHECK_ALIGN, 0, 2, sizeof(<typename>), sizeof(long), 0, 0, 0, 0);

static long search<typename>(long na, <typename> *a, <typename> v)
{
  long i = 0;
  <typename> *b;
  while(na > 10)
    {
      long mid = na>>1;
      if (<lessequal>(v, a[mid]))
        na = mid;
      else
        {
          i += mid;
          a += mid;
          na -= mid;
        }
    }
  b = a;
  while((<lessthan>(*b, v)) && na--)
    ++ b;
  return i+(b-a);
}

static int searchsorted<typename>(int niter, int ninargs, int noutargs,
          void **buffers, long *bsizes)
{
  maybelong   nbins;   
  <typename> *bins;
  <typename> *values;
  long       *indices;
  maybelong i;
  
  if (NA_checkOneCBuffer("searchsorted<typename>", 1,
       buffers[0], bsizes[0], sizeof(maybelong)))
       return -1;
  nbins    = *(maybelong *)  buffers[0];

  if (NA_checkOneCBuffer("searchsorted<typename>", nbins,
      buffers[1], bsizes[1], sizeof(<typename>)))
      return -1;
  bins    = (<typename> *) buffers[1];

  if (NA_checkOneCBuffer("searchsorted<typename>", niter,
      buffers[2], bsizes[2], sizeof(<typename>)))
      return -1;
  values  = (<typename> *) buffers[2];

  if (NA_checkOneCBuffer("searchsorted<typename>", niter,
      buffers[3], bsizes[3], sizeof(long)))
      return -1;
  indices = (long *)   buffers[3];

  if (NA_checkIo("searchsorted<typename>", 3, 1, ninargs, noutargs))
     return -1;

  BEGIN_THREADS
  for(i=0; i<niter; i++)
     indices[i] = search<typename>(nbins, bins, values[i]);
  END_THREADS
  return 0;
}

SELF_CHECKED_CFUNC_DESCR(searchsorted<typename>, CFUNC_UFUNC);

static int fillarray<typename> (long niter, long ninargs, long noutargs, 
			     void **buffers, long *bsizes) {
    long i;
    <typename> start, delta;
    <typename> *tin;
    <typename> *tparams;
    Int8 itemsizes[2] = { sizeof(<typename>), sizeof(<typename>) };
    Int8 iters[2] = { 0, 2 };

    if (NA_checkIo("fillarray<typename>", 1, 1, ninargs, noutargs) ||
	NA_checkNCBuffers("fillarray<typename>", 2,
			 niter, buffers, bsizes, itemsizes, iters))
	    return -1;

    tin = (<typename> *) buffers[0];
    tparams = (<typename> *) buffers[1];
    start = tparams[0];
    delta = tparams[1];

    BEGIN_THREADS
    for (i=0; i<niter; i++, tin++) {
        <fillstep>
    }
    END_THREADS
    return 0;
}

SELF_CHECKED_CFUNC_DESCR(fillarray<typename>, CFUNC_UFUNC);

static int nonzeroCoords<typename>(long niter, long ninargs, long noutargs,
                               void **buffers, long *bsizes)
{
	<typename>   *source;
	maybelong  *sstride;
	long  **output;
	long i, minbsize, next = 0;
	
	if (NA_checkIo("nonzeroCoords<typename>", 2, noutargs, ninargs, noutargs))
		return -1;

	if (NA_checkOneCBuffer("nonzeroCoords<typename>", niter,
		      buffers[0], bsizes[0], sizeof(<typename>)))
		return -1;
	    
	if (NA_checkOneCBuffer("nonzeroCoords<typename>", noutargs,
		      buffers[1], bsizes[1], sizeof(maybelong)))
		return -1;

	/* Check alignment only.  Storage optimization makes numarray
	 shorter than niter likely, possibly even 0! */
	for (i=0; i<noutargs; i++)
		if (NA_checkOneCBuffer("nonzeroCoords<typename>", 0,
			      buffers[2+i], bsizes[2+i], sizeof(long)))
			return -1;

	source     = (<typename> *)  buffers[0];
	sstride    = (maybelong  *)  buffers[1];
	output     = (long **) &buffers[2];

	minbsize = bsizes[0];
	for(i=0; i<noutargs; i++)
		if (sstride[i] <= 0) {
			PyErr_Format(PyExc_ValueError,
			     "nonzeroCoords<typename>: bad stride[%%ld].\\n",
				     i);
			return -1;
		}
		else if (bsizes[i+2] < minbsize)
			minbsize = bsizes[i+2];
	
	next = 0;
	for(i=0; i<niter; i++)
	{
		if (<nonzero>)
		{
			long j, index = i;
			if (next >= minbsize) {
				PyErr_Format(PyExc_ValueError,
					     "nonzeroCoords<typename>: insufficient index space.\\n");
				return -1;
			}
			for(j=0; j<noutargs; j++)
			{
				output[j][next] = index / sstride[j];
                                index %%= sstride[j];
			}
			++ next;
		}
	}
	return 0;
}

SELF_CHECKED_CFUNC_DESCR(nonzeroCoords<typename>, CFUNC_UFUNC);

'''

# ============================================================================
#          IMPORTANT:  no <>-sugared strings below this point

# translate <var> --> %(var)s in templates seen *so far*
template.sugar_dict(globals())  

# ============================================================================
    
sort_operator_td = {
  "lessthan"          : "STDC_LT",
  "lessequal"         : "STDC_LE",
  "equals"            : "STDC_EQ",
  "clear"             : "s = 0;",
  "dotstep"           : "s += *ap * *bp;",
  "fillstep"          : "*tin = start;  start += delta;",
  "nonzero"           : "source[i] != 0"
  }

sort_complex_td = {
  "lessthan"          : "NUM_CLT",
  "lessequal"         : "NUM_CLE",
  "equals"            : "NUM_CEQ",
  "clear"             : "s.r = 0; s.i=0;",
  "dotstep"           : "Complex64 t; NUM_CMUL(*ap, *bp, t); NUM_CADD(s, t, s);",
  "fillstep"          : "NUM_CASS(*tin, start); NUM_CADD(start, delta, start);",
  "nonzero"           : "((source[i].r !=0) || (source[i].i != 0))"
  }


class SortCodeGenerator(CodeGenerator):
    def __init__(self, *components):
        CodeGenerator.__init__(self, *components)
        self.module = "_sort"
        self.qualified_module = "numarray._sort"

    def addcfunc(self, type, name):
        CodeGenerator.addcfunc(self, name+type, key=repr((type, name)))

    def gen_body(self):
        for t in all_types():
            d = {}
            d["typename"] = t
            if t in ["Complex32","Complex64"]:
              d.update(sort_complex_td)
            else:
              d.update(sort_operator_td)
            self.codelist.append((self.separator + SORT_TEMPLATE) % d)
            self.addcfunc(t, "sort")
            self.addcfunc(t, "asort")
            self.addcfunc(t, "searchsorted")
            self.addcfunc(t, "fillarray")
            self.addcfunc(t, "nonzeroCoords")
            self.addcfunc(t, "quicksort")
            self.addcfunc(t, "aquicksort")
            self.addcfunc(t, "mergesort")
            self.addcfunc(t, "amergesort")
            self.addcfunc(t, "heapsort")
            self.addcfunc(t, "aheapsort")

generate_sort_code  = SortCodeGenerator(SORT_HEADER)
