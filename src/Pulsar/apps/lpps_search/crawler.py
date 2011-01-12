#!/usr/bin/env python
'''
Crawler that knows about PRESTO file names.

Thijs Coenen november 2010
'''
from __future__ import division
import re
import os

FILETYPE_SUFFIXES = {
    'INF' : r'^_DM(?P<dm>\d+\.\d{2})\.inf$',
    'DAT' : r'^_DM(?P<dm>\d+\.\d{2})\.dat$',
    'FFT' : r'^_DM(?P<dm>\d+\.\d{2})\.fft$',
    'REDFFT' : r'^_DM(?P<dm>\d+\.\d{2})\.red_fft$',
    'ACCEL' : r'^_DM(?P<dm>\d+\.\d{2})_ACCEL_(?P<z>\d+)$',
    'ACCEL_CAND' : r'^_DM(?P<dm>\d+\.\d{2})_ACCEL_(?P<z>\d+)\.cand$',
    'ACCEL_TXTCAND' : r'^_DM(?P<dm>\d+\.\d{2})_ACCEL_(?P<z>\d+)\.txtcand$',
    'SINGLEPULSE' : r'^_DM(?P<dm>\d+\.\d{2})\.singlepulse$',

    'RFIFIND_MASK' : r'^_rfifind.mask', 
    'RFIFIND_BYTEMASK' : r'^_rfifind.bytemask', 
    'RFIFIND_INF' : r'^_rfifind.inf', 
    'RFIFIND_PS' : r'^_rfifind.ps', 
    'RFIFIND_RFI' : r'^_rfifind.rfi', 
    'RFIFIND_STATS' : r'^_rfifind.stats', 
}

FILETYPE_PATTERNS = {}
for key, exp in FILETYPE_SUFFIXES.iteritems():
    FILETYPE_PATTERNS[key] = re.compile(exp)

def find_rfifind_output(path, basename, **kwargs):
    '''Find the output of PRESTO rfifind'''    
    output = []
    
    if kwargs.has_key('files'):
        files = kwargs['files']
    else:
        try:
            files = os.listdir(path)
        except OSError, e:
            files = []        

    L = len(basename) 
    for f in files:
        if not f.startswith(basename): continue
        suffix = f[L:]
        for filetype in ['RFIFIND_MASK', 'RFIFIND_STATS', 'RFIFIND_BYTEMASK',
            'RFIFIND_RFI', 'RFIFIND_RFI', 'RFIFIND_PS', 'RFIFIND_STATS']:          
            m = FILETYPE_PATTERNS[filetype].match(suffix)
            if m:
                output.append(f)
    return output
            
def find_accelsearch_output(path, basename, **kwargs):
    '''Find all files associated with PRESTO accelsearch.'''
    output = {}
    
    if kwargs.has_key('files'):
        files = kwargs['files']
    else:
        try:
            files = os.listdir(path)
        except OSError, e:
            files = []        

    L = len(basename)
    for f in files:
        if not f.startswith(basename): continue 
        suffix = f[L:]
        # match all the known output from accelsearch
        for filetype in ['ACCEL', 'ACCEL_CAND', 'ACCEL_TXTCAND']:
            match = FILETYPE_PATTERNS[filetype].match(suffix)
            if match:
                dm = float(match.group('dm'))
                z = int(match.group('z'))
                if output.has_key(dm):
                    if output[dm].has_key(z):
                        output[dm][z].append(f)
                    else:
                        output[dm][z] = [f]
                else:
                    output[dm] = {z : [f]}
    return output


def find_single_pulse_search_output(path, basename, **kwargs):
    '''Find all files associated with PRESTO single_pulse_search'''
    # TODO : find out wether to look for the .ps files as well.
    output = {}

    if kwargs.has_key('files'):
        files = kwargs['files']
    else:
        try:
            files = os.listdir(path)
        except OSError, e:
            files = []
        
    L = len(basename)
    for f in files:
        if not f.startswith(basename): continue
        suffix = f[L:]
        match = FILETYPE_PATTERNS['SINGLEPULSE'].match(suffix)
        match2 = FILETYPE_PATTERNS['INF'].match(suffix)
#        print match, match2

        if match:
            dm = float(match.group('dm'))
            output[dm] = f
    return output

def find_inf_files(path, basename, **kwargs):
    '''Find all PRESTO .inf files.'''
    output = {}

    if kwargs.has_key('files'):
        files = kwargs['files']
    else:
        try:
            files = os.listdir(path)
        except OSError, e:
            files = []
        
    L = len(basename)
    for f in files:
        if not f.startswith(basename): continue
        suffix = f[L:]
        match = FILETYPE_PATTERNS['INF'].match(suffix)

        if match:
            dm = float(match.group('dm'))
            output[dm] = [f]
    return output

# TODO : refactor the find_<SOMETHING>_files functions to be more general
   


if __name__ == '__main__':
    # some non formalized self tests:
    # TODO : formalize self tests using unittest module (if this module gets
    # used a lot) 
    l = [
        'BASENAME_DM2.00_ACCEL_0', 'BASENAME_DM2.00_ACCEL_0.cand',
        'BASENAME_DM2.00_ACCEL_0.txtcand',
        'BASENAME_DM2.00_ACCEL_10', 'BASENAME_DM2.00_ACCEL_10.cand',
        'BASENAME_DM2.00_ACCEL_10.txtcand',
    ]
    out = find_accelsearch_output('', 'BASENAME', files=l)
    if len(out[2.][10]) == 3 and len(out[2.][0]) ==3:
        print 'find_accelsearch_output test succelful.'
    else: 
        print 'find_accelsearch_output test failed.'

    l = ['blah', 'BASENAME_DM2.00.singlepulse', 'BASENAME_DM3.00.singlepulse']
    out = find_single_pulse_search_output('', 'BASENAME', files=l)
    if len(out.keys()) == 2:
        print 'find_single_pulse_search_output test succelful.'
    else:
        print 'find_single_pulse_search_output test failed.'

