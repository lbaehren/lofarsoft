import sys
import os
import re

# B1508+55_L2010_20681_RSP0_DM14.00_PSR_1509+5531.pfd.bestprof 

DM_IN_FILENAME_PATTERN = r'\S*_DM(?P<dm>(\d+\.\d+|\d+))_\S+\.pfd\.bestprof$'
DM_PATTERN = r'#\sBest\sDM\s*=\s*(?P<dm>(-|\+)?(\d+\.\d+|\d+)((e|E)(\+|-)\d+)?)\s*'
CHI_SQUARED_PATTERN = r'#\s*Reduced\s*chi-sqr\s*=\s*(?P<chi_squared>(-|\+)?(\d+\.\d+|\d+)((e|E)(\+|-)\d+)?)\s*'

dm_in_filename_regexp = re.compile(DM_IN_FILENAME_PATTERN)
dm_regexp = re.compile(DM_PATTERN)
chi_squared_regexp = re.compile(CHI_SQUARED_PATTERN)


def parse_pfd_bestprof_file_old(file):
    # Deals with the case that prepfold ran on data where a DM search was
    # performed.
    dm = None
    chi_squared = None
    try:
        f = open(file, 'r')
        try:
            for l in f:
                m = dm_regexp.match(l)
                if m:
                    dm = float(m.group('dm'))
                m = chi_squared_regexp.match(l)
                if m:
                    chi_squared = float(m.group('chi_squared'))
        finally:
            f.close()

    except IOError, e:
        print 'could not open %s' % file
        raise
    return chi_squared, dm

def parse_pfd_bestprof_file(file):
    # Deals with the case that the input dat file to prepfold was dedispersed 
    # to a certain DM. In that case there is no best_dm in the .pfd.bestprof 
    # file, in that case the DM comes from the filename.
    dm = None
    m = dm_in_filename_regexp.match(os.path.split(file)[-1])
    if m:
        dm = float(m.group('dm'))

    chi_squared = None
    try:
        f = open(file, 'r')
        try:
            for l in f:
                m = chi_squared_regexp.match(l)
                if m:
                    chi_squared = float(m.group('chi_squared'))
        finally:
            f.close()

    except IOError, e:
        print 'could not open %s' % file
        raise
    return chi_squared, dm

if __name__ == '__main__':
    print parse_pfd_bestprof_file(sys.argv[1])
