.. _cpp_level:

======================
Low level C++ routines
======================

This document should describe how to write low level C++ routines.
How they are parsed to generate *Python* bindings etc.


Code development
================

The C++ code for the pycrtools is located in the directory
``$LOFARSOFT/src/PyCRTools``. It consists of files
providing core functionality (``core.cc`` , ``core.h``, ``hfppnew*``)
and several modules, recognizable by their ``m`` prefix, consisting of
a ``.cc`` and ``.h`` files that contain the main functionality of the
pycrtools:

* mArray
* mBindings
* mFFT
* mFFTW
* mFilter
* mFitting
* mImaging
* mIO
* mMath
* mModule
* mNumpy
* mRF
* mTBB
* mVector

..
   Additional description of modules.


Functions
---------

Naming
~~~~~~

By convention, function names are written in camelcase with the
first character being an "h" (e.g. hFunctionName).  After building the
code the function will exist in two varieties:

  ================= ====================================================
  ``hFunctionName`` a stand alone version of the function,
                    e.g. ``hFunction(out_par, in_par)``.
  ``functionname``  a method function on the output parameter
                    (by default this is the first parameter) of the
                    function. As the output parameter is the parent of
                    the method it is not a part of the parameter list,
                    e.g. ``num.function(in_par)``.
  ================= ====================================================




Parameters
~~~~~~~~~~

To distinguish the parameters from function names it is preferred to
use lowercase names, and use underscores as separators when using
parameter names that consist of multiple words.

When a parameter serves as an output parameter it is used as the first
parameter of the parameter list.

Vectors or arrays should preferably be addressed by using iterators
(see :ref:`iterators`).


Scalar types
~~~~~~~~~~~~

Within the functions it is encouraged to use as much as possible the
following scalar types:

  ============ ==================================================
  ``hInteger`` for integers (``int32_t`` or ``int64_t``,
               depending on the architecture that is used)
  ``hNumber``  for floats (``double``)
  ``hComplex`` for complex numbers (``std::complex<HNumber>``)
  ``hBool``    for booleans (``bool``)
  ``hString``  for strings (``std::string``)
  ``hPointer`` for void pointers (``void*``)
  ============ ==================================================


Non-scalar types
~~~~~~~~~~~~~~~~

.. _hArray:

hArray
++++++

Besides the scalar types there are also an additional type called
``hArray``. This is actually a container class that supports
multi-dimensional storage of variables and slicing.


.. _iterators:

Iterators
+++++++++

In order to prevent unnecessary copying (large amounts) of data it is
highly recommended to make use of iterators instead of vectors.  This
is done by passing both start and end iterators to the
function. Furthermore,this has the benefit that STL vectors as well as
CASA and numpy vectors can be used (with the use of template types) using the
same interface type since the addressing is done in a similar way.

When addressing a vector or hArray, instead of using its name,
e.g. ``data``, the names for the start and end iterators are ``data``
and ``data_end`` should be provided.

.. _Wrappers:

Wrappers
--------

In order to expose the implemented C++ functions to Python
these functions need to be wrapped. This is done by a set of
preprocessor commands, described in the following section, which make
it possible to automatically generate wrappers for templated
functions.

.. _Wrapper_header_and_footer:

Header and footer
~~~~~~~~~~~~~~~~~

The function that is wrapped is surrounded by a wrapper header of the
following form:

.. code-block:: c

   //-----------------------------------------------------------------------
   //$DOCSTRING: Description of the function
   //$COPY_TO HFILE START --------------------------------------------------
   #define HFPP_FUNC_NAME hFunctionName
   //-----------------------------------------------------------------------
   ...
   //$COPY_TO END ----------------------------------------------------------
   /*!
     \brief $DOCSTRING
     $PARDOCSTRING
   */

describing the function characteristics and containing the function documentation, and a footer:

.. code-block:: c

   //$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

describing where the wrapper definition ends.

