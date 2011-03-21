#!/usr/bin/env python
'''
Compare candidates to the known catalog.
'''
from __future__ import division

import os
import sys
import math

from inf import ascii2ra, ascii2dec, dec_ok, ra_ok, dec2ascii, ra2ascii

# -----------------------------------------------------------------------------
# -- Constants, relevant to LPPS survey --------------------------------------- 

BRIGHT_PULSAR_CATALOG = os.path.join(os.environ['LOFARSOFT'],
    'release', 'share', 'pulsar', 'data', 'psr_cats.txt')

# Description of catalog format (white space seperated columns in text file).
# The columns mentioned below will be picked up as attributes for the 
# BrightPulsar instances created by the load_bright_pulsar_catalog function
# below.
CATALOG_SPEC = {
    0 : ('seq_no', int),
    1 : ('name', str),
    2 : ('psrj', str),
    3 : ('ra_j2000', ascii2ra),
    4 : ('dec_j2000', ascii2dec),
    5 : ('p0', float),
    6 : ('dm', float),
    7 : ('s400', float),
    8 : ('s1400', float),
}

DEFAULT_DDM = 0.05
DEFAULT_DP = 0.01

HARMONICS = set(range(2, 31))
HARMONICS |= set([i / 2 for i in range(40)])
HARMONICS |= set([i / 3 for i in range(59)])
HARMONICS |= set([i / 4 for i in range(42)])
HARMONICS.remove(1.)
HARMONICS.remove(0.)

# -----------------------------------------------------------------------------

class BrightPulsar(object):
    pass

def parse_pulsar_entry(line):
    '''Parse line, create pulsar or raise a ValueError on failure.'''
    n_columns = len(list(CATALOG_SPEC.keys())) 

    if not len(line):
        raise ValueError('Cannot parse empty line.')
    if line[0] == '#':
        raise ValueError('Cannot parse a line that is commented out.')
    tmp = line.split()
    if len(tmp) != n_columns:
        raise ValueError('Line does not match expected format: %s' % line)

    bp = BrightPulsar()
    for k, v in CATALOG_SPEC.iteritems():
        try:
            value = v[1](tmp[k])
        except ValueError, e:
            raise
        else:
            setattr(bp, v[0], value) 
    return bp

def load_bright_pulsar_catalog(catalog_location = BRIGHT_PULSAR_CATALOG):
    '''
    Read the system installed bright pulsar catalog, return a list of pulsar
    objects.

    The CATALOG_SPEC global variable contains all the attributes that the
    BrightPulsar instances will get after having been read from the 
    catalog file (using the parse_pulsar_entry function).
    '''    
    known_pulsars = []

    try:
        f = open(catalog_location, 'r')
        try:
            for l in f:
                try:
                    kp = parse_pulsar_entry(l)
                except ValueError, e:
                    pass
                else:
                    known_pulsars.append(kp)
        finally:
            f.close()
    except IOError, e:
        print 'Could not open bright pulsar catalog \'%s\'' % catalog_location

    return known_pulsars

def is_bright_pulsar(candidate, bright_pulsars, **kwargs):
    '''Check whether a candidate matches any of the supplied bright_pulsars.'''
    dp = kwargs.get('dp', DEFAULT_DP)
    ddm = kwargs.get('ddm', DEFAULT_DDM)
    for bp in bright_pulsars:
        for h in HARMONICS:
            if (h * bp.p0 * (1 - dp) < candidate.p < h * bp.p0 * (1 + dp)) and\
                (bp.dm * (1 - ddm) < candidate.DM < bp.dm * (1 + ddm)):
                print 'Identified bright pulsar %s at harmonic %.2f P0' % \
                    (bp.psrj, h)
                return True
        return False

def remove_bright_pulsars(candidates, bright_pulsars):
    '''Remove candidates from a list given a list of known bright pulsars.'''
    print '\nBefore removing bright pulsars there were %d candidates.' % \
        len(candidates)
    out = []
    for c in candidates:
        if not is_bright_pulsar(c, bright_pulsars):
            out.append(c)
    print 'After removing bright pulsars there were %d candidates.' % \
        len(out)
    return out

if __name__ == '__main__':        
    # Some self tests, run this module as a script to run the tests. 
    import unittest

    class Candidate(object):
        def __init__(self, p, dm):
            self.p = p
            self.DM = dm

    class TestLoadBrightCatalog(unittest.TestCase):
        def runTest(self):
            '''Test the loading of the system installed catalog.'''
            bpc = load_bright_pulsar_catalog()

    class TestParser(unittest.TestCase):
        def parse_pulsar(self):
            '''Test parsing a bona fide pulsar entry.'''
            parse_pulsar_entry('1     B1915+13      J1917+1353    19:17:39.7  +13:53:56.9   0.194631       94.54     43.00      1.90') 

        def parse_garbage(self):
            '''Test that parsing garbage entries raises ValueError.'''
            self.assertRaises(ValueError, parse_pulsar_entry, '')
            self.assertRaises(ValueError, parse_pulsar_entry, '#')
            self.assertRaises(ValueError, parse_pulsar_entry, 'This is not a pulsar')

    class TestRejection(unittest.TestCase):
        def setUp(self):
            self.bpc = [parse_pulsar_entry('1     B1915+13      J1917+1353    19:17:39.7  +13:53:56.9   0.194631       94.54     43.00      1.90')]        

        def test_is_bright_pulsar(self):
            '''Test whether candidates are recognized as bright pulsars.'''
            c2 = Candidate(0.194631, 94.54) # fundamental should be ignored
            self.assertFalse(is_bright_pulsar(c2, self.bpc))
            c1 = Candidate(20, 20)
            self.assertFalse(is_bright_pulsar(c1, self.bpc))

        def test_remove_bright_pulsars(self):
            '''Test removal of bright pulsars from candidates.'''
            l = [
                Candidate(0.194631, 94.54), # fundamental should remain 
                Candidate(0.5 * 0.194631 * 1.009, 94.54), # should go
                Candidate(0.5 * 0.194631, 94.54), # should go
                Candidate(23, 94.54), # should remain
                Candidate(0.5 * 0.194631, 1) # should remain
            ]
                
            l = remove_bright_pulsars(l, self.bpc)
            self.assertEqual(len(l), 3)

    test_suite = unittest.TestSuite()
    test_suite.addTest(TestLoadBrightCatalog())
    test_suite.addTest(TestParser('parse_pulsar'))
    test_suite.addTest(TestParser('parse_garbage'))
    test_suite.addTest(TestRejection('test_is_bright_pulsar'))
    test_suite.addTest(TestRejection('test_remove_bright_pulsars'))

    print 'Running module unittests'
    unittest.TextTestRunner(verbosity=2).run(test_suite)
    print '\n'

    print '-' * 78
    print ' current harmonics searched for'
    for h in HARMONICS:
        print h,
    print ''
