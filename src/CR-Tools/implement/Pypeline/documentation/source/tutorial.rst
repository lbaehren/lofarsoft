.. _tutorial:

==========
 Tutorial
==========

This tutorial will get you started with the pycrtools package.

.. Installation
.. ============


Starting up
===========

To work with the pycrtools package it first has to be loaded. This can
be done by starting Python or iPython and type the following at the
prompt (``>>>``)::

   >>> import pycrtools as cr

which makes all pycrtools functionality available under the namespace
:mod:`cr`. If you don't want to type the :mod:`cr` prefix for each
pycrtools function, and have all functionality available in the
default namespace, type::

   >>> from pycrtools import *



Vectors
=======

Some basics
-----------

The fundamental data structure we use is a standard C++ vector defined
in the C++ standard template library (STL). This is wrapped and
exposed to Python using the BOOST PYTHON system.

(NB: Unfortunately different systems provide different Python data
structures. Hence a function exposed to Python with SWIG or SIP is not
directly able to accept a BOOST PYTHON wrapped vector as input or vice
versa. If you want to do this you have to provide extra conversion
routines.)

In line with the basic Python philosophy, vectors are passed as
references. Since we are working with large sets of data processing
time is as important as convenience. Hence the basic principle is that
we try to avoid copying large chunks of data as much as possible.

The basic functions operating on the data in C++ either take STL
iterators as inputs (i.e. pointers to the begin and end of the memory
where the data in the STL vector is stored) or casa::Vectors, which
are created with shared memory (i.e. their physical memory is the same
as that of the STL vector).

For that reason MEMORY ALLOCATION is done almost exclusively in the
Python layer. The fast majority of C++ functions is not even able to
allocate or free any memory. This allows for very efficient memory
management and processing, but also means that the user is responsible
for providing properly sized vectors as input AND output
vectors. I.e. you need to know beforehand what sized vector you
expect in return!

This may be annoying, but forces you to think carefully about how to
use memory. For example, if there is a processing done multiple times
using a scratch vector of fixed size, you reuse the same vector over
and over again, thus avoiding a lot of unnecessary allocation and
deallocation of memory and creation of vectors.

Also, the basic vectors are inherently one-dimensional and not
multi-dimensional. On the other hand, multi-dimensional data is always
simply written sequentially into the memory - you just need to know
(and think about) how your data is organized.


Constructing vectors
--------------------

A number of vector types are provided: bool, int, float, complex, and
str. To create a vector most efficiently, use the original vector constructors:

1. BoolVec()
2. IntVec()
3. FloatVec()
4. ComplexVec()
5. StrVec()

e.g.::

    >>> v = FloatVec()
    >>> v
    Vector(float, 0, fill=[])

This will create a floating point vector of size 0. The vector can be
filled with a Python list or tuple, by using the extend attribute::

   >>> v.extend([1,2,3,4])
   >>> v
   Vector(float, 4, fill=[1,2,3,4])

Note, that Python has automatically converted the integers into
floats, since the STL vector does not allow any automatic typing.

The STL vector can be converted back to a Python list by using the
Python list creator::

    >>> list(v)
    [1.0, 2.0, 3.0, 4.0]

or use the :meth:`list` or :meth:`val` methods of the vector (where
the latter has the extra twist that it will return a scalar value, if
the vector has a length of one)::

    >>> v.val()
    [1.0, 2.0, 3.0, 4.0]
    >>> v.list()
    [1.0, 2.0, 3.0, 4.0]

However, the basic Boost Python STL vector constructor takes no
arguments and is a bit cumbersome to use in the long run.  Here we
provide a wrapper function that is useful for interactive use.

Usage:

``Vector(Type)``
  will create an empty vector of type "Type", where ``Type`` is a
  basic Python type, i.e.  bool, int, float, complex, str.

``Vector(Type, size)``
  will create an vector of type ``Type``, with length ``size``.

``Vector(Type, size, fill)``
  will create an vector of type ``Type``, with length ``size`` and
  initialized with the value ``fill``.

``Vector([1,2,3,...])`` or ``Vector((1,2,3,...))``

  if a *list* or a *tuple* is provided as first argument then a vector
  is created of the type of the first element in the list or tuple
  (here an integer) and filled with the contents of the list or tuple.

