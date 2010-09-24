# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterdesc import ClusterDesc, get_compute_nodes
import lofarpipe.support.utilities as utilities
import pyrap.images
from skim.main import run as create_hdf5
import os, os.path, glob, subprocess, sys, numpy
import shutil, errno, re, logging, imp

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
            help="Base filename for averaged images"
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
        for node in get_compute_nodes(clusterdesc):
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
                subprocess.check_call(exec_string, close_fds=True)
            except subprocess.CalledProcessError:
                self.logger.warn("No images moved from %s" % (node))
        
        image_names = glob.glob("%s/%s" % (results_dir, self.inputs['image_re']))
        if len(image_names) > 0:
            self.logger.info("Averaging results")
            result = reduce(
                numpy.add,
                (pyrap.images.image(file).getdata() for file in image_names)
            ) / len(image_names)

            self.logger.info("Writing averaged files")
            averaged_file = os.path.join(
                        self.config.get('layout', 'results_directory'),
                        self.inputs['averaged_name']
            )
            # Output for the averaged image.
            # Use the coordinate system from SB0.
            output = pyrap.images.image(
                averaged_file + ".img", values=result,
                coordsys=pyrap.images.image(image_names[0]).coordinates()
            )
            self.logger.info("Wrote: %s" % (averaged_file + ".img",))
            output.tofits(averaged_file + ".fits")
            self.logger.info("Wrote: %s" % (averaged_file + ".fits",))
            self.outputs['data'] = (averaged_file + ".fits",)
        else:
            self.logger.info("No images found; not averaging")
            self.outputs['data'] = None

        self.logger.info("Generating FITS files")
        fits_files = []
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
                stderr=subprocess.STDOUT,
                close_fds=True
            )

        self.logger.info("Creating HDF5 file")
        hdf5logger = logging.getLogger(self.logger.name + ".hdf5")
        hdf5logger.setLevel(logging.INFO)
        create_hdf5(
            self.config.get('layout', 'job_directory'),
            self.inputs['start_time'],
            hdf5logger
        )

        return 0

if __name__ == '__main__':
    sys.exit(collector().main())
