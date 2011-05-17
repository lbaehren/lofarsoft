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

As an example we create a new array of the dimensions of a

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

Let us see how we can open a file. First define a filename, e.g.::

    >>> filename_sun=LOFARSOFT+"/data/lopes/example.event"
    >>> filename_lofar_big=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
    >>> filename_lofar=LOFARSOFT+"/data/lofar/trigger-2010-02-11/triggered-pulse-2010-02-11-TBB1.h5"
    >>> filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"

We can create a new file object, using the :class:`crfile` class,
which is an interface to the LOFAR CRTOOLS datareader class.

The following will open a data file and return a :class:`DataReader`
object::

    >>> datafile=crfile(filename_lofar).set("blocksize",1024*2)

The associated filename can be retrieved with::

    >>> datafile.filename

The file will be automatically closed (and the :class:`DataReader`
object will be destroyed), whenever the :class:`crfile` object is
deleted, e.g. with ``file=0``.


Setting and retrieving metadata
-------------------------------

Now we need to access the metadata in the file. This is can be done in
multiple ways. One way is by using the get method. This method
actually calls the function :func:`hFileGetParameter` defined in the
C++ code.

To find the observatory we can type::

    >>> datafile.get("observatory")
    'LOFAR'

There are more keywords, of course. A list of implemented parameters
we can access is obtained by::

    >>> datafile.get("help")
    hFileGetParameter - available keywords: nofAntennas, nofSelectedChannels, nofSelectedAntennas, nofBaselines, block, blocksize, stride, fftLength, nyquistZone, sampleInterval, referenceTime, sampleFrequency, antennas, selectedAntennas, selectedChannels, positions, shift, increment, frequencyValues, frequencyRange, Date, Observatory, Filesize, presync, TL, LTL, EventClass, SampleFreq, StartSample, AntennaIDs, SAMPLE_OFFSET, SAMPLE_NUMBER, TIME, keywords, help

Note, that the results are returned as PythonObjects. Hence, this
makes use of the power of Python with automatic typing. For, example::

    >>> datafile.get("frequencyRange")

actually returns a vector.


A second way do retreive data is to use square brackets, since
``datafile[key]`` is equivalent to ``datafile.get(key)``::

    >>> datafile["blocksize"]

Just for fun let us define a number of variables that contain
essential parameters (we will later actually use different ones which
are automatically stored in the datafile object)::

    >>> obsdate   =datafile["Date"]
    >>> filesize  =datafile["Filesize"]
    >>> blocksize =datafile["blocksize"]
    >>> nAntennas =datafile["nofAntennas"]
    >>> antennas  =datafile["antennas"]
    >>> antennaIDs=datafile["AntennaIDs"]
    >>> selectedAntennas=datafile["selectedAntennas"]
    >>> nofSelectedAntennas=datafile["nofSelectedAntennas"]
    >>> fftlength =datafile["fftLength"]
    >>> sampleFrequency =datafile["sampleFrequency"]
    >>> maxblocksize=min(filesize,1024*1024);
    >>> nBlocks=filesize/blocksize;

    obsdate => 1265926154
    filesize => 132096
    blocksize => 2048
    nAntennas => 16
    antennas => Vector(int, 16, fill=[128002016,128002017,128002018,128002019,128002020,128002021,128002022,128002023,128003024,128003025,128003026,128003027,128003028,128003029,128003030,128003031])
    antennaIDs => Vector(int, 16, fill=[128002016,128002017,128002018,128002019,128002020,128002021,128002022,128002023,128003024,128003025,128003026,128003027,128003028,128003029,128003030,128003031])
    selectedAntennas => Vector(int, 16, fill=[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15])
    nofSelectedAntennas => 16
    fftlength => 1025
    sampleFrequency => 200000000.0
    maxblocksize => 132096
    nBlocks => 64


To get a readable version of the observing date use the python time
module::

    >>> import time
    >>> time.asctime(time.localtime(obsdate))
    'Thu Feb 11 23:09:14 2010'


Fortunately, you do not have to do this all the time, since all the
parameters will be read out at the beginning and will be stored as
attributes to the file object.
They will be updated whenever you do a ``file.set(key,value)``, however,
assigning a new value to the attribute will **not** automatically change
the parameter in the file. For this, you have to use the the 'set'
method, which is an implementation of the :func:`hFileSetParameter`
function. E.g. changing the blocksize we already did before. This is
simply::

    >>> datafile.set("blocksize",2048);

