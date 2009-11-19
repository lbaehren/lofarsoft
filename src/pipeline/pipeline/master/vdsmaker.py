import sys, os, tempfile
from subprocess import check_call
from ConfigParser import SafeConfigParser as ConfigParser

# Cusine core
from cuisine.WSRTrecipe import WSRTrecipe

# IPython client
from IPython.kernel import client as IPclient

# Local helpers
import utilities

# Root directory for config file
from pipeline import __path__ as config_path

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
        config = ConfigParser({
            "job_name": self.inputs["job_name"],
            "runtime_directory": self.inputs["runtime_directory"]
        })
        config.read("%s/pipeline.cfg" % (config_path[0],))

        ms_names = self.inputs['args']

        job_directory = config.get("layout", "job_directory")

        try:
            tc  = IPclient.TaskClient(config.get('cluster', 'task_furl'))
            mec = IPclient.MultiEngineClient(config.get('cluster', 'multiengine_furl'))
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

        self.logger.info("Cluster initialised")

        # Build VDS files for each of the newly created MeasurementSets
        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (self.inputs['job_name'], "dppp-vds")
        mec.push(dict(ms_names=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        clusterdesc = config.get('cluster', 'clusterdesc')
        tasks = []
        vdsnames = []
        for ms_name in ms_names:
            log_location = "%s/%s/makevds.log" % (
                config.get("layout", "log_directory"),
                os.path.basename(ms_name)
            )
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
            self.logger.info("Scheduling processing of %s" % (ms_name,))
            tasks.append(tc.run(task))
        self.logger.info("Waiting for all makevds tasks to complete")
        tc.barrier(tasks)
        for task in tasks:
            tc.get_task_result(task)

        # Combine VDS files to produce GDS
        self.logger.info("Combining VDS files")
        executable = config.get('VDS', 'combinevds')
        gvds_out   = "%s/%s" % (self.inputs['directory'], self.inputs['gds'])
        check_call([executable, gvds_out] + vdsnames)
#        for file in vdsnames:
#            os.unlink(file)
        self.outputs['gds'] = gvds_out

       # Save space on engines by clearing out old file lists
        mec.execute("clear_available_list(\"%s\")" % (available_list,))


if __name__ == '__main__':
    sys.exit(vdsmaker().main())
