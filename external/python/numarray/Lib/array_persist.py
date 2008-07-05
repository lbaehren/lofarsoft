
import numarray
import string

import numarray.random_array
import os
import random
import unittest
import sys

"""
array_persist.py

this module provides an easy way to persist numarrays to/from binary files

functions:
to_file
from_file

usage:

>>> a = numarray.array(range(10), 'f')
>>> to_file(a, file("C:\\temp\\a.bin", 'wb'))
>>> from_file(file("C:\\temp\\a.bin", 'rb'))
array([ 0.,  1.,  2.,  3.,  4.,  5.,  6.,  7.,  8.,  9.], type=Float32)

"""

def to_file(a, f):
    if type(f) == type(""):
        f = file(f, 'wb')
    if f.mode != 'wb':
        raise "FileModeError: file mode must equal 'wb'"
    f.write(a.type().name + '\n')
    f.write(str(a.shape) + '\n')
    f.write(a._byteorder + '\n')
    a.tofile(f)
    f.close()

def from_file(f):
    if type(f) == type(""):
        f = file(f, 'rb')
    if f.mode != 'rb':
        raise "FileModeError: file mode must equal 'rb'"
    array_type = string.strip(f.readline())
    array_shape = eval(string.strip(f.readline()))
    byteorder = string.strip(f.readline())
    a = numarray.fromfile(f, array_type, array_shape)
    f.close()
    if byteorder != sys.byteorder:
        a.byteswap()
    return a


class TestArrayPersist(unittest.TestCase):

    def setUp(self):
        self.num_test_arrays = 10
        self.max_rows = 1000
        self.max_cols = 100
        self.path = "c:\\temp\\TestArrayPersist"
        if os.path.exists(self.path):
            for filename in os.listdir(self.path):
                os.remove(os.path.join(self.path, filename))
        else:
            os.makedirs(self.path)
        self.make_files()

    def testfromfile(self):        
        for i in range(self.num_test_arrays):
            f = self.make_file(i, 'rb')
            a = from_file(f)

    def make_file(self, i, mode):
        return file(os.path.join(self.path, str(i)+".bin"), mode)

    def make_filename(self, i):
        return os.path.join(self.path, str(i)+".bin")

    def make_files(self):
        for i in range(self.num_test_arrays):
            a = self.make_array()
            if random.random() < 0.5:
                f = self.make_file(i, 'wb')
            else:
                f = self.make_filename(i)
            to_file(a, f)

    def make_array(self):    
        if random.random() < 0.5:
            return numarray.random_array.randint(-1000, 1000, self.make_shape())
        else:
            return numarray.random_array.random(self.make_shape())

    def make_shape(self):
        r = random.random()
        if r < .33:
            return (self.max_rows, )
        elif r < .66:
            return (self.max_rows, self.max_cols)
        else:            
            return (self.max_rows, self.max_cols, self.max_cols)

if __name__ == "__main__":
    unittest.main()
    
