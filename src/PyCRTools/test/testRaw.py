import pycrtools as cr

arraySize = 1024

# __________________________________________________________________________
#                                                                       Bool

def testReadWriteRaw_B():
    """
    Test method for writing and reading an hArray containing booleans.

    """
    None
#     print "test: ReadWriteRaw for Boolean array."

#     a = cr.hArray(Type=bool, dimensions=[arraySize], fill=True)
#     a[1] = False
#     a_raw = a.writeRaw()

#     b = cr.hArray(Type=bool, dimensions=[arraySize], fill=False)
#     b.readRaw(a_raw)

#     print a
#     print b


# __________________________________________________________________________
#                                                                    Integer

def testReadWriteRaw_I():
    """
    Test method for writing and reading an hArray containing integer.

    Example (and doctest):

    Create integer arrays a and b and copy content of a to b via raw string
    >>> print r"Integer testing"
    Integer testing
    >>> a = cr.hArray(Type=int, dimensions=[4], fill=range(4))
    >>> p = a.writeRaw()
    >>> b = cr.hArray(Type=int, dimensions=[4], fill=range(0))
    >>> b
    hArray(int,[4]) # len=4, slice=[0:4], vec -> [0,0,0,0]
    >>> b.readRaw(p)
    >>> b
    hArray(int,[4]) # len=4, slice=[0:4], vec -> [0,1,2,3]
    """

    print "test: ReadWriteRaw for Integer array."

    a = cr.hArray(Type=int, dimensions=[arraySize], fill=range(arraySize))
    a_raw = a.writeRaw()

    b = cr.hArray(Type=int, dimensions=[arraySize], fill=0)
    b.readRaw(a_raw)

    print a
    print b


# __________________________________________________________________________
#                                                                      Float

def testReadWriteRaw_F():
    """
    Test method for writing and reading an hArray containing float.

    Example (and doctest):

    Create float arrays a and b and copy content of a to b via raw string
    >>> print r"Float testing"
    Float testing
    >>> a = cr.hArray(Type=float, dimensions=[4], fill=range(4))
    >>> p = a.writeRaw()
    >>> b = cr.hArray(Type=float, dimensions=[4], fill=range(0))
    >>> b
    hArray(float,[4]) # len=4, slice=[0:4], vec -> [0.0,0.0,0.0,0.0]
    >>> b.readRaw(p)
    >>> b
    hArray(float,[4]) # len=4, slice=[0:4], vec -> [0.0,1.0,2.0,3.0]
    """

    print "test: ReadWriteRaw for Float array."

    a = cr.hArray(Type=float, dimensions=[arraySize], fill=range(arraySize))
    a_raw = a.writeRaw()

    b = cr.hArray(Type=float, dimensions=[arraySize], fill=0.)
    b.readRaw(a_raw)

    print a
    print b


# __________________________________________________________________________
#                                                                    Complex

def testReadWriteRaw_C():
    """
    Test method for writing and reading an hArray containing complex.

    Example (and doctest):

    Create complex arrays a and b and copy content of a to b via raw string.
    >>> print r"Complex testing"
    Complex testing
    >>> a = cr.hArray(Type=complex, dimensions=[4], fill=range(4))
    >>> p = a.writeRaw()
    >>> b = cr.hArray(Type=complex, dimensions=[4], fill=range(0))
    >>> b
    hArray(complex,[4]) # len=4, slice=[0:4], vec -> [0j,0j,0j,0j]
    >>> b.readRaw(p)
    >>> b
    hArray(complex,[4]) # len=4, slice=[0:4], vec -> [0j,(1+0j),(2+0j),(3+0j)]
    """

    print "test: ReadWriteRaw for Complex array."

    a = cr.hArray(Type=complex, dimensions=[arraySize], fill=range(arraySize))
    for idx in range(arraySize):
        a[idx] = idx + (arraySize-idx)*(0+0j)
    a_raw = a.writeRaw()

    b = cr.hArray(Type=complex, dimensions=[arraySize], fill=0)
    b.readRaw(a_raw)

    print a
    print b


def testReadWriteRaw():
    testReadWriteRaw_I()
    testReadWriteRaw_F()
    testReadWriteRaw_C()

if __name__ == '__main__':
    import doctest
    doctest.testmod()
    testReadWriteRaw()



