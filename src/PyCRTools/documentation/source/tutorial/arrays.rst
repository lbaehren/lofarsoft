.. _arrays:

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
    Vector(int, 9, fill=[0,1,2,3,4,5,6,7,8])

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
    >>> tmp_array
    hArray(int, [3, 3], fill=[0,2,4,6,8,10,12,14,16]) # len=9 slice=[0:9])
    >>> a
    hArray(int, [3, 3], fill=[0,1,2,3,4,5,6,7,8]) # len=9 slice=[0:9])

This could also be done calling the function ``hMul(tmp_array,a,2)``,
rather than the corresponding method.

An important constraint is that all these functions or methods only
work with either vector or array classes, a mix in the parameters
between vectors and arrays is currently not supported.


Changing dimensions
-------------------

The dimensions can be obtained and set, using the :meth:`shape` and
:meth:`setDim` methods. If the length of the underlying vector changes
due to a change in the dimensions, the vector will be resized and
padded with zeros, if necessary::

    >>> a.shape()

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
    hArray(int, [3, 3], fill=range(9)) # len=9 slice=[0:9])

The vector followed by a single number in square brackets
will *in principle* obtain the first column of the array::

    >>> a[0]
    hArray(int, [3, 3], fill=[0,1,2]) # len=9 slice=[0:3])

It says *in principle*, because the only thing which that command does is
to return a new ``hArray`` Python object, which will point to the same
data vector, but contain a different data slice which is then returned
whenever a method tries to operate on the vector::

    >>> a[0].vec()

This retrieves a copy of the data, since assigning a sub-slice of a
vector to another vector actually requires copying the data - as
vectors do not know about slicing (yet). Use one-dimensional arrays if
what you want to have are reference to a slice only.

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
two. This is the reason for using ``[0]:number_of_indices`` rather than
just ``0:number_of_indices``.

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

    >>> for i in range(a.shape()[0]):
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
