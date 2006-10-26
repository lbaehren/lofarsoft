/*****************************************************************************
*
* Copyright (c) 2000 - 2006, The Regents of the University of California
* Produced at the Lawrence Livermore National Laboratory
* All rights reserved.
*
* This file is part of VisIt. For details, see http://www.llnl.gov/visit/. The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or materials provided with the distribution.
*  - Neither the name of the UC/LLNL nor  the names of its contributors may be
*    used to  endorse or  promote products derived from  this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED.  IN  NO  EVENT  SHALL  THE  REGENTS  OF  THE  UNIVERSITY OF
* CALIFORNIA, THE U.S.  DEPARTMENT  OF  ENERGY OR CONTRIBUTORS BE  LIABLE  FOR
* ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <silo.h>

static void build_csg(DBfile *dbfile, char *name);
static void build_greenman_csg(DBfile *dbfile, char *name);
static void build_primitives_csg(DBfile *dbfile);

int
main(int argc, char *argv[])
{
    DBfile        *dbfile;
    int            i=1, driver = DB_PDB;

    DBShowErrors(DB_ALL, NULL);

    while (i < argc)
    {
        if (strcmp(argv[i], "-driver") == 0)
        {
            i++;

            if (strcmp(argv[i], "DB_HDF5") == 0)
            {
                driver = DB_HDF5;
            }
            else if (strcmp(argv[i], "DB_PDB") == 0)
            {
                driver = DB_PDB;
            }
            else
            {
               fprintf(stderr,"Uncrecognized driver name \"%s\"\n",
                   argv[i]);
               exit(-1);
            }
        }
        i++;
    }

    dbfile = DBCreate("csg.silo", 0, DB_LOCAL, "csg test file", driver);
    build_csg(dbfile, "csgmesh");
    build_greenman_csg(dbfile, "greenman_mesh");
    build_primitives_csg(dbfile);
    DBClose(dbfile);

    return 0;
}

static void
build_csg(DBfile *dbfile, char *name)
{
    // build and output the csg mesh (boundaries)
    {
        int typeflags[] =
        {
            DBCSG_SPHERE_PR,
            DBCSG_PLANE_X,
            DBCSG_PLANE_X,
            DBCSG_CYLINDER_PNLR,
            DBCSG_SPHERE_PR,
            DBCSG_SPHERE_PR
        };

        float coeffs[] =
        {
            0.0, 0.0, 0.0, 5.0,                // point-radius form of sphere
           -2.0,                               // x-intercept form of plane
            2.0,                               // x-intercept form of plane
          -10.0, 0.0, 0.0, 1.0, 0.0, 0.0, 20.0, 4.5, /* point-normal-length-radius form of cylinder */
            0.0, 0.0, 49.5, 50.0,              // point-radius form of sphere
            0.0, 0.0, -49.5, 50.0              // point radius form of sphere
        };

        int nbounds = sizeof(typeflags) / sizeof(typeflags[0]);
        int lcoeffs = sizeof(coeffs) / sizeof(coeffs[0]);

        double extents[] = {-5.0, -5.0, -5.0, 5.0, 5.0, 5.0};

        DBPutCsgmesh(dbfile, name, 3, nbounds, typeflags,
            NULL, coeffs, lcoeffs, DB_FLOAT, extents, "csgzl", NULL);
    }

    // build and output the csg zonelist
    {
        int typeflags[] =
        {
            DBCSG_INNER,
            DBCSG_OUTER,
            DBCSG_INNER,
            DBCSG_INNER,
            DBCSG_INTERSECT,
            DBCSG_INTERSECT,
            DBCSG_DIFF,
            DBCSG_INNER,
            DBCSG_INNER,
            DBCSG_INTERSECT,
            DBCSG_INTERSECT
        };
        int leftids[] =  { 0,  1,  2,  3,  0,  4,  5,  4,  5,  7,  9};
        int rightids[] = {-1, -1, -1, -1,  1,  2,  3, -1, -1,  8,  0}; 
        int zonelist[] = {6, 10};

        int nregs = sizeof(typeflags) / sizeof(typeflags[0]);
        int nzones = sizeof(zonelist) / sizeof(zonelist[0]);

        char *zonenames[] = {"ring housing", "lens-shaped fin"};

        DBoptlist *optlist = DBMakeOptlist(1);
        DBAddOption(optlist, DBOPT_ZONENAMES, zonenames);

        DBPutCSGZonelist(dbfile, "csgzl", nregs, typeflags, leftids, rightids,
                         NULL, 0, DB_INT, nzones, zonelist, optlist);

        DBFreeOptlist(optlist);
    }

    /* output a csg variable */
    {
        const void *pv[1];
        double var1_data[] = {3.14159267, 2.0};
        const char *pname[1];
        char name1[] = "var1";

        pv[0] = var1_data;
        pname[0] = name1;

        DBPutCsgvar(dbfile, "var1", "csgmesh", 1, pname, pv, 2, DB_DOUBLE,
            DB_ZONECENT, 0);
    }
}

