from __future__ import with_statement
import os, imp, logging, errno

from pipeline.support.lofarnode import LOFARnode
from pipeline.support.utilities import log_time

class qcheck_node(LOFARnode):
    """
    Run quality check modules on an image.
    """
    def run(self, infile, pluginlist, outputdir):
        with log_time(self.logger):
            self.logger.info("Processing: %s" % (infile))

            try:
                os.makedirs(outputdir)
            except OSError, failure:
                if failure.errno != errno.EEXIST:
                    raise

            file_handler = logging.FileHandler(os.path.join(
                    outputdir,
                    os.path.basename(infile) + ".qcheck.log"
                ),
                mode='w'
            )
            file_handler.setFormatter(logging.Formatter("%(message)s"))
            file_logger = logging.getLogger('main')
            file_logger.addHandler(file_handler)
            file_logger.setLevel(logging.INFO)
            pipeline_logger = logging.getLogger(self.logger.name + "." + os.path.basename(infile))
            pipeline_logger.setLevel(logging.WARN)

            loggers = {'main': file_logger, 'warn': pipeline_logger}

            for plugin in pluginlist:
                try:
                    qcheck = imp.load_source('qcheck', plugin)
                except ImportError:
                    self.logger.warn("Quality check module (%s) not found" % (plugin))
                try:
                    qcheck.run(infile, outputdir=outputdir, loggers=loggers)
                except Exception, e:
                    self.logger.warn("Quality check failed on %s" % (infile))
                    self.logger.exception(str(e))

            # Tidy up for the next image
            file_handler.flush()
            loggers['main'].remove_handler(file_handler)

        return 0
