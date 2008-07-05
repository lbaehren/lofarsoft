from distutils.core import Extension, setup

e = Extension(name="dimsdata", sources=["dimsdata.c"])

setup(name="dimsdata", ext_modules=[e])

