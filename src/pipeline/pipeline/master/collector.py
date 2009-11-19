# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.ipython import LOFARTask
from pipeline.support.clusterdesc import ClusterDesc
import pipeline.support.utilities as utilities
import os, os.path, glob, subprocess, sys, numpy, pyfits, shutil, errno, re

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
            '--image-re',
            dest="image_re",
            help="Regular expression to match CASA image names",
        )
        self.optionparser.add_option(
            '--working-directory',
            dest="working_directory",
            help="Working directory containing images on compute nodes",
        )
        self.optionparser.add_option(
            '--image2fits',
            dest="image2fits",
            help="Location of image2fits tool (from casacore)"
        )
        self.optionparser.add_option(
            '--averaged-name',
            dest="averaged_name",
            help="Filename for averaged FITS image"
        )

    def go(self):
        self.logger.info("Starting data collector run")
        super(collector, self).go()

        clusterdesc = ClusterDesc(
            self.config.get('cluster', 'clusterdesc')
        )
        results_dir = self.config.get('layout', 'results_directory')
        try:
            os.makedirs(results_dir)
        except OSError, failure:
            if failure.errno != errno.EEXIST:
                raise

        self.logger.debug("Copying CASA images to to %s"  % (results_dir))
        for node in clusterdesc.get('ComputeNodes'):
            self.logger.debug("Node: %s" % (node))
            try:
                exec_string = [
                            "ssh",
                            node,
                            "--",
                            "cp",
                            "-r",
                            "%s/%s/%s" % (
                                self.inputs['working_directory'],
                                self.inputs['job_name'],
                                self.inputs['image_re']
                            ),
                            results_dir
                    ]
                self.logger.info(exec_string)
                subprocess.check_call(exec_string)
            except subprocess.CalledProcessError:
                self.logger.warn("No images moved from %s" % (node))
        
        self.logger.info("Generating FITS files")
        fits_files = []
        image_names = glob.glob("%s/%s" % (results_dir, self.inputs['image_re']))
        for filename in image_names:
            self.logger.debug(filename)
            subband = re.search('(SB\d+)', os.path.basename(filename)).group()
            output = os.path.join(
                self.config.get('layout', 'results_directory'),
                "%s.fits" % (subband)
            )
            fits_files.append(output)
            subprocess.check_call(
                [
                    self.inputs['image2fits'],
                    'in=%s' % (filename),
                    'out=%s' % (output)
                ],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT
            )

        if len(fits_files) > 0:
            self.logger.info("Averaging results")
            result = reduce(
                numpy.add,
                (pyfits.getdata(file) for file in fits_files)
            ) / len(fits_files)

            self.logger.info("Writing averaged FITS file")
            hdulist = pyfits.HDUList(pyfits.PrimaryHDU(result))
            hdulist[0].header = pyfits.getheader(fits_files[0])
            averaged_file = os.path.join(
                        self.config.get('layout', 'results_directory'),
                        self.inputs['averaged_name']
                )
            hdulist.writeto(averaged_file)
            self.outputs['data'] = (averaged_file,)
            self.logger.info("Wrote: %s" % self.outputs['data'])
        else:
            self.logger.info("No FITS image found; not averaging")
            self.ouputs['data'] = None
                
        return 0

if __name__ == '__main__':
    sys.exit(locals()[locals()['__file__'].split('.')[0]]().main())

