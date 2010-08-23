"""
This is a model imaging pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""
from __future__ import with_statement
import sys, os
from pyrap.quanta import quantity
from lofarpipe.support.control import control
from lofarpipe.support.utilities import log_time, patch_parset
from lofarpipe.support.clusterhandler import ipython_cluster
from lofarpipe.support.lofaringredient import LOFARinput, LOFARoutput

class sip(control):
    def pipeline_logic(self):
        from to_process import datafiles # to_process is a list of MS paths.
        with log_time(self.logger):
            # Build a map of compute node <-> data location on storage nodes.
            storage_mapfile = self.run_task(
                "datamapper", datafiles
            )['mapfile']

            # Produce a GVDS file describing the data on the storage nodes.
            self.run_task('vdsmaker', storage_mapfile)
            self._save_state()

            # Read metadata (start, end times, pointing direction) from GVDS.
            vdsinfo = self.run_task("vdsreader")
            self._save_state()

            # NDPPP reads the data from the storage nodes, according to the
            # map. It returns a new map, describing the location of data on
            # the compute nodes.
            compute_mapfile = self.run_task(
                "ndppp",
                storage_mapfile,
                data_start_time=vdsinfo['start_time'],
                data_end_time=vdsinfo['end_time']
            )['mapfile']
            if os.path.exists(storage_mapfile):
                os.unlink(storage_mapfile)
            self._save_state()

            # Now build a sky model ready for BBS
            ra = quantity(vdsinfo['pointing']['ra']).get_value('deg')
            dec = quantity(vdsinfo['pointing']['dec']).get_value('deg')
            ra_min, ra_max = ra-2.5, ra+2.5
            dec_min, dec_max = dec-2.5, dec+2.5
            self.run_task("skymodel", ra_min=ra_min, ra_max=ra_max, dec_min=dec_min, dec_max=dec_max)
            self._save_state()

            # BBS modifies data in place, so the map produced by NDPPP remains
            # valid.
            self.run_task("bbs", compute_mapfile)
            self._save_state()

            # Patch the pointing direction recorded in the VDS file into
            # the parset for the cimager.
            imager_parset = patch_parset(
                self.task_definitions.get("cimager", "parset"),
                {
                    'Images.ra': quantity(vdsinfo['pointing']['ra']).formatted("time"),
                    'Images.dec': quantity(vdsinfo['pointing']['dec']).formatted("angle")
                },
                output_dir=self.config.get("layout", "parset_directory")

            )
            # And run cimager.
            try:
                self.outputs['images'] = self.run_task(
                    "cimager", compute_mapfile, parset=imager_parset
                )['images']
            finally:
                os.unlink(imager_parset)

            if os.path.exists(compute_mapfile):
                os.unlink(compute_mapfile)

if __name__ == '__main__':
    sys.exit(sip().main())
