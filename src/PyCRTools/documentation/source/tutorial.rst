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

.. note::

   The latter approach is not recommended as all pycrtools
   functions get cluttered up in the current namespace possibly
   overwriting already existing functions.

Vectors
=======

Some basics
-----------

The fundamental data structure we use is a standard C++ vector defined
in the C++ standard template library (STL). This is wrapped and
exposed to Python using the Boost Python system.

(NB: Unfortunately different systems provide different Python data
structures. Hence a function exposed to Python with SWIG or SIP is not
directly able to accept a Boost Python wrapped vector as input or vice
versa. If you want to do this you have to provide extra conversion
routines.)

In line with the basic Python philosophy, vectors are passed as
references. Since we are working with large sets of data processing
time is as important as convenience. Hence the basic principle is that
we try to avoid copying large chunks of data as much as possible.

The basic functions operating on the data in C++ either take STL
iterators as inputs (i.e. pointers to the begin and end of the memory
where the data in the STL vector is stored) or ``casa::Vectors``, which
are created with shared memory (i.e. their physical memory is the same
as that of the STL vector).

For that reason *memory allocation* is done almost exclusively in the
Python layer. The fast majority of C++ functions is not even able to
allocate or free any memory. This allows for very efficient memory
management and processing, but also means that the user is responsible
for providing properly sized vectors as input *and* output
vectors. I.e., you need to know beforehand what sized vector you
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

#. :meth:`BoolVec`
#. :meth:`IntVec`
#. :meth:`FloatVec`
#. :meth:`ComplexVec`
#. :meth:`StrVec`

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

    >>> # Summing all elements in a vector
    >>> v1.sum()
    27.0

    >>> # Standard Deviation
    >>> v1.stddev()
    1.87082869339


Arrays
======

While the basic underlying data structures are plain STL vectors, in
many cases, however, one has to deal with multi-dimensional data. For
this case we introduce a new wrapper class, named hArrays, that
mimicks a multi-dimensional array, but still operates on an underlying
vector with essentially a flat, horizontal data structure. Given that
a major concern is to minimize duplication of large data structures,
the array class will share memory with its associated vector and also
with arrays that are derived from it. Explicit copying will have to be
done in order to avoid this. Access to various dimensions (rows,
columns, etc...) is done via slices that need to be contiguous in
memory! Since the array is vector-based, all methods defined for
vectors are also inherited by hArrays and can be applied to slices or
even automatically loop over multiple slices (e.g., rows or columns).


Creating Arrays and basic operations
------------------------------------

An array is defined using the ``hArray`` function. This is a constructor
function and not a class of its own. It will return array classes of
different types, such as ``IntArray``, ``FloatArray``, ``ComplexArray``,
``StringArray``, ``BoolArray``, referring to the different data types they
contain. As for vectors, each array can only contain one type of data, e.g.::

    >>> hArray(Type=float,dimensions=[n1,n2,n3...],fill=None) -> FloatArray

where ``Type`` can be a Python type, a Python list/tuple (where the first
element determines the type), an STL vector, or another ``hArray``.

Dimensions are given as a list of the form ``[dim1,dim2,dim3, ...]``. The
size of the underlying vector will automatically be resized to
``dim1*dim2*dim3*...`` to be able to contain all elements. Alternatively,
one can provide another array, which dimensions will be copied.

The array can be filled with initialization values that can be
either a single value, a list, a tuple, or an STL vector of the same
type::

    >>> v = Vector(range(9))

    >>> a = hArray(v, [3,3])
    >>> a
    hArray(int, [3, 3], fill=[0,1,2,3,4,5,6,7,8]) # len=9 slice=[0:9])

One may wonder what the representation of the Array actually
means.

The first argument of the result of the :meth:`hArray` command contains
the data type. Then the array dimensions and total vector size, and
finally the currently active slice (given as start and end index of
the vector). An optional asterisk indicates that the next operation
will actually loop the previously specified slices (see below). At the
end the currently selected slice is displayed (while the array may
actually hold more data).

The underlying vector of an array can be retrieved with the :meth:`vec()`
method. I.e.::

    >>> a.vec()

The arrays have most of the vector methods defined, so you can also
add, multiply, etc. with scalars or other arrays::

    >>> a * 2
    hArray(int, [3, 3], fill=[0,2,4,6,8,10,12,14,16]) # len=9 slice=[0:9])

    >>> a * a
    hArray(int, [3, 3], fill=[0,1,4,9,16,25,36,49,64]) # len=9 slice=[0:9])

