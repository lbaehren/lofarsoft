import sys, os
from ConfigParser import SafeConfigParser as ConfigParser

# Cusine core
from cuisine.WSRTrecipe import WSRTrecipe
from cuisine.ingredient import WSRTingredient

# IPython client
from IPython.kernel import client as IPclient

# Local helpers
import utilities

# Root directory for config file
from pipeline import __path__ as config_path

def run_dppp(ms_name, ms_outname, parset, log_location):
    # Run on engine to process data with DPPP
    from pipeline.nodes.dppp import run_dppp
    return run_dppp(ms_name, ms_outname, parset, log_location)

class dppp(WSRTrecipe):
    def __init__(self):
        super(dppp, self).__init__()
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
        self.optionparser.add_option(
            '-g', '--g(v)ds-file',
            dest="gvds",
            help="G(V)DS file describing data to be processed"
        )

    def go(self):
        self.logger.info("Starting DPPP run")

        config = ConfigParser({
            "job_name": self.inputs["job_name"],
            "runtime_directory": self.inputs["runtime_directory"]
        })
        config.read("%s/pipeline.cfg" % (config_path[0],))

        # If we didn't get a GDS file on the command line, look for one in the
        # job's default VDS directory
        if not self.inputs['gvds']:
            self.inputs['gvds'] = "%s/%s.gvds" % (
                config.get('layout', 'vds_directory'),
                self.inputs['job_name']
            )
            self.logger.info("Using %s for initial data" % (self.inputs['gvds'],))
        try:
            gvds = utilities.get_parset(self.inputs['gvds'])
        except:
            self.logger.error("Unable to read G(V)DS file")
            raise

        job_directory = config.get("layout", "job_directory")

        try:
            parset = "%s/dppp.parset" % (
                config.get("layout", "parset_directory"),
            )
            if not os.access(parset, os.R_OK):
                raise IOError
        except IOError:
            self.logger.error("DPPP parset not found")
            raise
            
        try:
            tc  = IPclient.TaskClient(config.get('cluster', 'task_furl'))
            mec = IPclient.MultiEngineClient(config.get('cluster', 'multiengine_furl'))
            mec.push_function(
                dict(
                    run_dppp=run_dppp,
                    build_available_list=utilities.build_available_list,
                    clear_available_list=utilities.clear_available_list
                )
            )
        except:
            self.logger.error("Unable to initialise cluster")
            raise utilities.ClusterError

        self.logger.info("Cluster initialised")

        # We read the GVDS file to find the names of all the data files we're
        # going to process, then push this list out to the engines so they can
        # let us know which we have available
        ms_names = [
            gvds["Part%d.FileName" % (part_no,)] 
            for part_no in xrange(int(gvds["NParts"]))
        ]

        # Construct list of available files on engines
        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (self.inputs['job_name'], "dppp")
        mec.push(dict(ms_names=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )

        tasks = []
        outnames = []
        for ms_name in ms_names:
            outnames.append(ms_name + ".dppp")
            log_location = "%s/%s/dppp.log" % (
                config.get('layout', 'log_directory'),
                os.path.basename(ms_name)
            )
            task = IPclient.StringTask(
                "result = run_dppp(ms_name, ms_outname, parset, log_location)",
                push=dict(
                    ms_name=ms_name,
                    ms_outname=outnames[-1],
                    parset=parset,
                    log_location=log_location
                ),
                pull="result",
                depend=utilities.check_for_path,
                dependargs=(ms_name, available_list)
            )
            self.logger.info("Scheduling processing of %s" % (ms_name,))
            tasks.append(tc.run(task))
        self.logger.info("Waiting for all DPPP tasks to complete")
        tc.barrier(tasks)
        for task in tasks:
            print tc.get_task_result(task)

        # Save space on engines by clearing out old file lists
        mec.execute("clear_available_list(\"%s\")" % (available_list,))

        # Now set up a vdsmaker recipe to build a GDS file describing the
        # processed data
        self.logger.info("Calling vdsmaker")
        inputs = WSRTingredient()
        inputs['job_name'] = self.inputs['job_name']
        inputs['runtime_directory'] = config.get('DEFAULT', 'runtime_directory')
        inputs['directory'] = config.get('layout', 'vds_directory')
        inputs['gds'] = config.get('DPPP', 'gds_output')
        inputs['args'] = outnames
        outputs = WSRTingredient()
        sts = self.cook_recipe('vdsmaker', inputs, outputs)
        
        self.outputs['gds'] = outputs['gds']


if __name__ == '__main__':
    sys.exit(dppp().main())
