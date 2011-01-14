import sys
import subprocess
import glob
import os

from lofarpipe.support.lofarnode import LOFARnodeTCP

class thumbnail_combine(LOFARnodeTCP):
    def run(self, file_pattern, input_dir, output_file, clobber):
        # Returns 0 for success, 1 for faliure

        # Sanity checking checking
        if not os.path.exists(input_dir):
            return 1
        if os.path.exists(output_file):
            if clobber:
                os.unlink(output_file)
            else:
                return 1

        # Build list of input files
        input_files = glob.glob(os.path.join(input_dir, file_pattern))

        try:
            # Run "montage" command
            subprocess.check_call(['/usr/bin/montage'] + input_files + [output_file])
        except Exception, e:
            return 1

        return 0

if __name__ == "__main__":
    jobid, jobhost, jobport = sys.argv[1:4]
    sys.exit(thumbnail_combine(jobid, jobhost, jobport).run_with_stored_arguments())
