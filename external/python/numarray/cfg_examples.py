from distutils.core import Extension

EXAMPLES_DATA_FILES= [] 

EXAMPLES_PACKAGE_DIRS = {
    "numarray.examples":"Examples",
    "numarray.examples.convolve":"Examples/convolve"
    }

EXAMPLES_PACKAGES=["numarray.examples",
                   "numarray.examples.convolve"]

EXAMPLES_EXTENSIONS = [Extension("numarray.examples.convolve.high_level",
                              ["Examples/convolve/high_levelmodule.c"],
                              include_dirs = ["Include/numarray"]),
                    Extension("numarray.examples.convolve.element_wise",
                              ["Examples/convolve/element_wisemodule.c"],
                              include_dirs = ["Include/numarray"]),
                    Extension("numarray.examples.convolve.one_dimensional",
                              ["Examples/convolve/one_dimensionalmodule.c"],
                              include_dirs = ["Include/numarray"]),
                    Extension("numarray.examples.convolve.numpy_compat",
                              ["Examples/convolve/numpy_compatmodule.c",
                               "Examples/convolve/numpy_compat2.c"],
                              include_dirs = ["Include/numarray"]),
                    Extension("numarray.examples.convolve.testlite",
                              ["Examples/convolve/testlite.c"],
                              include_dirs = ["Include/numarray"]),
                    ]
