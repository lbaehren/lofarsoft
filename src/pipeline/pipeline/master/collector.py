# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.ipython import LOFARTask
import pipeline.support.utilities as utilities
import os, os.path, glob, subprocess, sys, numpy, pyfits, shutil

class collector(LOFARrecipe):
    """
    Collect images into results directory.
    Convert to fits files.
    Average.

    Outstanding issue: breaks if the results directory is already
    populated and the --clobber option isn't set.
    """

    def __init__(self):
        super(collector, self).__init__()
        self.optionparser.add_option(
            '--clobber',
            dest="clobber",
            help="Clobber results directory if already exists",
            action="store_true"
        )

    def go(self):
        self.logger.info("Starting data collector run")
        super(collector, self).go()

        if self.inputs['clobber'] and os.access(self.config.get('layout', 'results_directory'), os.W_OK):
            self.logger.info("Cleaning results directory")
            for file in glob.glob("%s/*" % self.config.get('layout', 'results_directory')):
                exec_string = ['/bin/rm', '-rf', '%s' % file]
                subprocess.check_call(exec_string)

        image_names = self.inputs['args']
        tc, mec = self._get_cluster()

        self.logger.info("Copying images to results directory")
        mec.push_function(
            {
                "build_available_list": utilities.build_available_list,
                "clear_available_list": utilities.clear_available_list
            }
        )
        self.logger.info("Building list of data available on engines")
        available_list = "%s%s" % (
            self.inputs['job_name'], self.__class__.__name__
        )
        mec.push(dict(filenames=image_names))
        mec.execute(
            "build_available_list(\"%s\")" % (available_list,)
        )
        mec.execute("import shutil")
        tasks = []
        for image_name in image_names:
            dest = os.path.join(
                self.config.get('layout', 'results_directory'),
                os.path.basename(image_name)
            )
            exec_string = "shutil.copytree(\"%s\", \"%s\")" % (image_name, dest)
            self.logger.debug(exec_string)
            task = LOFARTask(
                exec_string,
                depend=utilities.check_for_path,
                dependargs=(image_name, available_list)
            )
            self.logger.info("Scheduling processing of %s" % (image_name,))
            tasks.append(tc.run(task))
        tc.barrier(tasks)
        for task in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                print res.failure
        mec.execute("clear_available_list(\"%s\")" % (available_list,))
        
        self.logger.info("Generating FITS files")
        fits_files = []
        for filename in glob.glob(
            "%s/image.i.*" % (self.config.get('layout', 'results_directory'))
        ):
            self.logger.debug(filename)
            subband = os.path.basename(filename).split('.')[2]
            output = os.path.join(
                self.config.get('layout', 'results_directory'),
                "%s.fits" % (subband)
            )
            fits_files.append(output)
            subprocess.check_call(
                [
                    self.config.get('fitswriter', 'executable'),
                    'in=%s' % (filename),
                    'out=%s' % (output)
                ],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT
            )

        self.logger.info("Averaging results")
        result = reduce(
            numpy.add,
            (pyfits.getdata(file) for file in fits_files)
        ) / len(fits_files)

        self.logger.info("Writing averaged FITS file")
        hdulist = pyfits.HDUList(pyfits.PrimaryHDU(result))
        hdulist[0].header = pyfits.getheader(fits_files[0])
        hdulist.writeto(
            os.path.join(
                self.config.get('layout', 'results_directory'),
                self.config.get('fitswriter', 'averaged')
            )
        )
                
        return 0

if __name__ == '__main__':
    sys.exit(locals()[locals()['__file__'].split('.')[0]]().main())

