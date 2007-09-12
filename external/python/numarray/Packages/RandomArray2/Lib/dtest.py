"""
>>> eps = 1e-4
>>> seed(104162795, 1578461743)
>>> int((random()-0.0780920758843) < eps)
1

Average of 10000 random numbers

>>> int((num.sum(random(10000))/10000. - 0.49829185756540684) < eps)
1
>>> x = random([10,1000])
>>> x.shape
(10, 1000)
>>> x.shape = 10000

Average of 100 by 100 random numbers

>>> int((num.sum(x)/10000. - 0.50674083693527938) < eps)
1
>>> y = uniform(0.5,0.6, (1000,10))
>>> y.shape
(1000, 10)
>>> y.shape = 10000
>>> num.minimum.reduce(y) <= 0.5
0
>>> num.maximum.reduce(y) >= 0.6
0
>>> showint(randint(1, 10, shape=[50]))
array([4, 8, 5, 4, 9, 7, 2, 8, 6, 2, 5, 9, 1, 4, 6, 2, 2, 1, 4, 4, 9,
       1, 5, 5, 8, 6, 9, 5, 5, 7, 5, 2, 6, 9, 1, 3, 2, 3, 5, 2, 8, 9,
       8, 4, 3, 6, 6, 1, 4, 9], type=Int32)
>>> showint(permutation(10))
array([3, 9, 7, 2, 1, 6, 5, 4, 8, 0], type=Int32)
>>> showint(randint(3,9))
array(5, type=Int32)
>>> showint(random_integers(10, shape=[20]))
array([ 3,  6, 10,  5,  6,  2,  4,  1, 10,  7,  4, 10,  2,  7,  8,  7,
        4,  6,  9,  9], type=Int32)

>>> s = 3.0; x = normal(2.0, s, [10, 1000])
>>> x.shape
(10, 1000)
>>> x.shape = 10000

>>> mean_var_test(x, "normally distributed numbers with mean 2 and variance %f"%(s**2,), 2, s**2, 0, 1.98057479, 8.96347252, 0.01992834, eps=eps)
OK
OK
OK

>>> mean_var_test(exponential(3, 10000), "random numbers exponentially distributed with mean %f"%(s,), s, s**2, 2, 2.97389160, 8.93841228, 1.93402556, eps=eps)
OK
OK
OK

A multivariate normal

>>> x = multivariate_normal(num.array([10,20]), num.array(([1,2],[2,4]))); x
array([  9.95170432,  19.90340867])
>>> x.shape
(2,)

A 4x3x2 array containing multivariate normals

>>> case(multivariate_normal(num.array([10,20]), num.array([[1,2],[2,4]]), [4,3]),
...   num.array([[[ 10.78558756,  21.57117509],
...               [  8.81081523,  17.62163042],
...               [ 10.48636767,  20.97273535]],
...              [[  9.75619604,  19.51239207],
...               [  9.24218798,  18.48437595],
...               [ 10.38599356,  20.77198715]],
...              [[ 11.93676401,  23.873528  ], 
...               [  8.26186252,  16.52372503],
...               [ 11.73060812,  23.46121621]],
...              [[  8.94173038,  17.88346076],
...               [ 10.95564306,  21.91128612],
...               [  8.53284202,  17.06568409]]]),
...        eps)
OK

Average of 10000 multivariate normals with mean [-100,0,100]

>>> x = multivariate_normal(num.array([-100,0,100]),
...                         num.array([[3,2,1],[2,2,1],[1,1,1]]), 10000)
>>> x_mean = num.sum(x)/10000.
>>> x_minus_mean = x - x_mean

Estimated covariance of 10000 multivariate normals with covariance [[3,2,1],[2,2,1],[1,1,1]]

>>> case(num.matrixmultiply(num.transpose(x_minus_mean),x_minus_mean)/9999.,
...  num.array([[ 2.97686405,  1.98555651,  1.00144592],
...             [ 1.98555651,  1.98528312,  0.99716822],
...             [ 1.00144592,  0.99716822,  0.99382558]]),
...      eps)
OK

>>> x = beta(5.0, 10.0, 10000)
>>> mean_var_test(x, "beta(5.,10.) random numbers", 0.333, 0.014, [],
...               0.33464588, 0.01402210, eps=eps)
OK
OK

>>> x = gamma(.01, 2., 10000)
>>> mean_var_test(x, "gamma(.01,2.) random numbers", 2*100, 2*100*100, [],
...      200.10160522, 19908.49448647, eps=eps)
OK
OK

>>> x = chi_square(11., 10000)
>>> mean_var_test(x, "chi squared random numbers with 11 degrees of freedom",
...               11, 22, 2*num.sqrt(2./11.),
...               10.97071185, 21.70231540, 0.81841066, eps=eps)
OK
OK
OK

>>> x = F(5., 10., 10000)
>>> mean_var_test(x, "F random numbers with 5 and 10 degrees of freedom",
...               1.25, 1.35, [],
...               1.24867357, 1.27926212, eps=eps)
OK
OK

>>> x = poisson(50., 10000)
>>> mean_var_test(x, "poisson random numbers with mean 50", 50, 50, 0.14,
...               50.03410000, 49.84952214, 0.13964030, eps=eps)
OK
OK
OK

Each element is the result of 16 binomial trials with probability 0.5:

>>> binomial(16, 0.5, 16)
array([ 5,  8,  6,  5,  7,  5,  4, 10,  6,  9,  7,  8, 10,  8,  5,  9])

Each element is the result of 16 negative binomial trials with probability 0.5:

>>> negative_binomial(16, 0.5, [16,])
array([10,  8, 14, 11, 30, 17, 19,  9, 10, 23, 22, 16,  9, 15, 20, 17])

Each row is the result of 16 multinomial trials with probabilities [0.1, 0.5, 0.1 0.3]:

>>> x = multinomial(16, [0.1, 0.5, 0.1], 8); x
array([[ 2,  6,  4,  4],
       [ 1,  6,  4,  5],
       [ 1, 11,  1,  3],
       [ 0,  9,  2,  5],
       [ 0,  9,  3,  4],
       [ 2,  8,  3,  3],
       [ 0,  7,  3,  6],
       [ 2,  8,  4,  2]])

>>> num.sum(x)/8.   # Mean
array([ 1.,  8.,  3.,  4.])


Using array arguments:

>>> y = beta([5.0, 50.], [10.0, 100.0])
>>> int(y.shape in [(1,), (2,)])
1

>>> y = beta([5.0, 50.], 10.0)
>>> int(y.shape in [(1,), (2,)])
1

>>> y = beta(5.0, [10.0, 100.0])
>>> int(y.shape in [(1,), (2,)])
1

>>> y = beta(5.0, [[10.0, 100.0, 50.0], [12.0, 200.0, 150.0]])
>>> int(y.shape == (2, 3))
1

>>> y = beta(5.0, [10.0, 100.0], shape = (3, 2))
>>> int(y.shape == (3, 2))
1

"""

