from distutils.core import setup
from numarrayext import NumarrayExtension
import sys
import numinclude

setup(name = "RandomArray2",
      version = numinclude.version,
      description = "Random numarray ported from Numeric to Numarray",
      packages=[""],
      package_dir={"":"Lib"},
      extra_path="RandomArray2",
      ext_modules=[NumarrayExtension("ranlib2", ["Src/ranlib.c",
                                                "Src/ranlibmodule.c",
                                                "Src/com.c",
                                                "Src/linpack.c"])]
      )
