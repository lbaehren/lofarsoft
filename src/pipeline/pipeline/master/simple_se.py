from __future__ import with_statement
from contextlib import closing

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities
import os, os.path, glob, subprocess, sys, numpy, pyfits, shutil, errno, re

# SE tools
from tkp_lib.dataset   import DataSet
from tkp_lib.image     import ImageData
from tkp_lib.accessors import FitsFile
import tkp_lib.database as database

region_local = "SELECT '# Region file format: DS9 version 4.0' UNION SELECT '# Filename: %s' UNION SELECT 'global color=green font=\"helvetica 10 normal\" select=1 highlite=1 edit=1 move=1 delete=1 include=1 fixed=0 source' UNION SELECT 'fk5' UNION SELECT CONCAT('circle(', ra, ',', decl, ',0.025) #color=blue') FROM extractedsources WHERE image_id = %s;"
region_wenss = "SELECT '# Region file format: DS9 version 4.0' UNION SELECT '# Filename: %s' UNION SELECT 'global color=green font=\"helvetica 10 normal\" select=1 highlite=1 edit=1 move=1 delete=1 include=1 fixed=0 source' UNION SELECT 'fk5' UNION SELECT CONCAT('circle(', ra, ',', decl, ',0.025) #color=red') FROM catalogedsources,catalogs WHERE catname = 'WENSS' AND catid = cat_id AND ra BETWEEN %f AND %f AND decl BETWEEN %f AND %f;" 
associations = """
SELECT
    x1.ra, x1.decl, x1.I_Peak, catalogs.catname, cs.ra, cs.decl, a.assoc_weight, a.assoc_distance_arcsec
    FROM extractedsources x1
        LEFT OUTER JOIN
            assoccatsources a ON xtrsrc_id = xtrsrcid
        LEFT OUTER JOIN
            catalogedsources cs ON assoc_catsrc_id = catsrcid
        LEFT OUTER JOIN
            catalogs ON cat_id = catid
    WHERE
        x1.image_id = %d
    ORDER BY
        I_Peak;
"""

def image_region(image):
    """
    Find max and min RA & dec in an image by checking the corner positions.
    """
    ra_max, dec_max = image.pix_to_position([0,0])
    ra_min, dec_min = ra_max, dec_max

    corners = (
        (0, image.ydim-1),
        (image.xdim-1, 0),
        (image.xdim-1, image.ydim-1),
    )

    for corner in corners:
        ra, dec = image.pix_to_position(corner)
        ra_max  = max(ra_max, ra)
        ra_min  = min(ra_min, ra)
        dec_max = max(dec_max, dec)
        dec_min = min(dec_min, dec)

    if (ra_max - ra_min) > 180:
        ra_max, ra_min = ra_min, ra_max

    return ra_min, ra_max, dec_min, dec_max
    
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
            default="assocation.list"
        )

    def go(self):
        self.logger.info("Starting source identification")
        super(simple_se, self).go()

        ds_name = "%s-%s" % (self.inputs['job_name'], self.inputs['start_time'])
        self.logger.info("Creating dataset %s" % (ds_name,))
        dataset = DataSet(ds_name)
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
                with closing(con.cursor()) as cur:
                    self.logger.info("Querying for local region file")
                    my_query = region_local % (file, int(image.id[0]))
                    self.logger.debug(my_query)
                    cur.execute(my_query)
                    with open(
                        os.path.join(
                            os.path.dirname(file),
                            self.inputs['detected_regions']
                        ),
                        'w'
                    ) as output_file:
                        for line in cur.fetchall():
                            output_file.write(line[0] + '\n')
                    self.logger.info("Querying for WENSS region file")
                    params = [file,] + list(image_region(image))
                    my_query = region_wenss % tuple(params)
                    self.logger.debug(my_query)
                    cur.execute(my_query)
                    with open(
                        os.path.join(
                            os.path.dirname(file),
                            self.inputs['wenss_regions']
                        ),
                        'w'
                    ) as output_file:
                        for line in cur.fetchall():
                            output_file.write(line[0] + '\n')
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

        self.outputs['data'] = None
        return 0

if __name__ == '__main__':
    sys.exit(eval(os.path.splitext(os.path.basename(sys.argv[0]))[0])().main())

