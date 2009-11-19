import sys, datetime, logging, os.path
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
import pipeline.support.utilities as utilities

class sip(LOFARrecipe):
    """
    The LOFAR Standard Imaging Pipeline.
    """
    def go(self):
        super(sip, self).go()

        # Set up logging to file
        handler = logging.FileHandler('%s/pipeline.log.%s' % (
                self.config.get("layout", "log_directory"),
                str(datetime.datetime.now())
            )
        )
        formatter = logging.Formatter(
            "%(asctime)s - %(levelname)s - %(name)s:  %(message)s",
            "%Y-%m-%d %H:%M:%S"
        )
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)

        self.logger.info("Standard Imaging Pipeline starting.")

        self.logger.info("Reading VDS file")
        inputs = LOFARinput(self.inputs)
        inputs['gvds'] = self.config.get("layout", "gvds")
        outputs = LOFARoutput()
        if self.cook_recipe('vdsreader', inputs, outputs):
            self.logger.warn("vdsreader reports failure")
            return 1
        ms_names = outputs['ms_names']

        self.logger.info("Copying data to compute nodes")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = ms_names
        inputs['destination'] = os.path.join(
            self.config.get("mwimager", "working_directory"),
            self.inputs['job_name']
        )
        outputs = LOFARoutput()
        if self.cook_recipe('copier', inputs, outputs):
            self.logger.warn("copier reports failure")
            return 1
        ms_names = outputs['ms_names']

        self.logger.info("Calling DPPP")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = ms_names
        outputs = LOFARoutput()
        if self.cook_recipe('dppp', inputs, outputs):
            self.logger.warn("DPPP reports failure")
            return 1
        ms_names = outputs['ms_names']

        # Drop known-bad stations
        self.logger.info("Calling excluder")
        inputs = LOFARinput(self.inputs)
        inputs['station'] = "DE001LBA"
        inputs['suffix'] = ".excluded"
        inputs['args'] = ms_names
        outputs = LOFARoutput()
        if self.cook_recipe('excluder', inputs, outputs):
            self.logger.warn("excluder reports failure")
            return 1
        excluded_outnames = outputs['data']

        # Trim off any bad section of the data
        self.logger.info("Calling trimmer")
        inputs = LOFARinput(self.inputs)
        inputs['start_seconds'] = 300.0
        inputs['end_seconds'] = 300.0
        inputs['suffix'] = ".trimmed"
        inputs['args'] = excluded_outnames
        outputs = LOFARoutput()
        if self.cook_recipe('trimmer', inputs, outputs):
            self.logger.warn("trimmer reports failure")
            return 1
        trimmed_outnames = outputs['data']

        # Now set up a colmaker recipe to insert missing columns in the
        # processed data
#        self.logger.info("Calling colmaker")
#        inputs = LOFARinput(self.inputs)
#        inputs['args'] = trimmed_outnames
#        outputs = LOFARoutput()
#        if self.cook_recipe('colmaker', inputs, outputs):
#            self.logger.warn("colmaker reports failure")
#            return 1

        # Now set up a vdsmaker recipe to build a GDS file describing the
        # processed data
        self.logger.info("Calling vdsmaker")
        inputs = LOFARinput(self.inputs)
        inputs['directory'] = self.config.get('layout', 'vds_directory')
        inputs['gvds'] = self.config.get('dppp', 'gvds_output')
        inputs['args'] = trimmed_outnames
        outputs = LOFARoutput()
        if self.cook_recipe('vdsmaker', inputs, outputs):
            self.logger.warn("vdsmaker reports failure")
            return 1
        gvds = outputs['gvds']

        self.logger.info("Calling BBS")
        inputs = LOFARinput(self.inputs)
        inputs['force'] = True
        inputs['gvds'] = gvds
        outputs = LOFARoutput()
        if self.cook_recipe('bbs', inputs, outputs):
            self.logger.warn("BBS reports failure")
            return 1

        # Trim off any bad section of the data
        self.logger.info("Calling local flagger")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = processed_filenames
        inputs['n_factor'] = 2.0
        inputs['suffix'] = ".flagged"
        outputs = LOFARoutput()
        if self.cook_recipe('flagger', inputs, outputs):
            self.logger.warn("flagger reports failure")
            return 1
        flagged_outnames = outputs['data']

        self.logger.info("Calling vdsmaker")
        inputs = LOFARinput(self.inputs)
        inputs['directory'] = self.config.get('layout', 'vds_directory')
        inputs['gvds'] = "%s.flagged.gvds" % (self.inputs['job_name'],)
        inputs['args'] = flagged_outnames
        outputs = LOFARoutput()
        if self.cook_recipe('vdsmaker', inputs, outputs):
            self.logger.warn("vdsmaker reports failure")
            return 1
        self.outputs['gvds'] = outputs['gvds']

        self.logger.info("Calling MWImager")
        inputs = LOFARinput(self.inputs)
        inputs['gvds'] = self.outputs['gvds']
        outputs = LOFARoutput()
        if self.cook_recipe('mwimager', inputs, outputs):
            self.logger.warn("MWImager reports failure")
            return 1
        # The SIP will return the names of the generated images
        self.outputs['images'] = outputs['images']

        self.logger.info("Calling source extractor")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = outputs['images']
        outputs = LOFARoutput()
        if self.cook_recipe('sextractor', inputs, outputs):
            self.logger.warn("sextractor reports failure")
            return 1

        return 0

if __name__ == '__main__':
    sys.exit(sip().main())