The preprocessor command ``$COPY_TO FILE START`` and
``$COPY_TO FILE END`` define that the lines between these commands
should be copied to a file. The name of this file is directly placed
after the ``$COPY_TO`` command. Instead of a filename, one can also
use a symbolic name which should be defined at the top of the source
file (e.g. via ``//$FILENAME: FILE=mModuleName.def.h``).

Furthermore the filetype needs to be specified, since the same
function and parameters definitions need to produce different pieces
of code in a header file (``.h``), or in the file where the python
wrappers will be generated, or in the implementation file (``.cc``)
where the actual code for wrappers is produced. For ``.cc`` files this
is done via:

.. code-block:: c

   #undef HFPP_FILETYPE
   //-----------------------
   #define HFPP_FILETYPE CC

The part that is denoted by ellipsis (``...``) is filled with
preprocessor commands specific for generating the wrapper that specify
the syntax of the function, its parameters, and optionally some
special wrapper specific definitions which are described in the
following sections. These commands are described in the following
sections.


.. _Wrapper_function_name_definition:

Function name definition
~~~~~~~~~~~~~~~~~~~~~~~~

With the preprocessor command:

.. code-block:: c

   #define HFPP_FUNC_NAME hFunctionName

the function name is defined. This can be used to replace multiple
occurrences of the function name within the implementation of the
wrapper and function.  This function name is only defined between the
header and footer of the function in which it is defined, outside this
environment the variable is not defined!


.. _Wrapper_function_definition:

Function definition
~~~~~~~~~~~~~~~~~~~

The syntax of the function and its return value is defined by the
following preprocessor command:

.. code-block:: c

   #define HFPP_FUNCDEF(FUNC_TYPE)(FUNC_NAME)(FUNC_DOC)(FUNC_DIM)(FUNC_IT)(FUNC_PASS)

This preprocessor command has 6 mandatory parameters that contain:

  ============== ===============================================================
  ``FUNC_TYPE``  The type of the return value of the function.
  ``FUNC_NAME``  The name of the function. This should be ``HFPP_FUNC_NAME``,
                 which, after preprocessing, gets replaced by the value of
                 ``HFPP_FUNC_NAME``.
  ``FUNC_DOC``   Short quoted string describing the function. This should be
                 equal to ``"$DOCSTRING"`` which, after preprocessing, gets
                 replaced by the value of ``$DOCSTRING``.
  ``FUNC_DIM``   Describes if the function returns a scalar (use
                 ``HFPP_PAR_IS_SCALAR``), or a non-scalar (use
                 ``HFPP_PAR_IS_VECTOR``).
  ``FUNC_IT``    Describes how the function expects its input.
                 If the function is a scalar leave this entry empty.
                 If the function is a non-scalar, this entry describes how it is
                 passed:

                 * when passing an STL vector use ``STL``.
                 * When passing start and end iterators use ``STDIT``.
  ``FUNC_PASS``  Describes if the return value should be passed as a value
                 (use ``HFPP_PASS_AS_VALUE``) or as a reference
                 (use ``HFPP_PASS_AS_REFERENCE``).
  ============== ===============================================================


.. _Wrapper_parameter_definition:

Parameter definition
~~~~~~~~~~~~~~~~~~~~

The syntax of the parameters is defined by the following preprocessor commands:

.. code-block:: c

   #define HFPP_PARDEF_0(PAR_TYPE)(PAR_NAME)(PAR_DEFAULT)(PAR_DOC)(PAR_DIM)(PAR_IT)(PAR_PASS)
   #define HFPP_PARDEF_1(PAR_TYPE)(PAR_NAME)(PAR_DEFAULT)(PAR_DOC)(PAR_DIM)(PAR_IT)(PAR_PASS)
   #define HFPP_PARDEF_2(PAR_TYPE)(PAR_NAME)(PAR_DEFAULT)(PAR_DOC)(PAR_DIM)(PAR_IT)(PAR_PASS)
   ...
   #define HFPP_PARDEF_9(PAR_TYPE)(PAR_NAME)(PAR_DEFAULT)(PAR_DOC)(PAR_DIM)(PAR_IT)(PAR_PASS)

where the number at the end of the ``HFPP_PARDEF_`` command represents the location
of the parameter in the function declaration, starting with 0.

This preprocessor command has 7 mandatory parameters that contain:

  =============== ===============================================================================
  ``PAR_TYPE``    The type of the parameter.
  ``PAR_NAME``    The name of the parameter. For convenience the same naming should
                  be used as in the implementation. Non-scalar parameters that are passed
                  as iterators are treated as one wrapper parameter. The name of the parameter
                  is that of the start iterator.
  ``PAR_DEFAULT`` The default value of the parameter. As this is not implemented yet,
                  this should be empty.
  ``PAR_DOC``     Short quoted string describing the parameter.
  ``PAR_DIM``     Describes if the parameter is a scalar (use ``HFPP_PAR_IS_SCALAR``)
                  or a non-scalar (use ``HFPP_PAR_IS_VECTOR``).
  ``PAR_IT``      If the parameter is a scalar leave this entry empty.
                  If the function is a non-scalar, this entry describes how the it is passed:

                  * when passing an STL vector use ``STL``.
                  * when passing start and end iterators use ``STDIT``.
  ``PAR_PASS``    Describes if the parameter is passed as a value (use
                  ``HFPP_PASS_AS_VALUE``) or as a reference
                  (use ``HFPP_PASS_AS_REFERENCE``).
  =============== ===============================================================================


.. _Wrapper_special_types:

Special types
~~~~~~~~~~~~~

Within the wrapper definition the following wrapper types can be used
besides the types that are available in C/C++:

  ======================= ======================================================
  ``HFPP_VOID``           When a function is of type void, one should use this
                          wrapper type in the wrapper function definition.
  ``HFPP_TEMPLATED_TYPE`` When function templating is used, the templated type
                          can be refered to as ``HFPP_TEMPLATED_TYPE``. In the
                          case where multiple templates are used, these types
                          are refered to as ``HFPP_TEMPLATED_1``,
                          ``HFPP_TEMPLATED_2``, etc., within the wrapper
                          definition.
  ======================= ======================================================


.. _Wrapper_documentation:

Documentation
~~~~~~~~~~~~~

Within the wrapper definition of a function there are 3 locations to
document the code. This documentation is both used for the
documentation that is generated by Doxygen as well as for Sphinx.

Function description:
  A short description of the function is written after the
  ``$DOCSTRING`` wrapper variable. This is defined at the top of the
  wrapper header. The description is *not* written between quotes.

Parameter description:
  This is given as a quoted string in the 3rd parameter of the
  ``HFPP_PARDEF_n`` command. The description of all parameters of a
  function is stored in the ``$PARDOCSTRING`` wrapper variable.

Additional documentation:
  Further documentation such as an extended description of the
  function or an example of how the function is called is given in the
  comment block between ``/*!`` and ``*/`` at the end of the wrapper
  header.

A more elaborate description of how to write documentation can be
found :ref:`here <documenting>`.


.. _Wrapper_additional_functionality:

Additional functionality
~~~~~~~~~~~~~~~~~~~~~~~~

.. _Wrapper_iterating:

Iterating over functions
++++++++++++++++++++++++


When a set of functions has the same interface and a similar
implementation, a single wrapper for all elements in this set of
functions can be created by using the ``ITERATOR`` wrapper command.
This is accomplished by putting the following command above the
wrapper header definition of the function which has to be iterated.

.. code-block:: c

   //$ITERATE <iterator_variable_name> <iteration_list>

where ``<iteration_list>`` is a space separarted list of names over
which is iterated and ``<iterator_variable_name>`` is the name of
the placeholder which gets substituted by the current element name in the
iterator list. Within the wrapper and function definition this name
has a \verb|$| prefix e.g. ``{$ITERVARIABLE}``. %
If the iterator name should be in uppercase (or lowercase) this can be
done by putting ``!CAPS`` (or ``!LOW``) immediately after the
iterator variable name (e.g. if ``ITERVARIABLE`` is the iterator
variable name use ``{$ITERVARIABLE!CAPS}`` (or ``{$ITERVARIABLE!LOW}``)
for the uppercase (lowercase) version of the iterator element name).

The iterator environment should be closed outside the wrapper
definition by the following wrapper command:

.. code-block:: c

   //$ENDITERATE

.. TODO: reference to an example in the appendix


.. _Wrapper_commands:

Wrapper commands
++++++++++++++++

Below several commands are described that are used to change the
default behaviour in the creation of the wrapper commands.


``HFPP_CODE_PRE``:
  This wrapper command is used when a predefined piece of code should
  be executed before the function will be called.  The argument of
  this command contains the C/C++ that is executed before the function
  call.

  This can be useful if a scratch vector needs to be made before the
  function is called, e.g:

  .. code-block:: c

     #define HFPP_CODE_PRE hResize(vecout,vecin);

``HFPP_FUNC_MASTER_ARRAY_PARAMETER``:
    Defines which parameter is used as the master array when looping
    over an array (Looping over an array within Python is accomplished
    by using ellipsis). The argument is an integer which is equal to
    the (0-based) index of the parameter over which to loop. This
    parameter should be of the hArray or vector type, e.g.:

    .. code-block:: c

       #define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1

    to loop over the second parameter.

``HFPP_FUNC_VARIANT``:

    Often multiple variants of the same function are generated plus
    all their wrappers. Usually these functions are distinguished by
    different numbers of (templated) parameters and hence automatic
    overloading works just fine. However, sometimes a variant
    (overloaded) function has the same number (and type) of parameters
    as the wrapper of a previous variant (remember: a vector which is
    one parameters, suddenly becomes two parameters when passed as
    iterators!). In this case one has to explicitly name the
    underlying functions differently and add, e.g., a number to the
    functioname. The value of ``HFPP_FUNC_VARIANT`` is used to
    distinguish one variant from another, e.g.:

    .. code-block:: c

       #define HFPP_FUNC_VARIANT 1

    for one function and:

    .. code-block:: c

       #define HFPP_FUNC_VARIANT 2

    for a second function which has the same name.

..   %% Martin: add an example here...

``HFPP_WRAPPER_CLASSES``:
    By default wrappers are created for the following classes:
    ``HFPP_CLASS_STL``, ``HFPP_CLASS_CASA``, ``HFPP_CLASS_hARRAY`` and
    ``HFPP_CLASS_hARRAYALL``. This command is used to override these
    default classes. E.g.:

    .. code-block:: c

       HFPP_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_CASA

    will only create C++ wrappers for the STL and CASA classes.

``HFPP_WRAPPER_TYPES``:
  Create wrappers for the type described by the argument, which is one
  of the following types:

    =========================== =======================================================
    Wrapper type name           Containing types
    =========================== =======================================================
    ``HFPP_REAL_NUMERIC_TYPES`` ``HInteger``, and ``HNumber``
    ``HFPP_STRING_TYPES``       ``HString``
    ``HFPP_NUMERIC_TYPES``      ``HFPP_REAL_NUMERIC_TYPES``, and ``HComplex``
    ``HFPP_STRING_TYPES``       ``HString``
    ``HFPP_LOGICAL_TYPES``      ``HBool``
    ``HFPP_POINTER_TYPES``      ``HPointer``
    ``HFPP_NON_NUMERIC_TYPES``  ``HFPP_STRING_TYPES``, and ``HFPP_POINTER_TYPES``
    ``HFPP_ALL_TYPES``          ``HFPP_NUMERIC_TYPES``, and ``HFPP_NON_NUMERIC_TYPES``
    ``HFPP_ALL_PYTHONTYPES``    ``HFPP_NUMERIC_TYPES``, ``HFPP_LOGICAL_TYPES``, and
                                ``HFPP_STRING_TYPES``
    =========================== =======================================================

  By default wrappers are created for all numeric types
  (``HFPP_NUMERIC_TYPES``) consisting of ``HInteger``, ``HNumber`` and
  ``HComplex``.

``HFPP_PYTHON_WRAPPER_CLASSES``:
  Expose a specific set of (container) classes to Python.  By default
  all of the following classes are exposed to Python:
  ``HFPP_CLASS_STL``, ``HFPP_CLASS_hARRAY``, ``HFPP_CLASS_hARRAYALL``. E.g.:

  .. code-block:: c

     #define HFPP_PYTHON_WRAPPER_CLASSES HFPP_CLASS_STL HFPP_CLASS_hARRAY

  will expose only the STL and hArray classes to Python.

``HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS``:

  In the case that only STL wrappers need to be generated set this
  wrapper variable to ``HFPP_NONE``. E.g.:

  .. code-block:: c

     #define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE


.. _Pycrtools_usage:

Building and running the pycrtools
==================================


Building the pycrtools
----------------------

In order to be able to compile the pycrtools code one has to enable
the corresponding CMake configuration flag for this component. This is
set in the ``$LOFARSOFT/build/cr`` directory and can be changed by
calling the CMake configuration utility via:

.. code-block:: sh

   $ ccmake .

and apply the following setting::

  CR_WITH_PYPELINE=ON


The pycrtools can now be compiled by building the hftools component
via:

.. code-block:: sh

  $ make hftools

and installed via:

.. code-block:: sh

  $ make install

or combine the last two commands by typing:

.. code-block:: sh

   $ make hftools install


..
   Using the pycrtools

..
   Starting python

..
   Starting IPython


Code examples
=============

Example 1: hNew
---------------

.. _example_1_input_code:

Input code
~~~~~~~~~~

The code below shows the wrapper code for of the ``hNew`` function.
This is a templated function where the input parameter is an STL
vector of the templated type.  Furthermore, it should only be
generated for STL classes, and create wrappers for all types available
in Python (i.e. numeric type, logical types and string types):

.. code-block:: c

   //$DOCSTRING: Make and return a new vector of the same size and type as the input vector.
   //$COPY_TO HFILE START --------------------------------------------------
   #define HFPP_FUNC_NAME hNew
   //-----------------------------------------------------------------------
   #define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
   #define HFPP_WRAPPER_TYPES HFPP_ALL_PYTHONTYPES
   #define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_VALUE)
   #define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Input vector")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
   //$COPY_TO END --------------------------------------------------
   /*!
     \brief $DOCSTRING
     $PARDOCSTRING
   */
   template <class T>
   std::vector<T> HFPP_FUNC_NAME(std::vector<T> & vec)
   {
     std::vector<T> vnew(vec.size());
     return vnew;
   }
   //$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


.. _example_1_output_code:

Output code
~~~~~~~~~~~

The code that is generated from the :ref:`example_1_input_code` is shown
below and consists of four blocks of code.

In the first block the function is declared for the different types,
using unique function names. These functions serve as an interface for
the wrappers:

.. code-block:: c

   extern std::vector<HString> (*fptr_hNew_STL_HString_vec_1_STL)( std::vector<HString> & vec);
   extern std::vector<HBool> (*fptr_hNew_STL_HBool_vec_1_STL)( std::vector<HBool> & vec);
   extern std::vector<HComplex> (*fptr_hNew_STL_HComplex_vec_1_STL)( std::vector<HComplex> & vec);
   extern std::vector<HNumber> (*fptr_hNew_STL_HNumber_vec_1_STL)( std::vector<HNumber> & vec);
   extern std::vector<HInteger> (*fptr_hNew_STL_HInteger_vec_1_STL)( std::vector<HInteger> & vec);

The second block consists of the the implementation of the templated
function:

.. code-block:: c

   template <class T>
   std::vector<T> hNew(std::vector<T> & vec)
   {
     std::vector<T> vnew(vec.size());
     return vnew;
   }

The third block binds the function pointers that are used by Python to
the C++ function pointers:

.. code-block:: c

   std::vector<HString> (*fptr_hNew_STL_HString_vec_1_STL)( std::vector<HString> & vec) = &hNew;
   std::vector<HBool> (*fptr_hNew_STL_HBool_vec_1_STL)( std::vector<HBool> & vec) = &hNew;
   std::vector<HComplex> (*fptr_hNew_STL_HComplex_vec_1_STL)( std::vector<HComplex> & vec) = &hNew;
   std::vector<HNumber> (*fptr_hNew_STL_HNumber_vec_1_STL)( std::vector<HNumber> & vec) = &hNew;
   std::vector<HInteger> (*fptr_hNew_STL_HInteger_vec_1_STL)( std::vector<HInteger> & vec) = &hNew;

In the last block the function name (``hNew``) is associated with the
function pointers that are called from Python:

.. code-block:: c

   def("hNew",fptr_hNew_STL_HString_vec_1_STL );
   def("hNew",fptr_hNew_STL_HBool_vec_1_STL );
   def("hNew",fptr_hNew_STL_HComplex_vec_1_STL );
   def("hNew",fptr_hNew_STL_HNumber_vec_1_STL );
   def("hNew",fptr_hNew_STL_HInteger_vec_1_STL );



Example 2: hVectorLength
------------------------

.. _example_2_input_code:

Input code
~~~~~~~~~~

The code below shows the wrapper code for of the :func:`hVectorLength`
function.

This is a templated function where the input parameter is a vector of
the templated type.

As the functionality only applies to integer or real numeric vectors
it is only wrapped for these types of parameters.

.. code-block:: c

   //$DOCSTRING: Returns the lengths or norm of a vector.
   //$COPY_TO HFILE START --------------------------------------------------
   #define HFPP_FUNC_NAME hVectorLength
   //-----------------------------------------------------------------------
   #define HFPP_WRAPPER_TYPES HFPP_REAL_NUMERIC_TYPES
   #define HFPP_FUNCDEF  (HNumber)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
   #define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec)()("Numeric input vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
   //$COPY_TO END --------------------------------------------------
   /*!
     \brief $DOCSTRING
    $PARDOCSTRING
   */
   template <class Iter>
   HNumber HFPP_FUNC_NAME (const Iter vec,const Iter vec_end)
   {
     HNumber sum=0;
     Iter it=vec;
     while (it!=vec_end) {sum += (*it) * (*it); ++it;};
     return sqrt(sum);
   }
   //$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


.. _example_2_output_code:

Output code
~~~~~~~~~~~


The code that is generated from the :ref:`example_2_input_code` is shown below
and consists of five blocks of code.

The first block contains the declarations of the templated C++ functions:

.. code-block:: c

   template < class T > inline HNumber hVectorLength( std::vector<T> & vec);
   template < class T > inline HNumber hVectorLength( casa::Vector<T> & vec);
   template < class T > inline vector<HNumber> hVectorLength( hArray<T> & vec);

In the second block the function pointers are declared that will be
called from Python:

.. code-block:: c

   extern HNumber (*fptr_hVectorLength_STL_HNumber_vec_1_STDIT)( std::vector<HNumber> & vec);
   extern vector<HNumber > (*fptr_hVectorLength_hARRAY_HNumber_vec_1_STDIT)( hArray<HNumber> & vec);
   extern HNumber (*fptr_hVectorLength_STL_HInteger_vec_1_STDIT)( std::vector<HInteger> & vec);
   extern vector<HNumber > (*fptr_hVectorLength_hARRAY_HInteger_vec_1_STDIT)( hArray<HInteger> & vec);

The implementation of the templated C++ functions is done in the third
block:

.. code-block:: c

   template <class Iter>
   HNumber hVectorLength (const Iter vec,const Iter vec_end)
   {
     HNumber sum=0;
     Iter it=vec;
     while (it!=vec_end) {sum += (*it) * (*it); ++it;};
     return sqrt(sum);
   }

   template < class T > inline HNumber hVectorLength( std::vector<T> & vec)
   {
     return hVectorLength ( vec.begin(),vec.end());
   }

   template < class T > inline HNumber hVectorLength( casa::Vector<T> & vec)
   {
     return hVectorLength ( vec.cbegin(),vec.cend());
   }

   template < class T > inline vector<HNumber> hVectorLength( hArray<T> & vec)
   {
     bool iterate=true;
     bool hist=vec.isTrackingHistory();
     vector<HNumber> returnvector;
     HNumber returnvalue;
     if (hist) vec.addHistory((HString)"hVectorLength",(HString)"  Par "+hf2string(0)+": "+ /
         "vec"+" = "+"array["+"T"+","+hf2string(vec.length())+"] : "+pretty_vec(vec,3));
     while(iterate)
     {
       returnvalue = hVectorLength ( vec.begin(),vec.end());
       vec.next();
       iterate = vec.doLoopAgain();
       returnvector.push_back(returnvalue);
     };
     return returnvector;
   }

In the fourth block the function pointers that are used by python are
bound to the C++ function pointers:

.. code-block:: c

   HNumber (*fptr_hVectorLength_STL_HNumber_vec_1_STDIT)( std::vector<HNumber> & vec) = &hVectorLength;
   vector<HNumber > (*fptr_hVectorLength_hARRAY_HNumber_vec_1_STDIT)( hArray<HNumber> & vec) = &hVectorLength;
   HNumber (*fptr_hVectorLength_STL_HInteger_vec_1_STDIT)( std::vector<HInteger> & vec) = &hVectorLength;
   vector<HNumber > (*fptr_hVectorLength_hARRAY_HInteger_vec_1_STDIT)( hArray<HInteger> & vec) = &hVectorLength;

Finally, in the fifth block the function name ``hVectorLength`` is
associated with the function pointers that are called from Python:

.. code-block:: c

   def("hVectorLength",fptr_hVectorLength_STL_HNumber_vec_1_STDIT );
   def("hVectorLength",fptr_hVectorLength_hARRAY_HNumber_vec_1_STDIT );
   def("hVectorLength",fptr_hVectorLength_STL_HInteger_vec_1_STDIT );
   def("hVectorLength",fptr_hVectorLength_hARRAY_HInteger_vec_1_STDIT );

