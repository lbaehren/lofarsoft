import sys, os, tempfile
from subprocess import check_call

# Cusine core
from cuisine.WSRTrecipe import WSRTrecipe

# IPython client
from IPython.kernel import client as IPclient

# Local helpers
import utilities

def make_vds(infile, clusterdesc, outfile, log_location):
    from pipeline.nodes.vdsmaker import make_vds
    return make_vds(infile, clusterdesc, outfile, log_location)

class vdsmaker(WSRTrecipe):
    def __init__(self):
        super(vdsmaker, self).__init__()
        self.optionparser.add_option(
            '-j', '--job-name', 
            dest="job_name",
            help="Job Name"
        )
        self.optionparser.add_option(
            '-r', '--runtime-directory', 
            dest="runtime_directory",
            help="Runtime Directory"
        )

    def go(self):
        ms_names = self.inputs['args']

        job_directory = "%s/jobs/%s" % (
            self.inputs['runtime_directory'],
            self.inputs['job_name']
        )

        try:
            tc  = IPclient.TaskClient(self.inputs['runtime_directory'] + '/task.furl')
            mec = IPclient.MultiEngineClient(self.inputs['runtime_directory'] + '/multiengine.furl')
            mec.push_function(
                dict(
                    make_vds=make_vds,
                    build_available_list=utilities.build_available_list,
                    clear_available_list=utilities.clear_available_list
                )
            )
        except:
            self.logger.error("Unable to initialise cluster")
            raise utilities.ClusterError

        # Build VDS files for each of the newly created MeasurementSets
        available_list = "%s%s" % (self.inputs['job_name'], "dppp-vds")
        mec.push(dict(ms_names=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        clusterdesc = "%s/lioff.clusterdesc" % (self.inputs['runtime_directory'],)
        tasks = []
        vdsnames = []
        for ms_name in ms_names:
            log_location = "%s/logs/%s/makevds.log" % (
                job_directory,
                os.path.basename(ms_name)
            )
#            vdsnames.append(tempfile.mkstemp(dir=job_directory)[1])
            vdsnames.append(
                "%s/%s.vds" % (self.inputs['directory'], os.path.basename(ms_name))
            )
            task = IPclient.StringTask(
                "result = make_vds(ms_name, clusterdesc, vds_name, log_location)",
                push=dict(
                    ms_name=ms_name,
                    vds_name=vdsnames[-1],
                    clusterdesc=clusterdesc,
                    log_location=log_location
                ),
                pull="result",
                depend=utilities.check_for_path,
                dependargs=(ms_name, available_list)
            )
            tasks.append(tc.run(task))
        tc.barrier(tasks)
        for task in tasks:
            tc.get_task_result(task)

        # Combine VDS files to produce GDS
        executable = '/app/lofar/dev/bin/combinevds'
        gvds_out   = "%s/%s" % (self.inputs['directory'], self.inputs['gds'])
        check_call([executable, gvds_out] + vdsnames)
#        for file in vdsnames:
#            os.unlink(file)
        self.outputs['gds'] = gvds_out

       # Save space on engines by clearing out old file lists
        mec.execute("clear_available_list(\"%s\")" % (available_list,))


if __name__ == '__main__':
    sys.exit(vdsmaker().main())
