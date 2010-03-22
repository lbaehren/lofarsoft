from __future__ import with_statement
from contextlib import closing

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
import lofarpipe.support.utilities as utilities
import os, os.path, glob, subprocess, sys, numpy, shutil, errno, re

# SE tools
from tkp_lib.dataset   import DataSet
from tkp_lib.image     import ImageData
from tkp_lib.accessors import FitsFile
from tkp_lib.dbplots   import plotAssocCloudByXSource
from tkp_lib.dbregion  import createRegionByImage
import tkp_lib.database as database

associations = """
SELECT
    x1.xtrsrcid, x1.ra, x1.decl, x1.i_peak, x1.i_int, c.catname, c1.ra, c1.decl, a1.assoc_distance_arcsec
FROM
    extractedsources x1
LEFT OUTER JOIN
    assoccatsources a1 ON x1.xtrsrcid = a1.xtrsrc_id
LEFT OUTER JOIN
    catalogedsources c1 ON a1.assoc_catsrc_id = c1.catsrcid
LEFT OUTER JOIN
    catalogs c ON c.catid = c1.cat_id
WHERE
    image_id = %d
ORDER BY
    x1.I_Peak;
"""

class simple_se(LOFARrecipe):
    """
    Run source extraction on FITS images on the front-end.
    Dump ds9 region files of found sources & WENSS sources.
    Dump text file of assocations with catalogue sources.

    Designed to be run e.g. on an averaged image at the end of a pipeline run.
    """

    def __init__(self):
        super(simple_se, self).__init__()
        self.optionparser.add_option(
            '--detected-regions',
            dest="detected_regions",
            help="Filename for region file of local detections",
            default="detected.reg"
        )
        self.optionparser.add_option(
            '--wenss-regions',
            dest="wenss_regions",
            help="Filename for region file of WENSS detections",
            default="wenss.reg"
        )
        self.optionparser.add_option(
            '--associations',
            dest="associations",
            help="Filename for association list",
            default="association.list"
        )

    def go(self):
        self.logger.info("Starting source identification")
        super(simple_se, self).go()

        ds_name = "%s-%s" % (self.inputs['job_name'], self.inputs['start_time'])
        self.logger.info("Creating dataset %s" % (ds_name,))
        dataset = DataSet(ds_name)
        src_ids = []
        for file in self.inputs['args']:
            self.logger.info("Processing %s" % (file,))
            image = ImageData(FitsFile(file), dataset=dataset)
            self.logger.info("Running source finder")
            sr = image.sextract(det=5, anl=2)
            with closing(database.connection()) as con:
                self.logger.debug("Saving results to database")
                sr.savetoDB(con)
                self.logger.info("Generating source associations")
                database.assocXSrc2XSrc(image.id, con)
                database.assocXSrc2Cat(image.id, con)
                self.logger.info("Querying for region file")
                createRegionByImage(image.id[0], con,
                    os.path.join(
                        os.path.dirname(file),
                        self.inputs['detected_regions']
                    ), logger=self.logger
                )
                with closing(con.cursor()) as cur:
                    self.logger.info("Querying for association list")
                    my_query = associations % (image.id)
                    self.logger.debug(my_query)
                    cur.execute(my_query)
                    with open(
                        os.path.join(
                            os.path.dirname(file),
                            self.inputs['associations']
                        ),
                        'w'
                    ) as output_file:
                        for line in cur.fetchall():
                            output_file.write(str(line) + '\n')
                            src_ids.append(line[0])

        # Diagnostic plot for each extracted source
        self.logger.info("Generating associations plots")
        # Use set to uniqify the list of src_ids
        src_ids = list(set(src_ids))
        with closing(database.connection()) as con:
            for src_id in src_ids:
                self.logger.debug("Generating associations plot for src %d" % src_id)
                plotAssocCloudByXSource(
                    src_id, con, os.path.dirname(self.inputs['args'][0])
                )

            self.outputs['data'] = None
        return 0

if __name__ == '__main__':
    sys.exit(eval(os.path.splitext(os.path.basename(sys.argv[0]))[0])().main())

