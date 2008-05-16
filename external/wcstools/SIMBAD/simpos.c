/*** simpos.c - search object by its name from command line arguments
 *** July 31, 2006
 *** By Doug Mink, after IPAC byname.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../libwcs/fitshead.h"
#include "../libwcs/wcs.h"
#include "../libwcs/wcscat.h"

extern int   ned_errno;
static void usage();
const char *RevMsg = "SIMPOS 3.6.5, 31 July 2006, Doug Mink SAO";

main (ac, av)
int  ac;
char *av[];
{
   
    int    st;
    int    no_obj, lobj;
    int    i, j;
    int verbose = 0;
    int printid = 0;
    int printdeg = 0;
    int nid = 0;
    int outsys = WCS_J2000;
    int sysj = WCS_J2000;
    double ra, dec;
    char *str, *objname, *obj1, *posdec, *posra, *newobj;
    char rastr[32], decstr[32];
    char *buff, *idline, *posline, *errline, *id, *errend;
    char url[256];
    int lbuff;

    /* crack arguments */
    for (av++; --ac > 0 && *(str = *av) == '-'; av++) {
        char c;
        while (c = *++str)
        switch (c) {

        case 'b':       /* Print coordinates in B1950 */
            outsys = WCS_B1950;
            break;

        case 'd':       /* Print coordinates in degrees */
            printdeg++;
            break;

        case 'e':       /* Print coordinates in ecliptic coordinates */
            outsys = WCS_ECLIPTIC;
            break;

        case 'g':       /* Print coordinates in galactic coordinates */
            outsys = WCS_GALACTIC;
            break;

        case 'i':       /* Print all IDs found by NED */
            printid++;
            break;

        case 'v':       /* more verbosity, including first ID */
            verbose++;
            break;

        default:
            usage();
            break;
        }
    }

    /* There are ac remaining file names starting at av[0] */
    if (ac == 0)
        usage ();

   while (ac-- > 0) {

	objname = *av++;

	/* Replace underscores and spaces with plusses */
	lobj = strlen (objname);
	newobj = (char *) calloc (lobj+16, 1);
	j = 0;
	for (i = 0; i < lobj; i++) {
	    if (objname[i] == '+') {
		newobj[j++] = '%';
		newobj[j++] = '2';
		newobj[j++] = 'B';
		}
	    else if (objname[i] == ' ' || objname[i] == '_') {
		newobj[j++] = '%';
		newobj[j++] = '2';
		newobj[j++] = '0';
		}
	    else
		newobj[j++] = objname[i];
	    }
	if (verbose)
	    printf ("%s -> ", newobj);

	strcpy (url, "http://vizier.u-strasbg.fr/cgi-bin/nph-sesame?");
	strcat (url, newobj);
	buff = webbuff (url, verbose, &lbuff);

	if (buff == NULL) {
	    if (verbose)
		printf ("no return from SIMBAD\n");
	    else
		fprintf (stderr,"*** No return from SIMBAD for %s\n",objname);
	    continue;
	    }

	/* Read number of objects identified */
	if ((idline = strsrch (buff, "#=")) != NULL) {
	    id = strchr (idline, ':');
	    if (id != NULL)
		nid = atoi (id+2);
	    else
		nid = 0;

	/* Get position */
	    if ((posline = strsrch (buff, "%J ")) != NULL) {
		posra = posline + 3;
		while (*posra == ' ')
		    posra++;
		ra = atof (posra);
		posdec = strchr (posra, ' ');
		while (*posdec == ' ')
		    posdec++;
		posline = strchr (posdec, ' ');
		dec = atof (posdec);
		}
	    else {
		if (verbose)
		    printf ("no position from SIMBAD\n");
		else
		    fprintf (stderr,"*** No SIMBAD position for %s\n",objname);
		continue;
		}
	    }

	else {
	    nid = 0;
	    if ((errline = strsrch (buff, "#!SIMBAD: ")) != NULL) {
		if ((errend = strchr (errline, '\n')) != NULL)
		   *errend = (char) 0;
		fprintf (stderr, "*** %s\n", errline+10);
		}
	    else {
		fprintf (stderr, "*** No SIMBAD position for %s\n", objname);
		}
	    continue;
	    }

	if (nid > 0) {
	    if (verbose) {
		if (nid == 1)
		    fprintf (stdout, "%d object found by SIMBAD: \n", nid);
		else
		    fprintf (stdout, "%d objects found by SIMBAD: \n", nid);
		}
	    if (outsys != WCS_J2000)
		wcscon (sysj, outsys, 0.0, 0.0, &ra, &dec, 0.0);
	    if (outsys == WCS_ECLIPTIC || outsys == WCS_GALACTIC) {
		if (verbose)
		    fprintf (stdout, "l= ");
		fprintf (stdout, "%.6f ", ra);
		if (verbose)
		    fprintf (stdout, "b= ");
		if (dec >= 0.0)
		    fprintf (stdout, "+");
		fprintf (stdout, "%.6f ", dec);
		if (outsys == WCS_GALACTIC)
		    fprintf (stdout, " Galactic\n");
		else if (outsys == WCS_ECLIPTIC)
		    fprintf (stdout, " Ecliptic\n");
		}
	    else {
		if (verbose)
		    fprintf (stdout, "ra= ");
		if (printdeg)
		    fprintf (stdout, "%.6f ", ra);
		else {
		    ra2str (rastr, 31, ra, 3);
		    fprintf (stdout, "%s ", rastr);
		    }
		if (verbose)
		    fprintf (stdout, "dec=");
		if (printdeg)
			fprintf (stdout, "%.6f", dec);
		else {
		    dec2str (decstr, 31, dec, 2);
		    fprintf (stdout, "%s", decstr);
		    }
		if (outsys == WCS_B1950)
		    fprintf (stdout, " B1950\n");
		else
		    fprintf (stdout, " J2000\n");
		}
	    }
	free (buff);
	}
}

static void
usage ()
{
    fprintf (stderr,"%s\n", RevMsg);
    fprintf (stderr,"Return RA and Dec for object name using SIMBAD\n");
    fprintf (stderr,"Usage:  simpos [-id][b|e|g] name1 name2 ...\n");
    fprintf (stderr,"name(n): Objects for which to search (space -> _)\n");
    fprintf (stderr,"-b: Print coordinates in B1950 instead of J2000\n");
    fprintf (stderr,"-d: Print coordinates in degrees instead of sexigesimal\n");
    fprintf (stderr,"-e: Print coordinates in ecliptic instead of J2000\n");
    fprintf (stderr,"-g: Print coordinates in galactic instead of J2000\n");
    fprintf (stderr,"-i: Print ID returned from SIMBAD\n");
    exit (1);
}

/* Oct 25 2002	New program based on nedpos.c
 *
 * Jul 10 2003	Unknown changes
 *
 * Jul 27 2006	Deal with multiple spaces in SIMBAD returns
 * Jul 31 2006	Deal correctly with spaces and + signs in object names
 */
