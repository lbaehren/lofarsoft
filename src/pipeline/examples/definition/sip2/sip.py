"""
This is a model imaging pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""
from __future__ import with_statement
from contextlib import closing
from itertools import repeat
import sys
import os

from pyrap.quanta import quantity

from lofarpipe.support.control import control
from lofarpipe.support.utilities import log_time
from lofarpipe.support.parset import patched_parset
from lofarpipe.support.lofaringredient import ListField

class sip(control):
    outputs = {
        'images': ListField()
    }

    def pipeline_logic(self):
        from to_process import datafiles # datafiles is a list of MS paths.
        with log_time(self.logger):
            # Build a map of compute node <-> data location on storage nodes.
            storage_mapfile = self.run_task(
                "datamapper", datafiles
            )['mapfile']

            # Produce a GVDS file describing the data on the storage nodes.
            self.run_task('vdsmaker', storage_mapfile)

            # Read metadata (start, end times, pointing direction) from GVDS.
            vdsinfo = self.run_task("vdsreader")

            # NDPPP reads the data from the storage nodes, according to the
            # map. It returns a new map, describing the location of data on
            # the compute nodes.
            ndppp_results = self.run_task(
                "ndppp",
                storage_mapfile,
                parset=os.path.join(
                    self.config.get("layout", "parset_directory"),
                    "ndppp.1.initial.parset"
                ),
                data_start_time=vdsinfo['start_time'],
                data_end_time=vdsinfo['end_time']
            )

            # Remove baselines which have been fully-flagged in any individual
            # subband.
            compute_mapfile = self.run_task(
                "flag_baseline",
                ndppp_results['mapfile'],
                baselines=ndppp_results['fullyflagged']
            )['mapfile']

            # Build a sky model ready for BBS & return the name & flux of the
            # central source.
            ra = quantity(vdsinfo['pointing']['ra']).get_value('deg')
            dec = quantity(vdsinfo['pointing']['dec']).get_value('deg')
            central = self.run_task(
                "skymodel", ra=ra, dec=dec, search_size=2.5
            )

            # Patch the name of the central source into the BBS parset for
            # subtraction.
            with patched_parset(
                self.task_definitions.get("bbs", "parset"),
                {
                    'Step.correct.Model.Sources': "[ \"%s\" ]" % (central["source_name"]),
                    'Step.subtract.Model.Sources': "[ \"%s\" ]" % (central["source_name"])
                }
            ) as bbs_parset:
                # BBS modifies data in place, so the map produced by NDPPP
                # remains valid.
                self.run_task("bbs", compute_mapfile, parset=bbs_parset)

            # Now, run DPPP three times on the output of BBS. We'll run
            # this twice: once on CORRECTED_DATA, and once on
            # SUBTRACTED_DATA. Clip anything at more than 5 times the flux of
            # the central source.
            with patched_parset(
                os.path.join(
                    self.config.get("layout", "parset_directory"),
                    "ndppp.1.postbbs.parset"
                ),
                {
                    "clip1.amplmax": str(5 * central["source_flux"])
                },
                output_dir=self.config.get("layout", "parset_directory")
            ) as corrected_ndppp_parset:
                for i in repeat(None, 3):
                    self.run_task(
                        "ndppp",
                        compute_mapfile,
                        parset=corrected_ndppp_parset,
                        suffix=""
                    )

            with patched_parset(
                os.path.join(
                    self.config.get("layout", "parset_directory"),
                    "ndppp.1.postbbs.parset"
                ),
                {
                    "msin.datacolumn": "SUBTRACTED_DATA",
                    "msout.datacolumn": "SUBTRACTED_DATA",
                    "clip1.amplmax": str(5 * central["source_flux"])
                },
                output_dir=self.config.get("layout", "parset_directory")
            ) as subtracted_ndppp_parset:
                for i in repeat(None, 3):
                    self.run_task(
                        "ndppp",
                        compute_mapfile,
                        parset=subtracted_ndppp_parset,
                        suffix=""
                    )

            # Image CORRECTED_DATA.
            self.logger.info("Imaging CORRECTED_DATA")

            # Patch the pointing direction recorded in the VDS file into
            # the parset for the cimager.
            with patched_parset(
                self.task_definitions.get("cimager", "parset"),
                {
                    'Images.ra': quantity(vdsinfo['pointing']['ra']).formatted("time"),
                    'Images.dec': quantity(vdsinfo['pointing']['dec']).formatted("angle")
                },
                output_dir=self.config.get("layout", "parset_directory")

            ) as imager_parset:
                # And run cimager.
                self.outputs['images'] = self.run_task(
                    "cimager", compute_mapfile,
                    parset=imager_parset,
                    results_dir=os.path.join(
                        self.config.get("layout", "results_directory"),
                        "corrected"
                    )
                )['images']

            # Image SUBTRACTED_DATA.
            self.logger.info("Imaging SUBTRACTED_DATA")

            # Patch the pointing direction recorded in the VDS file into
            # the parset for the cimager, and change the column to be
            # imaged.
            with patched_parset(
                self.task_definitions.get("cimager", "parset"),
                {
                    'Images.ra': quantity(vdsinfo['pointing']['ra']).formatted("time"),
                    'Images.dec': quantity(vdsinfo['pointing']['dec']).formatted("angle"),
                    'datacolumn': "SUBTRACTED_DATA"

                },
                output_dir=self.config.get("layout", "parset_directory")

            ) as subtracted_imager_parset:
                # And run cimager.
                self.outputs['images'] = self.run_task(
                    "cimager", compute_mapfile,
                    parset=subtracted_imager_parset,
                    results_dir=os.path.join(
                        self.config.get("layout", "results_directory"),
                        "subtracted"
                    )
                )['images']

if __name__ == '__main__':
    sys.exit(sip().main())
