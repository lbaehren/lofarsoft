# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities
import os.path, glob, subprocess, sys, numpy, pyfits

class collector(LOFARrecipe):
    """
    Collect images into results directory.
    Convert to fits files.
    Average.
    """

    def go(self):
        self.logger.info("Starting data collector run")
        super(collector, self).go()

        tc, mec = self._get_cluster()

        self.logger.info("Copying images to results directory")
        mec.execute("import subprocess")
        cmd_string = "subprocess.check_call([\"cp\", \"-r\", \"%s/image*\", \"%s\")" % (
            os.path.join(
                self.config.get('mwimager', 'working_directory'),
                self.inputs['job_name']
            ),
            self.config.get('layout', 'results_directory')
        )
        mec.execute('cmd_string')
        
        self.logger.info("Generating FITS files")
        fits_files = []
        for filename in glob.glob(
            "%s/image.i.*" % (self.config.get('layout', 'results_directory')
        ):
            self.logger.debug(filename)
            subband = os.path.basename(filename).split('.')[2]
            output = os.path.join(
                self.config.get('layout', 'results_directory'),
                "%s.fits" % (subband)
            )
            fits_files.append(output)
            subprocess.check_call([
                self.config.get('fitswriter', 'executable'),
                'in=%s' % (filename),
                'out=%s' % (output)
            ])

        self.logger.info("Averaging results")
        result = reduce(
            numpy.add,
            (pyfits.getdata(file) for file in fits_files)
        ) / len(input_names)

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

if __name__ = '__main__':
    sys.exit(locals()[locals()['__file__'].split('.')[0]]().main())

