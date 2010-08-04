from __future__ import with_statement
from contextlib import closing

import monetdb.sql as db
from monetdb.sql import Error as Error

from lofarpipe.support.lofarrecipe import LOFARrecipe

query = """
SELECT
    t.line
FROM (
    SELECT CAST(
        '#(Name, Type, Ra, Dec, I, Q, U, V, MajorAxis, MinorAxis, Orientation, ReferenceFrequency=\\'60e6\\', SpectralIndexDegree=\\'0\\', SpectralIndex:0=\\'0.0\\', SpectralIndex:1=\\'0.0\\') = format' AS VARCHAR(300)
    ) AS line
    UNION
    SELECT CAST(
        CONCAT(t0.catsrcname, CONCAT(',',
        CONCAT(t0.src_type, CONCAT(',',
        CONCAT(ra2bbshms(t0.ra), CONCAT(',',
        CONCAT(decl2bbsdms(t0.decl), CONCAT(',',
        CONCAT(t0.i, CONCAT(',',
        CONCAT(t0.q, CONCAT(',',
        CONCAT(t0.u, CONCAT(',',
        CONCAT(t0.v, CONCAT(',',
        CONCAT(t0.MajorAxis, CONCAT(',',
        CONCAT(t0.MinorAxis, CONCAT(',',
        CONCAT(t0.Orientation, CONCAT(',',
        CONCAT(t0.ReferenceFrequency, CONCAT(',',
        CONCAT(t0.SpectralIndexDegree, CONCAT(',',
        CONCAT(t0.SpectralIndex_0, CONCAT(',',
        t0.SpectralIndex_1))))))))))))))))))))))))))))
        AS VARCHAR(300)
    ) AS line
    FROM (
        SELECT CAST(
            TRIM(c1.catsrcname) AS VARCHAR(20)
        ) AS catsrcname,
        CASE WHEN c1.pa IS NULL
            THEN CAST('POINT' AS VARCHAR(20))
            ELSE CAST('GAUSSIAN' AS VARCHAR(20))
        END AS src_type,
        CAST(c1.ra AS VARCHAR(20)) AS ra,
        CAST(c1.decl AS VARCHAR(20)) AS decl,
        CAST(c1.i_int_avg AS VARCHAR(20)) AS i,
        CAST(0 AS VARCHAR(20)) AS q,
        CAST(0 AS VARCHAR(20)) AS u,
        CAST(0 AS VARCHAR(20)) AS v,
        CASE WHEN c1.pa IS NULL
            THEN CAST('' AS VARCHAR(20))
            ELSE CASE WHEN c1.major IS NULL
                THEN CAST('' AS VARCHAR(20))
                ELSE CAST(c1.major AS varchar(20))
            END
        END AS MajorAxis,
        CASE WHEN c1.pa IS NULL
            THEN CAST('' AS VARCHAR(20))
            ELSE CASE WHEN c1.minor IS NULL
                THEN CAST('' AS VARCHAR(20))
                ELSE CAST(c1.minor AS varchar(20))
            END
        END AS MinorAxis,
        CASE WHEN c1.pa IS NULL
            THEN CAST('' AS VARCHAR(20))
            ELSE CAST(c1.pa AS varchar(20))
        END AS Orientation,
        CAST(c1.freq_eff AS VARCHAR(20)) AS ReferenceFrequency,
        CASE WHEN si.spindx_degree IS NULL
            THEN CAST('' AS VARCHAR(20))
            ELSE CAST(si.spindx_degree AS VARCHAR(20))
        END AS SpectralIndexDegree,
        CASE WHEN si.spindx_degree IS NULL
            THEN CASE WHEN si.c0 IS NULL
                THEN CAST(0 AS varchar(20))
                ELSE CAST(si.c0 AS varchar(20))
            END
            ELSE CASE WHEN si.c0 IS NULL
                THEN CAST('' AS varchar(20))
                ELSE CAST(si.c0 AS varchar(20))
            END
        END AS SpectralIndex_0,
        CASE WHEN si.c1 IS NULL
            THEN CAST('' AS varchar(20))
            ELSE CAST(si.c1 AS varchar(20))
        END AS SpectralIndex_1
        FROM catalogedsources c1
        LEFT OUTER JOIN spectralindices si ON c1.catsrcid = si.catsrc_id
            WHERE c1.cat_id BETWEEN %s AND %s
            AND c1.ra BETWEEN %s AND %s
            AND c1.decl BETWEEN %s AND %s
            AND c1.i_int_avg > %s
    ) t0
) t
"""

class skymodel(LOFARrecipe):
    """
    Extract basic sky model information from database
    """

    def __init__(self):
        super(simple_se, self).__init__()
        self.optionparser.add_option(
            '--db-host',
            help="Host with MonetDB database instance",
            default="ldb001"
        )
        self.optionparser.add_option(
            '--db-port',
            help="Host with MonetDB database instance",
            default="50000"
        )
        self.optionparser.add_option(
            '--db-dbase',
            help="Database name",
            default="gsm"
        )
        self.optionparser.add_option(
            '--db-user',
            help="Database user",
            default="gsm"
        )
        self.optionparser.add_option(
            '--db-dbase',
            help="Database password",
            default="msss"
        )
        self.optionparser.add_option(
            '--ra-min',
            help="Minimum RA for source selection (degrees)"
        )
        self.optionparser.add_option(
            '--ra-max',
            help="Maximum RA for source selection (degrees)"
        )
        self.optionparser.add_option(
            '--dec-min',
            help="Minimum dec for source selection (degrees)"
        )
        self.optionparser.add_option(
            '--dec-max',
            help="Maximum dec for source selection (degrees)"
        )
        self.optionparser.add_option(
            '--min-flux',
            help="Integrated flus threshold, in Jy, for source selection"
        )
        self.optionparser.add_option(
            '--skymodel-file',
            help="Output file for BBS-format sky model definition"
        )


    def go(self):
        self.logger.info("Building sky model")
        super(simple_se, self).go()

        try:
            with closing db.connect(
                hostname=self.inputs["db_host"],
                port=int(self.inputs["db_port"],
                database=self.inputs["db_dbase"],
                username=self.inputs["db_user"],
                password=self.inputs["db_password"]
            ) as db_connection:
                with closing(db_connection.cursor()) as db_cursor:
                    db_cursor.execute(
                        query % (
                            4, 4, # Only using VLSS for now
                            float(self.inputs('ra_min')),
                            float(self.inputs('ra_max')),
                            float(self.inputs('dec_min')),
                            float(self.inputs('dec_max')),
                            float(self.inputs('min_flux'))
                        )
                    )
                    results = db_cursor.fetchall()

        except db.Error, my_error:
            self.logger.warn("Failed to build sky model: %s " % (my_error))
            return 1

        try:
            with open(self.inputs['skymodel_file'], 'w') as file:
                for line in results:
                    file.write(line)
        except:
            self.logger.warn("Failed to write skymodel file")
            return 1

        return 0

if __name__ == '__main__':
    sys.exit(skymodel.main())

