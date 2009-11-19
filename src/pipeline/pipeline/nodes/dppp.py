# Python standard library
from subprocess import check_call
from tempfile import mkstemp
from os import unlink

# External
from cuisine.parset import Parset

def run_dppp(infile, outfile, parset):
    executable = "/app/lofar/stable/bin/CS1_IDPPP"

    # We need to patch the parset with the correct input/output MS names.
    temp_parset_filename = mkstemp()[1]
    temp_parset = Parset()
    temp_parset.readFromFile(parset)
    temp_parset['msin'] = infile
    temp_parset['msout'] = outfile
    temp_parset.writeToFile(temp_parset_filename)

    try:
        # What is the '1' for? Required by DP3...
        check_call([executable, temp_parset_filename, '1'])
    finally:
        unlink(temp_parset_filename)

if __name__ == "__main__":
    from sys import argv
    run_dppp(argv[1], argv[2], argv[3])
