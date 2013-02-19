#! /usr/bin/env python

def tImportPyCRTools():
    try:
        import pycrtools as cr
    except:
        raise ImportError()

if __name__ == '__main__':
    tImportPyCRTools()
