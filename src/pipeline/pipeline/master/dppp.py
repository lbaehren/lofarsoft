import sys, os, tempfile
from subprocess import check_call

# Cusine core
from cuisine.WSRTrecipe import WSRTrecipe
from cuisine.ingredient import WSRTingredient

# IPython client
from IPython.kernel import client as IPclient

# Local helpers
import utilities

#from pipeline.nodes.dppp import run_dppp
#from pipeline.nodes.vdsmaker import make_vds

def run_dppp(ms_name, ms_outname, parset, log_location):
    # Run on engine to process data with DPPP
    from pipeline.nodes.dppp import run_dppp
    return run_dppp(ms_name, ms_outname, parset, log_location)

def make_vds(infile, clusterdesc, outfile, log_location):
    from pipeline.nodes.vdsmaker import make_vds
    return make_vds(infile, clusterdesc, outfile, log_location)


class TestPipeline(WSRTrecipe):
    def __init__(self):
        super(TestPipeline, self).__init__()
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
        try:
            gvds = utilities.get_parset(self.inputs['gvds'])
        except:
            self.logger.error("Unable to read G(V)DS file")
            raise

        job_directory = "%s/jobs/%s" % (
            self.inputs['runtime_directory'],
            self.inputs['job_name']
        )
        try:
            parset = "%s/dppp.parset" % (job_directory,)
            if not os.access(parset, os.R_OK):
                raise IOError
        except IOError:
            self.logger.error("DPPP parset not found")
            raise
            
        try:
            tc  = IPclient.TaskClient(self.inputs['runtime_directory'] + '/task.furl')
            mec = IPclient.MultiEngineClient(self.inputs['runtime_directory'] + '/multiengine.furl')
            mec.push_function(
                dict(
                    run_dppp=run_dppp,
                    make_vds=make_vds,
                    build_available_list=utilities.build_available_list,
                    clear_available_list=utilities.clear_available_list
                )
            )
        except:
            self.logger.error("Unable to initialise cluster")
            raise utilities.ClusterError

        # We read the GVDS file to find the names of all the data files we're
        # going to process, then push this list out to the engines so they can
        # let us know which we have available
        ms_names = [
            gvds["Part%d.FileName" % (part_no,)] 
            for part_no in xrange(int(gvds["NParts"]))
        ]

        # Construct list of available files on engines
        available_list = "%s%s" % (self.inputs['job_name'], "dppp")
        mec.push(dict(ms_names=ms_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )

        tasks = []
        outnames = []
        for ms_name in ms_names:
            outnames.append(ms_name + ".dppp")
            log_location = "%s/logs/%s/dppp.log" % (
                job_directory,
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
            tasks.append(tc.run(task))
        tc.barrier(tasks)
        for task in tasks:
            tc.get_task_result(task)

        # Save space on engines by clearing out old file lists
        mec.execute("clear_available_list(\"%s\")" % (available_list,))

        # Build VDS files for each of the newly created MeasurementSets
        available_list = "%s%s" % (self.inputs['job_name'], "dppp-vds")
        mec.push(dict(ms_names=outnames))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        clusterdesc = "%s/lioff.clusterdesc" % (self.inputs['runtime_directory'],)
        tasks = []
        vdsnames = []
        for ms_name in outnames:
            log_location = "%s/logs/%s/makevds.log" % (
                job_directory,
                os.path.basename(ms_name)
            )
            vdsnames.append(tempfile.mkstemp(dir=job_directory)[1])
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
        gvds_out   = '%s/dppp.gds' % (job_directory,)
        check_call([executable, gvds_out] + vdsnames)
#        for file in vdsnames:
#            os.unlink(file)
        self.outputs['gds'] = gvds_out


if __name__ == '__main__':
    sys.exit(TestPipeline().main())
