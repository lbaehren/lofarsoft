#! /usr/bin/python

"""
   Executable fix_fits_coordinates.py
   For LOTAAS pointings and other FITS files, update coordinates based on parfile

.. contact:: Joeri van Leeuwen
"""

from optparse import OptionParser
from numpy import *
import sys, os, re, shutil
import ephem, pyfits

def write_header_comment(string, infile):
    start=infile.tell()
    infile.seek(-49,1) # go back to start
    infile.write(string)
    infile.seek(start)


def write_header_pos(string, infile):
    # make sure first zero is in there
    string = re.sub("^(-?)(\d:)", "\g<1>0\g<2>", string)

    # format with quotes and some blanking spaces
    fitsval = "'%s'   " % string

    # overwrite current value
    start=infile.tell()
    infile.seek(-70,1) # go back to start
    infile.write(fitsval)
    infile.seek(start)


def read_parset_coordinate(cmd):
    status=os.popen(cmd).readlines()
    # RA info exists in parset file
    try:
        value=float64(status[0][:-1].split(" = ")[-1])
    except: 
        print "Oops, could not get value for: %s", cmd

    return value


def get_coordinates_from_parset(sap, beam, parsetdir, obsid):
    rarad =read_parset_coordinate("grep 'Observation.Beam\[%s].angle1' %s/%s.parset" % (sap, parsetdir, obsid))
    decrad=read_parset_coordinate("grep 'Observation.Beam\[%s].angle2' %s/%s.parset" % (sap, parsetdir, obsid))
    raradoff =read_parset_coordinate("grep 'Observation.Beam\[%s].TiedArrayBeam\[%s].angle1' %s/%s.parset" % (sap, beam, parsetdir, obsid))
    decradoff=read_parset_coordinate("grep 'Observation.Beam\[%s].TiedArrayBeam\[%s].angle2' %s/%s.parset" % (sap, beam, parsetdir, obsid))
    return (rarad+raradoff, decrad+decradoff)


def main():
    # main loop
    usage = 'usage: %prog FITSFILE \n Updates the header in a single Lofar FITSFILE to the proper pointing coordinates'
    parser = OptionParser(usage=usage)
    
    global options
    pardir='/globalhome/lofarsystem/log/'
    parser.add_option("-d", "--pardir", 
                      action="store", type="string", dest="parsetdir",
                      help="which directory to grab parfiles from (default: %s)" % pardir, 
                      default=pardir)
    parser.add_option("-v", "--verbose", 
                      action="store_true", dest="verbose", 
                      help="Print coordinates and file name",
                      default=False)

    (options, args) = parser.parse_args()
    if len(args) < 1:
        parser.error("Incorrect number of arguments")

    # deconstruct filename
    fullfilename=args[0]
    filename=os.path.basename(fullfilename)
    p = re.compile('(L\d\d\d\d*)_SAP(\d*)_BEAM(\d*)') # find OBSID, SAP, BEAM
    m = p.search(filename)
    try:
        (obsid,sap,beam)=m.groups()
    except:
        print "WARNING: Could not parse the file name as OBSID_SAP_BEAM. Perhaps this is a Fly's Eye observation, which as no beam number in the file name. Not updating coordinates as Fly's Eye always points in same direction anyway."
        sys.exit(0)

    if (options.verbose):
        print "Working on file %s:" % fullfilename

    # then extract the proper coordinates:
    (rarad,decrad)=get_coordinates_from_parset(sap, beam, options.parsetdir, obsid)

    # convert to HHMMSS
    src      = ephem.FixedBody()
    src._ra  = float(rarad)
    src._dec = float(decrad)

    #now update the fits file 
# unfortunately, pyfits won't write out our fits format
#    hdulist = pyfits.open(fullfilename)
#    prihdr = hdulist[0].header
#    print "Old coordinates: %s, %s" % (prihdr['RA'], prihdr['DEC'])
#    print "New coordinates: %s, %s" % (str(src._ra), str(src._dec))

    infile = open(fullfilename, "r+b")
    Update_RA=Update_Dec=False
    while (Update_RA==False or Update_Dec==False):
        data = infile.read(80)
#        print "line: * %s *" % data
        
        # Fix the RA
        if (data.split()[0]=='RA'):
            if (options.verbose):
                print "  Replacing RA : %s -> %s'" % (data.split()[2], str(src._ra))
            write_header_pos(str(src._ra), infile)
            write_header_comment('/ Right ascension (hh:mm:ss.ssss) [Updated]', infile)
            Update_RA=True

        # Fix the DEC
        if (data.split()[0]=='DEC'):
            if (options.verbose):
                print "  Replacing Dec: %s -> %s'" % (data.split()[2], str(src._dec))
            write_header_pos(str(src._dec), infile)
            write_header_comment('/ Declination (-dd:mm:ss.sss) [Updated]', infile)
            Update_Dec=True
 
    infile.close()

if __name__ == '__main__':
  main()


