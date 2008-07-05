"""This module provides functions for creating read-only arrays.

get_readonly_p(a)  returns a copy of 'a' which is read-only in Python code.
                   Writing to a read-only-p array in Python causes an
                   exception.  Writing to a read-only-p array in C succeeds.

get_readonly_c(a)  returns a copy of 'a' which is read-only in both Python
                   and C. Writing to a read-only-c array in Python causes
                   an exception.   Writing to a read-only-c array in C causes
                   a segfault which can be used for debugging.
"""
import numarray
import numarray.memmap as memmap
import tempfile

def get_readonly_p(a):
    """Returns a copy of 'a' which is read-only to Python operations."""
    v = a.view()
    v._data = a.tostring()
    return v

def get_readonly_c(a):
    """returns a copy of 'a' which is read-only in Python and C."""
    s = a.tostring()
    f = tempfile.NamedTemporaryFile("wb")
    f.write(s)
    f.flush()
    m = memmap.open(f.name, "r", len=len(s))  # Get the memory map object
    b = m[:]                           # Get a buffer-like object
    r  = numarray.NumArray(buffer=b, shape=a.shape, type=a.type())
    r._memmap = m
    r._memmap_file = f
    return r

def main():
    a = numarray.arange(100)
    
    r = get_readonly_p(a)
    print r
    
    r = get_readonly_c(a)
    print r

if __name__ == "__main__":
    main()


