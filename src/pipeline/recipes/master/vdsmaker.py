from __future__ import with_statement
import sys, os, tempfile, errno
import subprocess

import lofarpipe.support.utilities as utilities
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.lofarnode import run_node

class vdsmaker(LOFARrecipe):
    def __init__(self):
        super(vdsmaker, self).__init__()
        self.optionparser.add_option(
            '-g', '--gvds',
            dest="gvds",
            help="Output file name"
        )
        self.optionparser.add_option(
            '--directory',
            dest="directory",
            help="Directory for output files"
        )
        self.optionparser.add_option(
            '--makevds',
            dest="makevds",
            help="makevds executable",
            default="/opt/LofIm/daily/lofar/bin/makevds"
        )
        self.optionparser.add_option(
            '--combinevds',
            dest="combinevds",
            help="combinevds executable",
            default="/opt/LofIm/daily/lofar/bin/combinevds"
        )

    def go(self):
        super(vdsmaker, self).go()

        ms_names = self.inputs['args']

        try:
            os.makedirs(self.inputs['directory'])
        except OSError, failure:
            if failure.errno != errno.EEXIST:
                raise

        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                make_vds=run_node,
                build_available_list=utilities.build_available_list,
                clear_available_list=utilities.clear_available_list
            )
        )
        self.logger.info("Pushed functions to cluster")

        # Build VDS files for each of the newly created MeasurementSets
        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (self.inputs['job_name'], "dppp-vds")
        mec.push(dict(filenames=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        clusterdesc = self.config.get('cluster', 'clusterdesc')

        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))
            tasks = []
            vdsnames = []
            for ms_name in ms_names:
                vdsnames.append(
                    "%s/%s.vds" % (self.inputs['directory'], os.path.basename(ms_name))
                )
                task = LOFARTask(
                    "result = make_vds(ms_name, clusterdesc, vds_name, executable)",
                    push=dict(
                        recipename=self.name,
                        nodepath=os.path.dirname(self.__file__.replace('master', 'nodes')),
                        ms_name=ms_name,
                        vds_name=vdsnames[-1],
                        clusterdesc=clusterdesc,
                        executable=self.inputs['makevds'],
                        loghost=loghost,
                        logport=logport
                    ),
                    pull="result",
                    depend=utilities.check_for_path,
                    dependargs=(ms_name, available_list)
                )
                self.logger.info("Scheduling processing of %s" % (ms_name,))
                tasks.append(tc.run(task))
            self.logger.info("Waiting for all makevds tasks to complete")
            tc.barrier(tasks)

        failure = False
        for task in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                self.logger.warn("Task %s failed" % (task))
                self.logger.warn(res)
                self.logger.warn(res.failure.getTraceback())
                failure = True
        if failure:
            return 1

        # Combine VDS files to produce GDS
        self.logger.info("Combining VDS files")
        executable = self.inputs['combinevds']
        gvds_out   = "%s/%s" % (self.inputs['directory'], self.inputs['gvds'])
        try:
            # When we get an OSError 24, it seems we have
            # /sys/devices/virtual/vtconsole/vtcon0 open many times.
            # Speculating this is due to a failure of subprocess to write to
            # the console (why?); try writing to pipes instead.
            command = [executable, gvds_out] + vdsnames
            combineproc = subprocess.Popen(
                command,
                close_fds=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
            sour, serr = combineproc.communicate()
            if combineproc.returncode != 0:
                raise subprocess.CalledProcessError(combineproc.returncode, command)
            self.outputs['gvds'] = gvds_out
            return 0
        except subprocess.CalledProcessError, cpe:
            self.logger.exception("combinevds failed with status %d: %s" % (cpe.returncode, serr))
            return 1
        except OSError, failure:
            # This has previously bombed out with OSError 24: too many open
            # files. Attempt to print something useful if that happens again.
            try:
                if failure.errno == errno.EMFILE:
                    count = 0
                    for x in xrange(0, os.sysconf('SC_OPEN_MAX')):
                        try:
                            self.logger.debug("open file %d: %s" (x, str(os.fstat(x))))
                            count += 1
                        except:
                            pass
                    self.logger.info("Had %d open files" % (count,))
                else:
                    self.logger.exception(failure)
            finally:
                return 1
        finally:
            # Save space on engines by clearing out old file lists
            mec.execute("clear_available_list(\"%s\")" % (available_list,))
        self.logger.info("vdsmaker done")

if __name__ == '__main__':
    sys.exit(vdsmaker().main())