So, what we will now use is::

    >>> v = Vector([1.,2,3,4])
    >>> v
    Vec(4)=[1.0,2.0,3.0,4.0]

Note, that size and fill take precedence over the list and tuple
input. Hence if you create a vector with ``Vector([1,2,3], size=2)`` it
will contain only ``[1,2]``. ``Vector([1,2,3], size=2, fill=4)`` will give
``[4,4]``.

.. Some simple support for multiple dimensions had been implemented,
.. using the methods::

..     >>> vector.setDim([n1,n2,..])
..     >>> vector.getDim()
..     >>> vector.elem(n)

.. However, this is already depreciated, since there is an array class to
.. do this better.



Referencing, memory allocation, indexing, slicing
-------------------------------------------------

Following basic Python rules, the STL vector will persist in memory as
long as there is a Python reference to it. If you destroy ``v`` also the
C++ memory will disappear. Hence, if you keep a pointer to the vector
in C++ and try to work on it after the Python object was destroyed,
your program may crash. That is why, by default, memory management is
done **ONLY** on one side, namely the Python side!

To illustrate how Python deals with references, consider the following
example::

    >>> x = v
    >>> x[0] = 3
    >>> v
    Vec(4)=[3.0,2.0,3.0,4.0]

Hence, the new Python object ``x`` is actually a reference to the same C++
vector that was created in ``v``. Modifying elements in ``x`` modifies
elements in ``v``. If you destroy ``v`` or ``x``, the vector will not be
destroyed, since there is still a reference to it left. Only if you
destroy ``x`` and ``v`` the memory will be freed.

As noted above, this vector is subscriptable and sliceable, using the
standard Python syntax::

    >>> v[1:3]
    Vec(2)=[2.0,3.0]

We can also resize vectors and change their memory allocation::

    >>> v1 = Vector([0.0,1,2,3,4,5])
    >>> v1
    Vector(float, 6, fill=[0,1,2,3,4,5])

    >>> v2 = Vector(float,len(v1),2.0)
    >>> v2
    Vector(float, 6, fill=[2,2,2,2,2,2])

With the resize attribute you allocate new memory while keeping the
data. It is not guaranteed that the new memory actually occupies the
same physical space::

    >>> v2.resize(8)
    >>> v2
    Vector(float, 8, fill=[2,2,2,2,2,2,0,0])

Resize a vector and fill new entries with non-zero values::

    >>> v2.resize(10,-1)
    >>> v2
    Vector(float, 10, fill=[2,2,2,2,2,2,0,0,-1,-1])

Resize a vector to same size as another vector::

    >>> v2.resize(v1)
    >>> v2
    Vector(float, 6, fill=[2,2,2,2,2,2])

Make a new vector of same size and type as the original one::

    >>> v3 = v2.new()
    >>> v3
    Vector(float, 6, fill=[0,0,0,0,0,0])

Fill a vector with values::

    >>> v3.fill(-2)
    >>> v3
    Vector(float, 6, fill=[-2,-2,-2,-2,-2,-2])


Vector arithmetic
-----------------

The vectors have a number of mathematical functions attached to
them. A full list can be seen by typing::

    >>> dir(v1)

Some of the basic arithmetic is available in an intuitive way. You can
add a scalar to a vector by::

    >>> v1 + 3

This will actually create a new vector (and destroy it right away,
since no reference was kept). The original vector is unchanged.

A technical limitation is that - even though addition and
multiplication is commutative, the scalar (i.e., non-vector) values
has to come as the second argument.

You can also add two vectors (which is commutative)::

    >>> v1 + v2

In order to change the vector, you can use the "in place" operators
``+=``, ``-=``, ``/=``, ``*=``, e.g. adding a vector in place::

    >>> v1 += v2
    v1 => Vector(float, 6, fill=[2,3,4,5,6,7])

now ``v1`` was actually modified such that ``v2`` was added to the content of
``v1`` and the results is stored in ``v1``. Similarly you can do::

    >>> v1 -= v2
    >>> v1 *= v2
    >>> v1 /= v2

Here are examples of some basic statistics functions one can use::

    >>> # Mean
    >>> v1.mean()
    4.5

    >>> # Median
    >>> v1.median()
    5.0

    >>> # Summing all elements in a vector::
    >>> v1.sum()
    27.0

    >>> # Standard Deviation::
    >>> v1.stddev()
    1.87082869339
