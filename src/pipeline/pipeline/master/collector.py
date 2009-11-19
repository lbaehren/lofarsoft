# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.ipython import LOFARTask
from pipeline.support.clusterdesc import ClusterDesc
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
        self.optionparser.add_option(
            '--image-re',
            dest="image_re",
            help="Regular expression to match CASA image names"
            default="image*"
        )

    def go(self):
        self.logger.info("Starting data collector run")
        super(collector, self).go()

        if self.inputs['clobber'] and os.access(self.config.get('layout', 'results_directory'), os.W_OK):
            self.logger.info("Cleaning results directory")
            for file in glob.glob("%s/*" % self.config.get('layout', 'results_directory')):
                exec_string = ['/bin/rm', '-rf', '%s' % file]
                subprocess.check_call(exec_string)

        clusterdesc = ClusterDesc(
            self.config.get('cluster', 'clusterdesc')
        )
        results_dir = self.config.get('layout', 'results_directory'),
        self.logger.debug("Copying CASA images to to %s"  % (results_dir))
        for node in clusterdesc.get('ComputeNodes'):
            self.logger.debug("Node: %s" % (node))
            try:
                check_call(
                    [
                        "ssh",
                        node,
                        "--",
                        "mv",
                        "%s/%s/%s" % (
                            self._input_or_default('working_directory'),
                            self.inputs['job_name'],
                            self.inputs['image_re']
                        ),
                        results_dir
                    ])
            except CalledProcessError:
                self.logger.warn("No images moved from %s" % (node))
        
        self.logger.info("Generating FITS files")
        fits_files = []
        image_names = glob.glob("%s/%s" % (results_dir, self.inputs['image_re'])
        for filename in image_names:
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

