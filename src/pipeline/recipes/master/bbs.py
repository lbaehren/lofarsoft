from __future__ import with_statement
from contextlib import closing
import psycopg2


# Unused?
import sys, os, logging, tempfile, glob, shutil, numpy
from subprocess import check_call, CalledProcessError
from collections import defaultdict

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.lofaringredient import LOFARinput, LOFARoutput
from lofarpipe.support.group_data import group_files
import lofarpipe.support.utilities as utilities
from lofarpipe.support.clusterdesc import ClusterDesc, get_compute_nodes

class bbs(LOFARrecipe):
    def __init__(self):
        super(bbs, self).__init__()
        self.optionparser.add_option(
            '--executable',
            dest="executable",
            help="Executable to be run (ie, calibrate script)"
        )
        self.optionparser.add_option(
            '--initscript',
            dest="initscript",
            help="Initscript to source (ie, lofarinit.sh)"
        )
        self.optionparser.add_option(
            '-p', '--parset',
            dest="parset",
            help="BBS configuration parset"
        )
        self.optionparser.add_option(
            '-s', '--skymodel',
            dest="skymodel",
            help="initial sky model (in makesourcedb format)"
        )
        self.optionparser.add_option(
            '-w', '--working-directory',
            dest="working_directory",
            help="Working directory used on compute nodes"
        )
        self.optionparser.add_option(
            '-f', '--force',
            dest="force",
            help="Automatically clean control database, sky model parmdb, and instrument model parmdb",
            action="store_true"
        )
        self.optionparser.add_option(
            '--key',
            dest="key",
            help="Key to identify BBS session"
        )
        self.optionparser.add_option(
            '--db-host',
            dest="db_host",
            help="Database host with optional port"
        )
        self.optionparser.add_option(
            '--db-user',
            dest="db_user",
            help="Database user"
        )
        self.optionparser.add_option(
            '--db-name',
            dest="db_name",
            help="Database name"
        )
        self.optionparser.add_option(
            '--log',
            dest="log",
            help="Log file"
        )
        self.optionparser.add_option(
            '--makevds-exec',
            dest="makevds_exec",
            help="makevds executable"
        )
        self.optionparser.add_option(
            '--combinevds-exec',
            dest="combinevds_exec",
            help="combinevds executable"
        )
        self.optionparser.add_option(
            '--max-bands-per-node',
            dest="max_bands_per_node",
            help="Maximum number of subbands to farm out to a given cluster node",
            default="8"
        )

    def go(self):
        self.logger.info("Starting BBS run")
        super(bbs, self).go()

        # First: clean the database
        self.logger.debug("Cleaning BBS database for key %s" % (self.inputs["key"]))
        with closing(
            psycopg2.connect(
                host=self.inputs["db_host"],
                user=self.inputs["db_user"],
                database=self.inputs["db_name"]
            )
        ) as db_connection:
            db_connection.set_isolation_level(
                psycopg2.extensions.ISOLATION_LEVEL_AUTOCOMMIT
            )
            with closing(db_connection.cursor()) as db_cursor:
                db_cursor.execute(
                    "DELETE FROM blackboard.session WHERE key=%s",
                    (self.inputs["key"],)
                )


        # Unfinished.

        return 0

if __name__ == '__main__':
    sys.exit(bbs().main())
