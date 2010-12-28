# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call, CalledProcessError
from dateutil.parser import parse as parse_date
from datetime import timedelta
import os.path, tempfile, shutil, time

from pipeline.support.lofarnode import LOFARnode
from pipeline.support.utilities import patch_parset, create_directory, log_time
from pipeline.support.lofarexceptions import ExecutableMissing
import pipeline.support.utilities as utilities

CASA_DATE_FORMAT = "%Y/%m/%d/%H:%M:%S.000"

class casapy_node(LOFARnode):
    def run(self, infile, parset, start_time, end_time, increment):
        # Time execution of this job
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))

            start_time = parse_date(start_time)
            end_time   = parse_date(end_time)

            self.logger.debug("Start time: %s, end time: %s" % (str(start_time), str(end_time)))
            increment = timedelta(0, increment)

            process_start = start_time
            while process_start < end_time:
                process_end = process_start + increment
                if process_end > end_time:
                    td = end_time - process_start
                    self.logger.info(
                        "Note: final image is %.3f seconds long" % (
                            td.days * 86400 + td.seconds + td.microseconds / 1e6
                        )
                    )
                    process_end = end_time
                time_range = "\'%s~%s\'" % (
                    process_start.strftime(CASA_DATE_FORMAT),
                    process_end.strftime(CASA_DATE_FORMAT)
                )
                self.logger.debug("Now processing %s" % (time_range))

                tmp_parset_filename = patch_parset(
                    parset, {
                        'Selection.timerange': time_range,
                        'Images.name': '-' + str(int(time.mktime(process_start.timetuple()))),
                        'dataset': infile
                    }
                )

                try:
                    result = check_call([
                        os.path.expanduser('~rol/sw/bin/casapy'),
                        tmp_parset_filename,
                    ])
                except CalledProcessError, e:
                    self.logger.error(str(e))
                    self.logger.error("Failed dataset was %s %s" % (infile, time_range))
                    raise Exception
                finally:
                    # Clean up tempoerary files.
                    os.unlink(tmp_parset_filename)

                process_start += increment

            return result