// This mesh is based on some test data from Greg Greenman
static void
build_greenman_csg(DBfile *dbfile, char *name)
{
    // build and output the csg mesh (boundaries)
    {
        int typeflags[] =
        {
            DBCSG_QUADRIC_G,
            DBCSG_QUADRIC_G,
            DBCSG_QUADRIC_G,
            DBCSG_QUADRIC_G,
            DBCSG_QUADRIC_G,
            DBCSG_QUADRIC_G
        };

        const float c0 = .464102;
        const float c1 = .707107;
        const float c2 = -1.0718;
        float coeffs[] =
        {
         // x^2  y^2  z^2  xy   yz   xz    x    y    z    c
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,    // "bottom" 
            1.0,  c0,  c0, 0.0,  c2, 0.0, 0.0, 0.0, 0.0, 0.0,    // "cone"
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,  c1,  c1, 0.0,    // "cone_apex"
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,  c1,  c1,-4.0,    // "cone_end"
            1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -100.0, // "cylinder"
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, -10.0   // "top" 
        };

        int nbounds = sizeof(typeflags) / sizeof(typeflags[0]);
        int lcoeffs = sizeof(coeffs) / sizeof(coeffs[0]);

        double extents[] = {0.0, 0.0, 0.0, 10.0, 10.0, 10.0};

        char *bndnames[] = {"bottom", "cone", "cone_apex", "cone_end",
                            "cylinder", "top"};

        DBoptlist *optlist = DBMakeOptlist(2);
        DBAddOption(optlist, DBOPT_EXTENTS, extents);
        DBAddOption(optlist, DBOPT_BNDNAMES, bndnames);

        DBPutCsgmesh(dbfile, name, 3, nbounds, typeflags,
            NULL, coeffs, lcoeffs, DB_FLOAT, extents, "greenman_csgzl", optlist);

        DBFreeOptlist(optlist);
    }

    // build and output the csg zonelist
    {
        int typeflags[] =
        {
            DBCSG_OUTER,          // 0: above bottom plane
            DBCSG_INNER,          // 1: below top plane
            DBCSG_INNER,          // 2: inside of cylinder
            DBCSG_INTERSECT,      // 3: between top/bottom
            DBCSG_INTERSECT,      // 4: cylinder clipped to between top/bottom
            DBCSG_OUTER,          // 5: outside of cylinder
            DBCSG_INTERSECT,      // 6: oustide of cylinder cliped to between top/bottom
            DBCSG_INNER,          // 7: inside of cone
            DBCSG_INNER,          // 8: below cone_end plane
            DBCSG_OUTER,          // 9: above cone_apex plane
            DBCSG_INTERSECT,      //10: between cone planes 
            DBCSG_INTERSECT,      //11: inside of cone between cone planes
            DBCSG_DIFF            //12: cylinder minus cone
        };
        //                 0   1   2   3   4   5   6   7   8   9  10  11  12
        int leftids[] =  { 0,  5,  4,  0,  3,  4,  5,  1,  3,  2,  8, 10,  4};
        int rightids[] = {-1, -1, -1,  1,  2, -1,  3, -1, -1, -1,  9,  7, 11};
        int zonelist[] = {6, 11, 12};

        int nregs = sizeof(typeflags) / sizeof(typeflags[0]);
        int nzones = sizeof(zonelist) / sizeof(zonelist[0]);

        char *zonenames[] = {"void", "uranium", "air"};

        DBoptlist *optlist = DBMakeOptlist(1);
        DBAddOption(optlist, DBOPT_ZONENAMES, zonenames);

        DBPutCSGZonelist(dbfile, "greenman_csgzl", nregs, typeflags, leftids, rightids,
                         NULL, 0, DB_INT, nzones, zonelist, optlist);
    }
}

