"""
numarray.ma is a port of Paul Dubois' MA packge for Numeric.

numarray.linear_algebra, numarray.fft, and numarray.random_array are
likewise ports of Numeric's LinearAlgebra, FFT, and RandomArray.

numarray.convolve and numarray.image were created for numarray.
"""

import os, sys
from generate import USE_LAPACK, USE_ABSOFT, normalize, OurExtension

if os.path.exists('/System/Library/Frameworks/vecLib.framework'):
	USE_LAPACK = True

if USE_LAPACK:
    print "Using external BLAS and LAPACK"
    linear_algebra_sourcelist = [
        os.path.join('Packages/LinearAlgebra2/Src', 'lapack_litemodule.c'),
        ]

    # Set to list off libraries to link against.
    # (only the basenames, e.g. 'lapack'.  the library filename will be
    # something like liblapack.a, liblapack.so, etc.

    # Set to list directories to be searched for BLAS and LAPACK libraries
    lapack_compile_args = []
    lapack_link_args = [] 
    lapack_include_dirs = ["Packages/LinearAlgebra2/Src"] 

    if os.path.exists('/System/Library/Frameworks/vecLib.framework'):
        # use Apple's optimized BLAS implementation
        lapack_libs = []
        lapack_link_args = ['-framework', 'vecLib']
        lapack_compile_args = ['-framework', 'vecLib']
        lapack_dirs = []
        lapack_include_dirs +=  ['/System/Library/Frameworks/vecLib.framework/Headers']
    elif USE_ABSOFT:  # Absoft Fortran
        lapack_dirs = ['/usr/local/lib/atlas', '/opt/absoft/lib']
        lapack_libs = ['lapack', 'f77blas', 'cblas', 'atlas',
                       'f90math', 'fio', 'f77math', 'm']
    else:
        lapack_dirs = ['/usr/local/lib/atlas']
        lapack_include_dirs += ["/usr/local/include/atlas"]
        lapack_libs = ['lapack', 'cblas', 'f77blas', 'atlas', 'g2c', 'm']
    
    if os.environ.has_key('LINALG_LIB'):
        lapack_dirs += [os.environ['LINALG_LIB']]

    if os.environ.has_key('LINALG_INCLUDE'):
        lapack_include_dirs += [os.environ['LINALG_INCLUDE']]
else:
    print "Using builtin 'lite' BLAS and LAPACK"
    linear_algebra_sourcelist = [
        os.path.join('Packages/LinearAlgebra2/Src', 'lapack_litemodule.c'),
        os.path.join('Packages/LinearAlgebra2/Src', 'blas_lite.c'),
        os.path.join('Packages/LinearAlgebra2/Src', 'f2c_lite.c'),
        os.path.join('Packages/LinearAlgebra2/Src', 'zlapack_lite.c'),
        os.path.join('Packages/LinearAlgebra2/Src', 'dlapack_lite.c'),
        os.path.join('Packages/LinearAlgebra2/Src', 'dlamch.c')
        ]
    lapack_include_dirs = ["Packages/LinearAlgebra2/Src"]
    lapack_libs = []
    lapack_dirs = []
    if os.path.isfile(os.path.join("Packages/LinearAlgebra2/Src","config.h")):
        lapack_compile_args = ['-DHAVE_CONFIG']
    else:
        lapack_compile_args = []
    lapack_link_args = []

ADDONS_DATA_FILES = [('numarray/ma', ['Packages/MA/Legal.htm'])]

ADDONS_PACKAGES = ["numarray.convolve",
                   "numarray.fft",
                   "numarray.linear_algebra",
                   "numarray.random_array",
                   "numarray.image",
                   "numarray.nd_image",
                   "numarray.ma",
                   "numarray.mlab",
                   "numarray.matrix",
                   ]

ADDONS_PACKAGE_DIRS = {"numarray.convolve":"Packages/Convolve/Lib",
                       "numarray.fft": "Packages/FFT2/Lib",
                       "numarray.linear_algebra":"Packages/LinearAlgebra2/Lib",
                       "numarray.random_array":"Packages/RandomArray2/Lib",
                       "numarray.image":"Packages/image/Lib",
                       "numarray.nd_image":"Packages/nd_image/Lib",
                       "numarray.ma":"Packages/MA/Lib",
                       "numarray.mlab":"Packages/mlab/Lib",
                       "numarray.matrix":"Packages/matrix/Lib",
                       }

ADDONS_EXTENSIONS = [
    OurExtension("numarray.convolve._correlate",
                 ["Packages/Convolve/Src/_correlatemodule.c"]),
    OurExtension("numarray.image._combine",
                 ["Packages/image/Src/_combinemodule.c"]),
    OurExtension("numarray.nd_image._nd_image",
                 ["Packages/nd_image/Src/nd_image.c",
                  "Packages/nd_image/Src/ni_filters.c",
                  "Packages/nd_image/Src/ni_fourier.c",
                  "Packages/nd_image/Src/ni_interpolation.c",
                  "Packages/nd_image/Src/ni_measure.c",
                  "Packages/nd_image/Src/ni_morphology.c",
                  "Packages/nd_image/Src/ni_support.c"],
                 include_dirs = ['Packages/nd_image/Src']),
    OurExtension("numarray.convolve._lineshape",
                 ["Packages/Convolve/Src/_lineshapemodule.c"]),
    OurExtension('numarray.fft.fftpack',
                 ['Packages/FFT2/Src/fftpackmodule.c',
                  'Packages/FFT2/Src/fftpack.c'],
                 include_dirs = ['Packages/FFT2/Include']),
    OurExtension('numarray.linear_algebra.lapack_lite2',
                 linear_algebra_sourcelist,
                 include_dirs = lapack_include_dirs,
                 library_dirs = lapack_dirs,
                 libraries = lapack_libs,
                 extra_compile_args = lapack_compile_args,
                 extra_link_args = lapack_link_args ),
    OurExtension("numarray.random_array.ranlib2",
                 ["Packages/RandomArray2/Src/ranlib.c",
                  "Packages/RandomArray2/Src/ranlibmodule.c",
                  "Packages/RandomArray2/Src/com.c",
                  "Packages/RandomArray2/Src/linpack.c"])
    ]

# Define this here because it needs all the lapack libraries
if USE_LAPACK:  
    ADDONS_EXTENSIONS.append(
        OurExtension('numarray._dotblas',
                     ['Src/_dotblas.c'],
                     include_dirs = lapack_include_dirs,
                     library_dirs = lapack_dirs,
                     libraries = lapack_libs,
                     extra_link_args = lapack_link_args )
        )