from RandomArray2 import *
import numarray.numeric as num

class SelftestFailure(Exception):
	pass

def showint(x):
    return num.explicit_type(num.inputarray(x).astype('Int32'))

def cndns(m):
    return num.maximum.reduce(num.inputarray(num.abs(m)).flat)

def case(expr, ans, eps=1e-9):
    if abs(cndns(ans-expr)/cndns(ans)) < eps:
        print "OK"
    else:
        raise SelftestFailure
    
def mean_var_test(x, type, mean, var, skew=[], mean_ans=None, var_ans=None, skew_ans=None,
                  eps=1e-9):
    if mean_ans is None or var_ans is None:
        raise ValueError, "Invalid test parameters"
    n = len(x) * 1.0
    x_mean = num.sum(x)/n
    x_minus_mean = x - x_mean
    x_var = num.sum(x_minus_mean*x_minus_mean)/(n-1.0)    
    case(x_mean, mean_ans, eps)
    case(x_var, var_ans, eps)
    if skew != []:
       x_skew = (num.sum(x_minus_mean*x_minus_mean*x_minus_mean)/9998.)/x_var**(3./2.)
       case(x_skew, skew_ans, eps)

from numarray.numtest import dtp

def test():
    import doctest, dtest
    return doctest.testmod(dtest)