static void
build_primitives_csg(DBfile *dbfile)
{

    // build and output the csg mesh (boundaries)
    {
        int typeflags[] =
        {
            DBCSG_SPHERE_PR
        };

        float coeffs[] =
        {
            0.0, 0.0, 0.0, 1.0                // point-radius form of sphere
        };

        int nbounds = sizeof(typeflags) / sizeof(typeflags[0]);
        int lcoeffs = sizeof(coeffs) / sizeof(coeffs[0]);

        double extents[] = {-1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

        char *bndnames[] = {"sphere"};

        DBoptlist *optlist = DBMakeOptlist(2);
        DBAddOption(optlist, DBOPT_EXTENTS, extents);
        DBAddOption(optlist, DBOPT_BNDNAMES, bndnames);

        DBPutCsgmesh(dbfile, "sphere", 3, nbounds, typeflags,
            NULL, coeffs, lcoeffs, DB_FLOAT, extents, "sphere_csgzl", optlist);

        DBFreeOptlist(optlist);
    }

    // build and output the csg zonelist
    {
        int typeflags[] =
        {
            DBCSG_INNER           // 0: inside of sphere 
        };
        //                 0
        int leftids[] =  { 0};
        int rightids[] = {-1};
        int zonelist[] = {0};

        int nregs = sizeof(typeflags) / sizeof(typeflags[0]);
        int nzones = sizeof(zonelist) / sizeof(zonelist[0]);

        char *zonenames[] = {"sphere"};

        DBoptlist *optlist = DBMakeOptlist(1);
        DBAddOption(optlist, DBOPT_ZONENAMES, zonenames);

        DBPutCSGZonelist(dbfile, "sphere_csgzl", nregs, typeflags, leftids, rightids,
                         NULL, 0, DB_INT, nzones, zonelist, optlist);
    }

    // build and output the csg mesh (boundaries)
    {
        int typeflags[] =
        {
            DBCSG_CYLINDER_PNLR
        };

        float coeffs[] =
        {
           0.0, -10.0, 0.0, 0.0, 1.0, 0.0, 20.0, 4.5 /* point-normal-length-radius form of cylinder */
        };

        int nbounds = sizeof(typeflags) / sizeof(typeflags[0]);
        int lcoeffs = sizeof(coeffs) / sizeof(coeffs[0]);

        double extents[] = {-5.0, -10.0, -5.0, 5.0, 10.0, 5.0};

        char *bndnames[] = {"cylinder"};

        DBoptlist *optlist = DBMakeOptlist(2);
        DBAddOption(optlist, DBOPT_EXTENTS, extents);
        DBAddOption(optlist, DBOPT_BNDNAMES, bndnames);

        DBPutCsgmesh(dbfile, "cylinder", 3, nbounds, typeflags,
            NULL, coeffs, lcoeffs, DB_FLOAT, extents, "cylinder_csgzl", optlist);

        DBFreeOptlist(optlist);
    }

    // build and output the csg zonelist
    {
        int typeflags[] =
        {
            DBCSG_INNER           // 0: inside of cylinder 
        };
        //                 0
        int leftids[] =  { 0};
        int rightids[] = {-1};
        int zonelist[] = {0};

        int nregs = sizeof(typeflags) / sizeof(typeflags[0]);
        int nzones = sizeof(zonelist) / sizeof(zonelist[0]);

        char *zonenames[] = {"cylinder"};

        DBoptlist *optlist = DBMakeOptlist(1);
        DBAddOption(optlist, DBOPT_ZONENAMES, zonenames);

        DBPutCSGZonelist(dbfile, "cylinder_csgzl", nregs, typeflags, leftids, rightids,
                         NULL, 0, DB_INT, nzones, zonelist, optlist);
    }
}
