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
        self.optionparser.add_option(
            '--unlink',
            help="Unlink VDS files after combining",
            default="True"
        )

    def go(self):
        super(vdsmaker, self).go()

        ms_names = self.inputs['args']
        if self.inputs['unlink'] == "False":
            self.inputs['unlink'] = False

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

        # Save space on engines by clearing out old file lists
        mec.execute("clear_available_list(\"%s\")" % (available_list,))
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
        gvds_out = self.inputs['gvds']
        try:
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
        except subprocess.CalledProcessError, cpe:
            self.logger.exception("combinevds failed with status %d: %s" % (cpe.returncode, serr))
            failure = True
        except OSError, failure:
            self.logger.warn("Caught OSError")
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
                elif failure.errno == errno.ENOMEM:
                    self.logger.info("Failed to run: %s" % str(command))
                    import operator
                    total = reduce(operator.add, (len(x) for x in command))
                    self.logger.debug("Num args: %d, num characters: %d" % (len(command), total))
                    try:
                        p = subprocess.Popen(['free'], stdout=subprocess.PIPE)
                        sout, serr = p.communicate()
                        self.logger.free(sout)
                    except:
                        self.logger.warn("Failed to spawn free")
                    self.logger.exception(failure)
                else:
                    self.logger.exception(failure)
            finally:
                failure = True
        finally:
            if self.inputs["unlink"]:
                self.logger.debug("Unlinking temporary files")
                for file in vdsnames:
                    os.unlink(file)
            self.logger.info("vdsmaker done")
        if failure:
            return 1
        else:
            return 0

if __name__ == '__main__':
    sys.exit(vdsmaker().main())
