_real_params = """
>>> a = num.array([[1.,2.], [3.,4.]])
>>> b = num.array([2., 1.])
>>> eps = 1e-14
"""

_complex_params = """
>>> a = a.astype(num.Complex64)
>>> b = b.astype(num.Complex64)
"""

_la_tests = """
>>> x = solve_linear_equations(a, b);
>>> case( num.matrixmultiply(a, x), b, eps)
OK
>>> a_inv = inverse(a);
>>> case(num.matrixmultiply(a, a_inv),
...      num.identity(a.shape[0], type=a.type()), eps)
OK
>>> ev = eigenvalues(a); evalues, evectors = eigenvectors(a)
>>> case(ev, evalues, eps)
OK
>>> evectors = num.transpose(evectors)
>>> case(num.matrixmultiply(a, evectors), evectors*evalues, eps)
OK
>>> u, s, vt = singular_value_decomposition(a)
>>> case(a, num.matrixmultiply(u*s, vt), eps)
OK
>>> a_ginv = generalized_inverse(a)
>>> case(num.matrixmultiply(a, a_ginv),
...      num.identity(a.shape[0], type=a.type()), eps)
OK
>>> case(determinant(a), num.multiply.reduce(evalues), eps)
OK
>>> x, residuals, rank, sv = linear_least_squares(a, b)
>>> case(b, num.matrixmultiply(a, x), eps)
OK
>>> rank-a.shape[0]
0
>>> case(sv, s, eps)
OK
"""

_qr_tests = """
>>> eps = 1e-14
>>> a = num.array([[1,2,3],[3,4,5]]); testqr(a, eps)
OK
>>> a = num.array([[1,2,3],[3,4,5]]); testqr(num.transpose(a), eps)
OK
>>> a=num.array([[1+1j,2,3],[3,4,5]]); testqr(a, eps)
OK
>>> a=num.array([[1+1j,2,3],[3,4,5]]); testqr(num.transpose(a), eps)
OK
>>> a=num.array([[1+1j,2],[3,4]]); testqr(a, eps)
OK
>>> a=num.array([[1,2],[3,4]]); testqr(a, eps)
OK
"""

__doc__ = _real_params + _la_tests + _complex_params + _la_tests + _qr_tests

import numarray.numeric as num
from LinearAlgebra2 import *

class SelftestFailure(Exception):
	pass

def cndns(m):
    return num.maximum.reduce(num.inputarray(num.abs(m)).flat)

def case(expr, ans, eps):
    if abs(cndns(expr - ans) / cndns(ans)) < eps:
        print "OK"
    else:
	raise SelftestFailure

def testqr(a, eps):
	q,r = qr_decomposition(a,mode='e')
	q,r = qr_decomposition(a,mode='r')
	q,r = qr_decomposition(a,mode='full')
	res = num.matrixmultiply(q,r)
	b = num.ravel( num.abs(res - a) )
        if num.maximum.reduce(b) > eps:
            raise SelftestFailure
        else:
            print "OK"

