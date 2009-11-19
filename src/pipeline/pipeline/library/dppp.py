# Python standard library
from subprocess import check_call
from tempfile import mkstemp
from os import unlink

# External
from cuisine.parset import Parset

def run_dppp(infile, outfile, parset):
    executable = "CS1_IDPPP"

    # We need to patch the parset with the correct input/output MS names.
    temp_parset_filename = mkstemp()[1]
    temp_parset = Parset().readFromFile(parset)
    temp_parset['msin'] = infile
    temp_parset['msout'] = outfile
    temp_parset.writeToFile(temp_parset_filename)

    try:
        check_call([executable, temp_parset_filename])
    finally:
        unlink(temp_parset_filename)

if __name__ == "__main__":
    run_dppp(ARGV[0], ARGV[1], ARGV[2])
