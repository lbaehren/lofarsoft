from __future__ import with_statement
import sys, os, tempfile
from subprocess import check_call, CalledProcessError

# Local helpers
from pipeline.support.ipython import LOFARTask
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities
from pipeline.support.clusterlogger import clusterlogger

def make_vds(infile, clusterdesc, outfile, executable):
    from pipeline.nodes.vdsmaker import makevds_node
    return makevds_node(loghost=loghost, logport=logport).run(
        infile,
        clusterdesc,
        outfile,
        executable
    )

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
        job_directory = self.config.get("layout", "job_directory")

        tc, mec = self._get_cluster()
        mec.push_function(
            dict(
                make_vds=make_vds,
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

        for task in tasks:
            tc.get_task_result(task)

        # Combine VDS files to produce GDS
        self.logger.info("Combining VDS files")
        executable = self.inputs['combinevds']
        gvds_out   = "%s/%s" % (self.inputs['directory'], self.inputs['gvds'])
        try:
            check_call([executable, gvds_out] + vdsnames)
            self.outputs['gvds'] = gvds_out
            return 0
        except CalledProcessError:
            self.logger.exception("Call to combinevds failed")
            return 1
        finally:
            # Save space on engines by clearing out old file lists
            mec.execute("clear_available_list(\"%s\")" % (available_list,))
        self.logger.info("vdsmaker done")

if __name__ == '__main__':
    sys.exit(vdsmaker().main())
