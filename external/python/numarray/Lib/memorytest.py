"""
This module provides a selftest for the numarray.memory c-extension.

>>> m = new_memory(10)
>>> len(m)
10
>>> for i in range(len(m)):
...     m[i] = i
>>> str(m)
'\\x00\\x01\\x02\\x03\\x04\\x05\\x06\\x07\\x08\\t'
>>> [ x for x in m ]
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
>>> [ x for x in m.copy()]
[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

A slice of a memory object returns an alias to the sliced region, not a copy.

>>> str(m[5:8])
'\\x05\\x06\\x07'

>>> m[5:8].tolist()
[5, 6, 7]
>>> m[5:8] = [20, 30, 40]
>>> m.tolist()
[0, 1, 2, 3, 4, 20, 30, 40, 8, 9]
>>> n = new_memory(10)
>>> o = new_memory(3)
>>> n.clear()
>>> for i in range(len(o)):
...     o[i] = 10
>>> n[2:5] = o
>>> n.tolist()
[0, 0, 10, 10, 10, 0, 0, 0, 0, 0]
"""

from numarray.memory import *

def test():
    import doctest, numarray.memorytest as memorytest
    return doctest.testmod(memorytest)

if __name__ == "__main__":
    test()