Underlying these operations are the basic hftools functions, e.g. the
multiplication is essentially a python method that first creates a new
array and then calls ``hMul``::

    >>> tmp_array = a.new()
    >>> tmp_array.mul(a,2)
    >>> a
    hArray(int, [3, 3], fill=[0,1,2,3,4,5,6,7,8]) # len=9 slice=[0:9])

This could also be done calling the function ``hMul(tmp_array,a,2)``,
rather than the corresponding method.

An important constraint is that all these functions or methods only
work with either vector or array classes, a mix in the parameters
between vectors and arrays is currently not supported.


Changing dimensions
-------------------

The dimensions can be obtained and set, using the :meth:`getDim` and
:meth:`setDim` methods. If the length of the underlying vector changes
due to a change in the dimensions, the vector will be resized and
padded with zeros, if necessary::

    >>> a.getDim()

    >>> a.setDim([3,3,2])

    >>> a.setDim([3,3])


Memory sharing
--------------

Note, that the array and vector point share the same memory. Changing
an element in the vector::

    >>> v[0]=-1
    >>> v
    Vector(int, 9, fill=[-1,1,2,3,4,5,6,7,8])

    >>> a
    hArray(int, [3, 3], fill=[-1,1,2,3,4,5,6,7,8]) # len=9 slice=[0:9])

will also change the corresponding element in the array. The same is
true if one creates an array from an array. Both will share the same
underlying data vector. They will also share the same size and
dimensions::

    >>> b=hArray(a)
    >>> b[0,0]=-2

    >>> b
    hArray(int, [3, 3], fill=[-2,1,2,3,4,5,6,7,8]) # len=9 slice=[0:9])
    >>> a
    hArray(int, [3, 3], fill=[-2,1,2,3,4,5,6,7,8]) # len=9 slice=[0:9])
    >>> v
    Vector(int, 9, fill=[-2,1,2,3,4,5,6,7,8])

    >>> v[0]=0
    >>> a
    hArray(int, [3, 3], fill=[0,1,2,3,4,5,6,7,8]) # len=9 slice=[0:9])

To actually make a physically distinct copy, you need to explicitly
copy the data over::

    >>> c=hArray(int,a)
    >>> a.copy(c)

or more simply::

    >>> c=hArray(int,a,a)

(the 2nd parameter is for the dimensions, the third one is the fill
parameter that initiates the copying).


Basic slicing
-------------

The main purpose of these arrays is, of course, to be able to access
multiple dimensions. This is done using the usual :meth:`__getitem__` method
of Python.

Let us take our two-dimensional array from before::

    >>> a
    hArray(int, [3, 3], fill=[0,0,0,0,0,0,0,0,0]) # len=9 slice=[0:9])

The vector followed by a single number in square brackets
will *in principle* obtain the first column of the array::

    >>> a[0]
    hArray(int, [3, 3], fill=[0,0,0]) # len=9 slice=[0:3])

It says *in principle*, because the only thing which that command does is
to return a new ``hArray`` Python object, which will point to the same
data vector, but contain a different data slice which is then returned
whenever a method tries to operate on the vector::

    >>> a[0].vec()

This retrieves a copy of the data, since assigning a sub-slice of a
vector to another vector actually requires copying the data - as
vectors do not know about slicing (yet). Use one-dimensional arrays if
you want to have are reference to a slice only.

In contrast, ``a.vec()``, without slicing, will give you a reference to
the underlying vector.

For convenience ``a[0,1]`` will return the value, rather than a one
element slice. That behaviour changed from earlier versions and is a
bit inconsistent ...

One may wonder, why one has to use the extra methods :meth:`vec` and
:meth:`val` to access the data. The reason is that slicing on its own
will return an array (and not a vector), which we need for other
purposes still.

Slicing can also be done over multiple elements of one dimension,
using the known Python slicing syntax::

    >>> a[0,0:2].val()

however, currently this is restricted to the last dimension only, in
order to point to a contiguous memory slice. Hence::

    >>> a[0:2]

is possible, but not::

    >>> a[0:2,0:2]

where the first slice is simply ignored.

Finally, negative indices count from the end of the slice, i.e.::

    >>> a[-1]

gives the last slice of the first index, while::

    >>> a[0:-1]

gives all but the last slice of the first index.



Selecting & copying parts of the array - a list as index
--------------------------------------------------------

Assume, we want to have a list of all the elements of a that are
between the values (but excluding) 0 and 10 and perform an operation
on it. Then we need to create an index vector first::

    >>> indices = hArray(int,dimensions=a,fill=-1)

and fill it with the indices according to our condition::

    >>> number_of_indices = indices[...].findbetween(a[...],0,8)
    >>> number_of_indices
    Vector(int, 3, fill=[0,0,0])

    >>> indices[...].pprint(-1)
      [-1,-1,-1]
      [-1,-1,-1]
      [-1,-1,-1]

