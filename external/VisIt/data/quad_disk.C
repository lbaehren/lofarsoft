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

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <silo.h>

//
// Purpose: Build a simple, 2D, quadmesh with coordinate field chosen such
// that the mesh takes the shape of a circle. Also, add a node-centered
// elevation variable for a hemi-sphere above the disk.
//

void
build_mesh(int N, float **xCoords, float **yCoords, float **sphElev)
{
    int iMax = (N - 1) / 2;
    int jMax = (N - 1) / 2;

    float *xvals = new float[N * N];
    float *yvals = new float[N * N];
    float *evals = new float[N * N];

    for (int i = -iMax; i <= iMax; i++)
    {
        for (int j = jMax; j >= -jMax; j--)
        {
            int absi = i < 0 ? -i : i;
            int absj = j < 0 ? -j : j;
            int sq = absi < absj ? absj : absi;

            float radius = 1.4142135623731 * sq;
            float x, y, angle;
            if (absi != 0)
            {
                angle = atan((double) absj / (double) absi);
                x = radius * cos(angle);
                y = radius * sin(angle);
            }
            else
            {
                x = 0.0;
                y = radius;
            }

            if (absi != i)
                x = -x;
            if (absj == j)
                y = -y;

            int ii = i + iMax;
            int jj = j + jMax;
            int kk = jj * N + ii;
            xvals[kk] = x;
            yvals[kk] = y;
            evals[kk] = sqrt(2.0 * iMax * iMax - radius * radius);
        }
    }

    *xCoords = xvals;
    *yCoords = yvals;
    *sphElev = evals;
}

int
main(int argc, char **argv)
{
    DBfile *dbfile;
    int N = 21;
    int driver = DB_PDB;

    int i = 1;
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
        else if (strcmp(argv[i], "-n") == 0)
        {
            i++;
            N = atoi(argv[i]);
            if (N < 0 || N > 10000)
            {
               fprintf(stderr,"size, %d, too large\n", N);
               exit(-1);
            }
            if (N % 2 != 1)
            {
               fprintf(stderr,"size, %d, should be an odd number\n", N);
               exit(-1);
            }
        }

        i++;
    }


    float *coords[2] = {0, 0};
    float *sphElev = 0;
    build_mesh(N, &coords[0], &coords[1], &sphElev);

    dbfile = DBCreate("quad_disk.silo", DB_CLOBBER, DB_LOCAL,
                      "2D logical grid deformed into a disk", driver);

    char *coordnames[2];
    coordnames[0] = "xcoords";
    coordnames[1] = "ycoords";
    int ndims = 2;
    int dims[2];
    dims[0] = N; 
    dims[1] = N;

    DBPutQuadmesh(dbfile, "mesh", coordnames, coords, dims, ndims,
        DB_FLOAT, DB_NONCOLLINEAR, NULL);

    DBPutQuadvar1(dbfile, "sphElev", "mesh", sphElev, dims, ndims,
                             NULL, 0, DB_FLOAT, DB_NODECENT, NULL);

    DBClose(dbfile);
}