again the list of implemented keywords is visible with using::

    >>> datafile.set("help",0);

Here the listed keywords actually start with capital letters, however,
to spare you some annoyance, you can use a spelling which starts with
either an upper or a lower case letter.

Another useful feature: The set method actually returns the ``crfile``
object itself. Hence, you can append multiple set commands after each
other::

    >>> datafile.set("block",2).set("selectedAntennas",[0,2,3]);

Alternatively you can also use square brackets::

    >>> datafile["selectedAntennas"]=[0,2]

but then it is not possible to append multiple set commands in one
line, so you need to provide lists of keywords and list of values,
like::

    >>> datafile["blocksize","selectedAntennas"]=[2048,[0,2]]
    >>> datafile["blocksize","selectedAntennas"]

Note, that we have now reduced the number of antennas to two: namely
antenna 0 and 2 and the number of selected antennas is::

    >>> datafile["nofSelectedAntennas"]

However, in the follwing we want to work on all antennas again, so we
do::

    >>> datafile.set("block",0).set("selectedAntennas",range(nAntennas))


Reading in data
---------------

The next step is to actually read in data. This is done with the read
method (accessing :func:`hFileRead`). The data is read flatly into a
1D vector. This is also true if multiple antennas are read out at
once. Here simply the data from the antennas follow each other.

Also, by default, memory allocation of the vectors has to be done in
Python before calling any of the functions. This improves speed and
efficiency, but requires one to program carefully and to understand
the data structure.

First we create a FloatArray of the correct dimensions, naming it
``Voltage`` and setting the unit to counts::

    >>> fxdata=hArray(float,[nofSelectedAntennas,blocksize],name="E-Field").setUnit("","Counts")

This is now a large vector filled with zeros.

Now we can read in the raw time series data, either using
:func:`datafile.read` and a keyword, or actually better, use the
:func:`read` method of arrays, as they then store filename and history
information in the array.

Currently implemented keywords for reading data fields are: *Fx*,
*Voltage*, *FFT*, *CalFFT*,*Time* and *Frequency*.

So, let us read in the raw time series data, i.e. the electric field
in the antenna as digitized by the ADC. This is provided by the
keyword *Fx* (meaning f(x))::

    >>> fxdata.read(datafile,"Fx")
    'hArray(float, [16, 2048], fill=[16,17,10,18,17,6,16,17,-1,0,...,-9,-12,-25,-13,-7,-15,-5,3,-2,1], name="E-field") # len=32768 slice=[0:32768])'

and the vector is filled with time series data from the data
file. Note that we had to use the .vec method for the array, since
:func:`datafile`.read does not accept arrays (since it cannot handle
C++ iterators).

Now, you can access the individual antennas as single vectors through
slicing::

    >>> ant0data=fxdata[0].vec()

If you do not have yet a pre-existing array into which you want to read
data, you can automatically create one, using the square brackets
syntax already known from retrieving the file header keywords. So, for
example, to get the `x`-Axis we create a second vector::

    >>> times=datafile["Time"]

.. note::
    you can also create an empty array with the same properties
    and dimensions, but without reading data into the array, by preceding
    the keyword with the word ``empty``, i.e. ``times=datafile['emptyTime']``.

In the square bracket notation python will actually set the name and
units of the data accordingly. So, let us have the time axis in
microseconds, by using :func:`setUnit`::

    >>> times.setUnit("\\mu","")

We do the same now for the frequency axis, which we convert to *MHz*::

    >>> frequencies=datafile["Frequency"].setUnit("M","")

We can calculate the average spectum of the data set for one antenna,
by looping over all blocks. Here we do not use the square bracket
notation, since we want to read the data repeatedly into the same
memory location!::

    >>> fftdata=datafile["emptyFFT"]
    >>> avspectrum=hArray(float,dimensions=fftdata,name="average spectrum")
    >>> for block in range(nBlocks):
    ...     fftdata.read(datafile.set("Block",block),"FFT").none()
    ...     avspectrum[...].spectralpower(fftdata[...])

.. note::
   The ``.none()`` method is appended to suppress unwanted output
   in generating the tutorial, when an operation returns an array or
   vector.

Alternatively you can use the method::

    >>> avspectrum.craveragespectrum(datafile)

which does this automatically.