As the result we get a vector with the number of elements in each row
that have satisfied the condition and in ``indices`` we get their
position. Note that the indices vector must be large enough to hold
all indices, hence in the general case needs to be of the same size
(and dimension) as the input data array. Following our basic philosophy,
the index vector will not be automatically resized. If the number of
selected indices is smaller than the remaining spaces they simply remain
untouched (containing whatever was in there before). To illustrate this
effect, we filled the indices array with "-1"s. If, on the other hand,
the vector were too short it will be filled until the end and then the
search stops. No error message will be given in this case - this is a
feature.

To retrieve the selected elements we make use of the copy method again
to create a new array::

    >>> b = a.new()
    >>> b.fill(-99)
    >>> b[...].copy(a[...], indices[..., [0]:number_of_indices], number_of_indices)
    >>> b.pprint(-1)
    [-99,-99,-99,-99,-99,-99,-99,-99,-99]

This (contiguous) with variable length we can use for further looping
operations (as described below) on the rows of the array. E.g.::

    >>> b[..., [0]:number_of_indices].sum()

will take the sum of the first :math:`n` elements in each row of our array,
where :math:`n` given by the vector number_of_indices that were returned by
out find operation. Clearly, the -99 values that we put into our array
for demonstration purposes were not taken into account for the sum of
the rows. Note, that the slice specification in the line above needs
to have either vectors or scalar values, but not a mix of the
two. This is the reason for using [0]:number_of_indices rather than
just 0:number_of_indices.

It would have been nicer to do right away something like the
following::

    >>> a[indices[...,[0]:number_of_indices],...].sum()

but that is not yet implemented, since looping cannot yet be done over
nested indices!



Applying methods to Slices
--------------------------

First, of all, we can apply the known vector functions also to
array slices directly. E.g.::

    >>> a[0].sum()

will return the sum over the first row of the array, i.e. the first
three elements of the underlying vector. While::

    >>> a[0].negate(); a
    hArray(int, [3, 3], fill=[0,0,0,0,0,0,0,0,0]) # len=9 slice=[0:9])
    >>> a[0].negate(); a
    hArray(int, [3, 3], fill=[0,0,0,0,0,0,0,0,0]) # len=9 slice=[0:9])

returns nothing, but will actually change ths sign of the first three
elements in the underlying vector.

In principle one could now loop over all slices using a for loop::

    >>> for i in range(a.getDim()[0]):
    ...     print "Row",i,":",a[i].val(), " => a =", a
    Row 0 : [0, 0, 0]  => a = hArray(int, [3, 3], fill=[0,0,0,0,0,0,0,0,0]) # len=9 slice=[0:9])
    Row 1 : [0, 0, 0]  => a = hArray(int, [3, 3], fill=[0,0,0,0,0,0,0,0,0]) # len=9 slice=[0:9])
    Row 2 : [0, 0, 0]  => a = hArray(int, [3, 3], fill=[0,0,0,0,0,0,0,0,0]) # len=9 slice=[0:9])

However, looping over slices in simple way is aready built into the
arrays, by appending the *ellipsis symbol* ``...`` to the dimensions. This
will actually put the array in *looping mode*::

    >>> l = a[0:3,...]; l
    hArray(int, [3, 3], fill=[0,0,0]) # len=9 slice=[0:3]*)

which is indicated in the screen representation of the array by an
extra asterisk and actually means that one can loop over all the
elements of the respective dimension::

    >>> iterate=True
    >>> while iterate:
    ...     print "Row",l.loop_nslice(),":",l.val(), " => l =", l
    ...     iterate=l.next().doLoopAgain()
    Row 0 : [0, 0, 0]  => l = hArray(int, [3, 3], fill=[0,0,0]) # len=9 slice=[0:3]*)
    Row 1 : [0, 0, 0]  => l = hArray(int, [3, 3], fill=[0,0,0]) # len=9 slice=[3:6]*)
    Row 2 : [0, 0, 0]  => l = hArray(int, [3, 3], fill=[0,0,0]) # len=9 slice=[6:9]*)

    >>> l
    hArray(int, [3, 3], fill=[0,0,0]) # len=9 slice=[0:3]*)

This will do exactly the same as the for-loop above.

Here :meth:`doLoopAgain` will return ``True`` as long as the array is in looping
mode and has not yet reached the last slice. :meth:`loop_nslice` returns the
current slice the array is set to (see also :meth:`loop_i`, :meth:`loop_start`,
:meth:`loop_end`). :meth:`next` will advance to the next slice until the end is
reached (and doLoopAgain is set to false). The loop will be reset at
the next call of :meth:`next`. Hence, as written above the loop could be
called multiple times where the loop will be automatically reset each
time.

We could also explicitly reset the loop in using to its starting
values, but that should not be necessary most of the time::

    >>> l.resetLoop()

Now, since this is still a bit too much work, you can actually apply
(most of) the available vector methods to multiple slices at once, by
just applying it to an array in looping mode.

As an example, let us calculate the mean value of each slice at the to
level of our example array, which is simply::

    >>> l.mean()

In contrast to the same method applied to vectors, where a single
value is returned, the return value is now a vector of values, each of
which corresponds to the mean of one top-level slice. Hence, the
vector has looped automatically over all the slices specified in the
definition of the array.

The looping over slices can be more complex taking start, stop, and
increment values into account.

    >>> a[1:,...].mean()

will loop over all top-level slices starting at the 2nd slice
(slice #1) until the last.

    >>> a[:2,...].mean()

will loop over the first two top-level slices.

    >>> a[0:3:2,...].mean()

will loop over the two top-level slices using an increment of 2,
i.e. here take the first and third only (so, here non contiguous
slices can be put to work).

To loop over all slices in one dimensions, a short-cut can be used by
leaving away the slice specification. Hence,

    >>> a[...].mean()

will do the same as

    >>> a[0:,..].mean()

It is even possible to specify an array of indices for the slicing.

    >>> a[[0,2],...].mean()

will loop over slices 0 and 2.

It is possible to specify a slice after the ellipse, e.g.,

    >>> a[...,0:2].mean()

which means that the mean is taken only from the first two elements
of each top-level slice.

Even more complicated: the elements of the slice can be vectors or lists:

    >>> a[...,[0,1]:[2,3]].mean()

over which one can loop as well. Hence, in the operation on the first
row, the subslice ``[0:2]`` will be taken, while for the second slice/row
the slice ``[1:3]`` is used.



Parameters of looping arrays
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Looping can also be done for methods that require multiple arrays as
inputs. In this case the :meth:`next` method will be applied to every array
in the paramter list and looping proceeds until the first array has
reached the end. Hence, care has to be taken that the same slice
looping is applied to all arrays in the parameter list.

As an example we create a new array of the dimensions of ``a``::

    >>> x = hArray(int, a)

and fill it with slices from ``a`` multiplied by the scalar value 2::

    >>> x[[0,2],...].mul(a[[0,2],...],2)
    >>> x
    hArray(int, [3, 3], fill=[0,0,0,0,0,0,0,0,0]) # len=9 slice=[0:9])

and indeed now the first and last slice were operated on and filled
with the results of the operation.

Forgetting slicing in a parameter can lead to unexpected results,
e.g., in the following example ``a`` is looped over but ``x`` is not. Hence,
the result will always be written (and overwritten) into the first
three elements of ``x``, containing at the end only the results of the
mutliplication of the last slice in ``a``::

    >>> x.fill(0); x[...].mul(a,2)
    >>> x
    hArray(int, [3, 3], fill=[0,0,0,0,0,0,0,0,0]) # len=9 slice=[0:9])

NOTE: There are currently relatively strict rules on how to change the
parameters from a vector to an array.

#. When going from a vector to an array, all other vectors in the
   argument list also have to be provided as arrays!

#. Scalar parameters can be provided as single-valued scalars or as
   vectors. In the latter case the algorithm will take one element
   after the other in each loop as input parameter.

#. If one scalar parameter is provided as a vector, all scalar
   parameters have to be provided as Vectors. (They can be of different
   length and of length unity, though, which means that always the
   same value is taken.)

#. If an algorithm has a scalar return value, a vector of values will
   be returned by the same algorithm if invoked with arrays.

#. If a slice is specified with vectors as elements
   (i.e. ``[1,2,3]:[5,6,7]``), both start and stop have to be
   vectors. The algorithm will then loop over all elements in the
   lists.



Units and Scale Factors
-----------------------

Numerical arrays allow one to set a (single) unit for the data. With
``setUnit(prefix, unit_name)`` one can specify the name of the unit and
the scale factor, which is specified as a string being one of
'f','p','n','micro','m','c','d','','h','k','M','G','T','P','E','Z'.

    >>> a.setUnit("M","Hz")

will set the unit name to ``MHz`` without modifiying the values in the
array (assuming that the values were deliverd initially in this
unit). However, the scaling can be changed by calling setUnit again
(with or without a unit name), e.g.::

    >>> a.setUnit("k","")

Which has converted the values to ``kHz``.  The name of the unit can be
retrieved with::

    >>> a.getUnit()

and cleared with :meth:`clearUnit`



Keywords and Values
-------------------

For documenting the vector further and to store certain values, one
can store keywords and values in the array. This is done with::

    >>> a.setKey("name","TestArray")

The keywords can be arbitrary strings ann the values also arbitrary
strings. Thus numbers need to be converted to strings and back. The
keyword ``name`` is special in the sense that it is a default key that
is recognized by a number of other modules (including the :meth:`__repr__`
method governing array output) to briefly describe the data.

The keyword values can be retrieved using :meth:`getKey`::

    >>> a.getKey("name")


File I/O
========


Opening and closing a file
--------------------------

Let us see how we can open a file. First define some variable names
that represent the file names of the files we are going to use::

    >>> filename_lofar = LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"

We can create a new file object, using the :meth:`open` method ,
which is an interface to the LOFAR CRTOOLS datareader class.

The following will open a data file and return a :class:`DataReader`
object::

    >>> datafile = open(filename_lofar)

The associated filename can be retrieved with::

    >>> datafile["FILENAME"]

The file will be automatically closed (and the object will be
destroyed), whenever the :class:`open` object is deleted, e.g. with
``datafile = 0``.


Setting and retrieving metadata
-------------------------------

Now we need to access the metadata in the file.

This can be done by providing a keyword to the datafile object, e.g. type::

    >>> datafile["FILENAME"]

to obtain the filename of the datafile object. A list of valid
keywords can be obtained by::

    >>> datafile.keys()

Note, that the results are returned as PythonObjects. Hence, this
makes use of the power of Python with automatic typing. For example::

    >>> datafile["FREQUENCY_RANGE"]

actually returns a list of frequency ranges.

Just for convenience let us define a number of variables that contain
essential parameters (we will later actually use different ones which
are automatically stored in the datafile object)::

    >>> obsdate = datafile["TIME"][0]          # Timestamp of the first event
    >>> filesize  = datafile["DATA_LENGTH"][0] # number of samples per dipole
    >>> blocksize = datafile["BLOCKSIZE"]      # Number of samples per block
    >>> nAntennas = datafile["NOF_DIPOLE_DATASETS"] # Number of antennas
    >>> antennaIDs = datafile["DIPOLE_NAMES"]  # List of antenna IDs
    >>> selectedAntennas = datafile["SELECTED_DIPOLES"] # List of selected antennas
    >>> nofSelectedAntennas = datafile["NOF_SELECTED_DATASETS"] # Number of selected antennas
    >>> fftlength = datafile["FFTSIZE"] # Length of an FFT block
    >>> sampleFrequency = datafile["SAMPLE_FREQUENCY"][0] # Sample frequency
    >>> maxblocksize=min(filesize,1024*1024); # Maximum blocksize we will use
    >>> nBlocks = filesize/blocksize; # Number of blocks

    obsdate => 1214929202
    filesize => 2048000
    blocksize => 1024
    nAntennas => 1
    antennaIDs => ['001001009']
    selectedAntennas => ['001001009']
    nofSelectedAntennas => 1
    fftlength => 513
    sampleFrequency => 200000000.0
    nBlocks => 2000

As you can see the date is expressed in a not well interpretable
format, i.e. the nr. of seconds after January 1st, 1970.  To get a
human readable version of the observing date use the python time
module::

    >>> import time
    >>> time.asctime(time.localtime(obsdate))
    'Thu Feb 11 23:09:14 2010'

Fortunately, you do not have to do this all the time, since all the
parameters will be read out at the beginning and will be stored as
attributes to the file object.

To set the data attributes you can simply use the same attribute
naming as mentioned above, e.g.::

    >>> datafile["BLOCKSIZE"] = 2048



Reading in data
---------------

The next step is to actually read in data. This is done with the
:meth:`read` method.

Before this is done, one has to allocate the memory in which the data
is put. Although this requires one to program carefully and understand
the data structure, this improves speed and efficiency.

Let's first create a :type:`FloatArray` of the correct dimensions,
naming it ``fxdata`` and setting the unit to counts::

    >>> fxdata = hArray(float,[nofSelectedAntennas,blocksize],name="E-Field")
    >>> fxdata.setUnit("","Counts")

This is now a large vector filled with zeros.

Now we can read in the raw timeseries data, either by using
:func:`datafile.read` and a keyword, or use the :func:`read` method of
arrays, e.g.::

    >>> datafile.read("TIMESERIES_DATA", fxdata)

or::

    >>> fxdata.read(datafile, "TIMESERIES_DATA")

The types of data that can be read are `TIMESERIES_DATA`, `FFT_DATA`,
`FREQUENCY_DATA`, and `TIME_DATA`.



Basic plotting
==============

In order to plot the data we use the :mod:`matplotlib` package. This
can be loaded by using the following command::

    >>> import matplotlib.pyplot as plt

however, this should have already been done when loading the
:mod:`pycrtools`. Besides the plotting commands provided by
:mod:`matplotlib`, this plotting functionality is also available from
the :class:`hArrays`.


Matplotlib
----------

.. [under construction]

Depending on the system you are working on you may have to create a
window in which the plots are shown. This is done by::

    >>> plt.show()

Now we can use the following plotting commands to show the average
spectra for two antennas::

    >>> plt.subplot(1,2,1)
    >>> plt.title("Average spectrum for two antennas")
    >>> plt.semilogy(frequencies.vec(), avspectrum[0].vec())
    >>> plt.semilogy(frequencies.vec(), avspectrum[1].vec())
    >>> plt.xlabel(avspectrum.getKey("name")+" ["+avspectrum.getUnit()+"]")
    >>> plt.ylabel(frequencies.getKey("name")+" ["+frequencies.getUnit()+"]")

Note that the array entries need to be converted to vectors (using the
:func:`vec` method) when they are given as arguments to the plotting
command.

To plot the time series of the entire dataset, we first read in all
samples from all antennas::

    >>> datafile["block", "blocksize"] = (0, maxblocksize)
    >>> timeall = datafile["Time"]
    >>> fxall = datafile["Fx"]

and then we plot it::

    >>> plt.subplot(1,2,2)
    >>> plt.title("Time series of antenna 0")
    >>> plt.plot(timeall.vec(), fxall[0].vec())
    >>> plt xtitle("Time [$\\mu$s]")
    >>> plt ytitle("Electric Field [ADC counts]")

.. [resulting plot of the code above]

If you want to plot a linear plot use :func:`plt.plot`, for a loglog
plot you use :func:`plt.loglog` and for a log-linear plot you can use
:func:`plt.semilogx` or :func:`plt.semilogy`, depending on which axis
needs to be logarithmic.


Plotting using the :class:`hArray` plotting methods
---------------------------------------------------

.. [under construction]

Another way of producing plots is to use the plot method of the
:class:`hArray` class::

    >>> avspectrum.par.xvalues = frequencies
    >>> avspectrum.par.title = "Average spectrum"
    >>> avspectrum[0].plot(logplot="y")

.. [resulting plot of the code above]

This creates a semilog-plot with appropriate labels and units (if
provided beforehand).

You can either provide the parameters directly (has precedence), or
set the plotting parameters and attributes to the :class:`par` class
of the array, e.g.::

   >>> array.par.xvalues = x_vector
   >>> array.plot()

If the array is in looping mode, multiple curves are plotted in a
single window. Hence,::

   >>> avspectrum.par.logplot = "y"
   >>> avspectrum[...].plot(legend=datafile.antennas)

.. [resulting plot of the code above]

will simply plot all spectra of all antennas (= highest array index)
in the array.

The available parameters, used in the :class:`hArray.par` class, are:

  =============== ==================================================
  ``xvalues``     An array with corresponding x values.
                  If ``None``, integer numbers from 0 to the length of the array are used.
  ``xlabel``      The x-axis label.
                  If not specified, use the ``name`` keyword of the array.
                  Units are added automatically.
  ``ylabel``      The y-axis label.
                  If not specified, use the ``name`` keyword of the array.
                  Units are added automatically.
  ``xlim``        Tuple with minimum and maximum values for the x-axis.
  ``ylim``        Tuple with minimum and maximum values for the y-axis.
  ``title``       A title for the plot
  ``clf``         If ``True`` (default) clear the screen before plotting.
                  If ``False`` plots are composed with multiple lines from different arrays.
  ``logplot``     Can be used to make log-log or semi-log plots:

                  * "x" -> semilog plot in x
                  * "y" -> semilog plot in y
                  * "xy" -> loglog plot
  =============== ==================================================

.. [in preparation]


Some use cases
==============

.. [in preparation]


Quality check of time series data
---------------------------------

For an automatic pipeline it is essential to check whether the data is
of good quality, or whether one needs to flag particular
antennas. Here we demonstrate a simple, but effective way to do this.

The basic parameters to look at are the mean value of the time series
(indicating potential DC offsets), the root-mean-square (RMS)
deviation (related to the power received), and the number of peaks in
the data (indicating potential RFI problems).

For cosmic ray data, we expect spikes and peaks to be in the middle of
a trace, so we will just look at the first or/and last quarter of a
data set and set the block size accordingly::

    >>> blocksize = min(filesize/4, maxblocksize)

We will then read this block of data into an appropriately sized data array::

    >>> dataarray = datafile.set("blocksize",blocksize).set("block",3)["Voltage"]

The array now contains all the measured voltages of the selected
antennas in the file.

First we calculate the mean over all samples for each antennas (and
make use of the looping through the Ellipsis (...) object)::

    >>> datamean = dataarray[...].mean()
    Vector(float, 16, fill=[7.17281,-7.79052,-1.97656,-0.283491,5.22623,-1.53358,0.440468,-6.22257,1.53373,4.74461,6.70252,-4.63748,-4.67412,7.92508,0.101472,-4.59787])

Similarly we get the RMS (where we spare the algorithm from recalculating the
mean, by providing it as input - actually a list of means)::

    >>> dataRMS = dataarray[...].stddev(datamean)
    Vector(float, 16, fill=[10.4218,10.3521,10.1878,11.9229,10.0157,11.8848,9.56963,10.0076,10.7717,10.1781,10.3913,10.879,9.58076,10.2835,10.4557,9.78504])

and finally we get the total number of peaks 5 sigma above the noise::

    >>> dataNpeaks = dataarray[...].countgreaterthanabs(dataRMS*5)
    Vector(int, 1, fill=[0])

To see whether we have more peaks than expected, we first calculate
the expected number of peaks for a Gaussian distribution and our
``blocksize``, as well as the error on that number::

    >>> Npeaks_expected = funcGaussian(5, 1,0)*blocksize
    0.04909742525458545

    >>> Npeaks_error = sqrt(Npeaks_expected)
    0.22157938815373926

So, that we can get a normalized quantity::

    >>> G = (Npeaks_detected - Npeaks_expected)/Npeaks_error

which should be of order unity if we have roughly a Gaussian
distribution. If it is much larger or less than unity we have more or
less peaks than expected and the data is clearly not Gaussian noise.

We do the calculation of G using our STL vectors (even though speed is not
of the essence here)::

    >>> dataNonGaussianity = Vector(float, nAntennas)
    >>> dataNonGaussianity.sub(dataNpeaks, Npeaks_expected)
    >>> dataNonGaussianity /= Npeaks_error

The next stept is to make a nice table of the results and check
whether these parameters are within the limits we have imposed (based
on empirical studies of the data).

To ease the operation we combine all the data into one python array
(using the zip function - zip, as in zipper)::

    >>> dataproperties = zip(selectedAntennas,datamean,dataRMS,dataNpeaks,dataNonGaussianity)

which is a rather nasty collection of numbers. So, we print a nice
table (restricting it to the first 5 antennas):

    >>> for prop in dataproperties[0:5]: print "Antenna {0:3d}: mean={1: 6.2f}, rms={2:6.1f}, npeaks={3:5d}, spikyness={4: 7.2f}".format(*prop)
    Antenna   0: mean=  6.59, rms= 135.7, npeaks=    0, spikyness=  -0.22

Clearly this is a spiky dataset, with only one antenna not being
affected by too many peaks (which is in fact not the case for the
first block of that dataset).

To check automatically whether all parameters are in the allowed
range, we can use a little python helper function, using a python
"dict" as database for allowed parameters::

    >>> qualitycriteria = {"mean":(-15,15),"rms":(5,15),"spikyness":(-3,3)}
    >>> CheckParameterConformance(dataproperties[0], {"mean":1,"rms":2,"spikyness":4}, qualitycriteria)

The first parameter is just the list of numbers of the mean, RMS,
etc. of one antenna we created above. The second parameter is a dict,
describing which parameter to find at which position in the input
list, and the third parameter is yet another dict specifying for each
parameter the range of allowed upper and lower values. The result is a
list of parameter names, where the antennas failed the test. The list
is empty if the antenna passed it.

Finally, we do not want to do this manually all the time. So, a little
python function is available, that does the quality checking for you
and returns a list with failed antennas and their properties::

    >>> badantennalist = CRQualityCheck(qualitycriteria, datafile=datafile, dataarray=dataarray, blocksize=blocksize, verbose=False)
    Block=     0, Antenna   0: mean=  6.59, rms= 141.7, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=     1, Antenna   0: mean=  6.63, rms= 138.8, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=     2, Antenna   0: mean=  6.62, rms= 141.4, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=     3, Antenna   0: mean=  6.59, rms= 135.7, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=     4, Antenna   0: mean=  6.61, rms= 140.9, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=     5, Antenna   0: mean=  6.60, rms= 137.5, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=     6, Antenna   0: mean=  6.62, rms= 143.1, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=     7, Antenna   0: mean=  6.62, rms= 141.6, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=     8, Antenna   0: mean=  6.63, rms= 135.0, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=     9, Antenna   0: mean=  6.63, rms= 146.0, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    10, Antenna   0: mean=  6.63, rms= 140.9, npeaks=    1, spikyness=   4.29   ['rms', 'spikyness']
    Block=    11, Antenna   0: mean=  6.60, rms= 142.4, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    12, Antenna   0: mean=  6.61, rms= 143.0, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    13, Antenna   0: mean=  6.61, rms= 141.6, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    14, Antenna   0: mean=  6.59, rms= 143.3, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    46, Antenna   0: mean=  6.61, rms= 142.5, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    47, Antenna   0: mean=  6.60, rms= 136.7, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    48, Antenna   0: mean=  6.59, rms= 141.1, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    49, Antenna   0: mean=  6.60, rms= 138.7, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    50, Antenna   0: mean=  6.63, rms= 143.8, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    51, Antenna   0: mean=  6.59, rms= 139.5, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    52, Antenna   0: mean=  6.58, rms= 141.5, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    53, Antenna   0: mean=  6.64, rms= 145.5, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    54, Antenna   0: mean=  6.61, rms= 143.1, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    55, Antenna   0: mean=  6.60, rms= 145.5, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    56, Antenna   0: mean=  6.62, rms= 135.0, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    57, Antenna   0: mean=  6.60, rms= 146.0, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    58, Antenna   0: mean=  6.60, rms= 143.3, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    59, Antenna   0: mean=  6.60, rms= 135.8, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    60, Antenna   0: mean=  6.59, rms= 141.6, npeaks=    0, spikyness=  -0.22   ['rms']
    Block=    61, Antenna   0: mean=  6.64, rms= 135.6, npeaks=    0, spikyness=  -0.22   ['rms']

    >>> badantennalist[0]
    [0, 0, (6.5949006782945734, 141.70452128542746, 0, -0.22157938815373929), ['rms']]

(first the antenna number, then the block, then a list with the
*mean*, *rms*, *npeaks*, and *spikyness*, and finally the failed
fields)

Note, that this function can be called with ``file=None``. In this
case the data provided in the datararray will be used.


Finding peaks in a vector
-------------------------

In the following example we try to find peaks in some artificially
generated data.

First we make a test time series data set for 4 antennas::

    >>> data = hArray(float, [4,512], name='Random series with peaks')

and fill it with random data that have arbitrary offsets::

    >>> data.random(-1024,1024)
    >>> data[...] += Vector([-128.,256., 385.,-50.])

Then we put some peaks at location 2-3, 32, and 64-67 in each of the 4
data sets::

    >>> for i in range(4):
    ...     data[i,[2,3,32,64,65,67],...] = Vector([4096.,5097,-4096,4096,5099,3096])

Now, we reverse-engineer and try finding all 5 sigma peaks::

    >>> nsigma = 5

First make a scratch array that will contain the locations of the
peaks.  A location is actually a 'slice' in the array, i.e. given by
its beginning and ending position (plus one). The length of the return
array must be pre-allocated and should be long enough to contain all
peaks (at maximum as long as the input array)::

    >>> datapeaks = hArray(int, [4,5,2], name="Location of peaks")

Now, retrieve the mean and RMS of the array to set the thresholds
above and below which one considers a peak to be significant::

    >>> datamean = data[...].mean()
    >>> datathreshold2 = data[...].stddev(datamean)
    >>> datathreshold2 *= nsigma
    >>> datathreshold1 = datathreshold2*(-1)
    >>> datathreshold1 += datamean
    >>> datathreshold2 += datamean

Finally, we determine the input parameters for the search algorithm::

    >>> maxgap = Vector(int, len(datamean), fill=10)

The gap vector tells the algorithm how many samples can be between two
values that are above threshold, so that the two peaks are considered
as one::

    >>> minlength = Vector(int, len(datamean), fill=1)

A minimum length can be specified to exclude peaks that consists of
only a single or a few values (no relevant here, so set to 1, i.e. all
peaks are relevant). Then call :func:`hFindSequenceOutside` (or
:func:`hFindSequenceBetween`, :func:`hFindSequenceGreatererThan`,
:func:`hFindSequenceLessEqual` ...)::

    >>> npeaks = datapeaks[...].findsequenceoutside(data[...], datathreshold1, datathreshold2, maxgap, minlength)

The return value is the number of peaks found (in each row of the data set)::

    >>> npeaks
    Vector(int, 4, fill=[3,3,3,3])

And the slices are actually contained in the return vector for each antenna::

    >>> datapeaks.mprint()
    [2,4,32,33,64,66,0,0,0,0]
    [2,4,32,33,64,66,0,0,0,0]
    [2,4,32,33,64,66,0,0,0,0]
    [2,4,32,33,64,66,0,0,0,0]



.. Fourier transforms (FFT) and cross correlation
.. ----------------------------------------------



.. Coordinate transformation
.. -------------------------

