
#-------------------------------------------------------------------------------
# $Id: plotLocations.g,v 1.1 2006/03/06 15:34:53 horneff Exp $
#-------------------------------------------------------------------------------
#
# Generate a plot similar to the antenna layout plot, but this time for
# geographic positions. This is intended to demonstrate the direction of an 
# identified (RFI) source in the context of the observatory location, e.g. the
# villages surrounding the LOFAR ITS.
#
#-------------------------------------------------------------------------------

pragma include once;

include "pgplotter.g"
include 'pgplotwidget.g'

#-------------------------------------------------------------------------------
#Name: locationsITS
#-------------------------------------------------------------------------------
#Type: Glish function
#Created: 2005/05/11 (Lars B&auml;hren)
#Doc:  Obtain a record with positions of geographic locations near the ITS site.
#Ret:  locations - Record with the locations to be printed on a map of the ITS
#                  site.<br>
#                  <tt>[*4=[label=ITS, pos=[6.86911 52.9088] , symbol=5],
#                       *5=[label=Exloo, pos=[6.8667 52.8833] , symbol=19],
#                       ...]</tt>
#Example: loc := locationsITS();
#-------------------------------------------------------------------------------

locationsITS := function () {

  locations := [=];

  locations[1].label := 'ITS'
  locations[1].pos := [6.869115,52.908815];
  locations[1].symbol := 5;

  locations[2].label := 'Exloo';
  locations[2].pos := [6.8667,52.8833];
  locations[2].symbol := 19;

  locations[3].label := 'Borger';
  locations[3].pos := [6.8000,52.9167];
  locations[3].symbol := 19;

  locations[4].label := 'D27';
  locations[4].pos := [6.798040,52.930198];
  locations[4].symbol := 13;

  locations[5].label := 'D28';
  locations[5].pos := [6.811287,52.925951];
  locations[5].symbol := 13;

  locations[6].label := 'D29';
  locations[6].pos := [6.811575,52.925655];
  locations[6].symbol := 13;

  locations[7].label := 'D30';
  locations[7].pos := [6.845824,52.891582];
  locations[7].symbol := 13;

  locations[8].label := 'Buinen';
  locations[8].pos := [6.8333,52.9333];
  locations[8].symbol := 19;

  locations[9].label := 'Buinerveen';
  locations[9].pos := [6.8833,52.9333];
  locations[9].symbol := 19;

  #locations[].label := '';
  #locations[].pos := [,];
  #locations[].symbol := ;

  return locations;
}

#-------------------------------------------------------------------------------
#Name: plotGeographicLocations
#-------------------------------------------------------------------------------
#Type: Glish function
#Created: 2005/05/11 (Lars B&auml;hren)
#Doc:  Plot the set of provided locations (with custom labels and symbols) and
#      add -- if demanded -- a set of directions.
#Par:  locations     - Record with the list of locations which are about to be
#                      included into the plot. The supported and required 
#                      fields for each location are:
#                      <ul>
#                        <li><i>label</i> (String) - Label to be added next to
#                            the symbol marking the position.
#                        <li><i>pos</i> (Vector&lt;Double&gt;[2]) - The
#                            2-dimensional position of the object/location.
#                        <li><i>symbol</i> (Int) - PGPlot code for the symbol
#                            used to mark the position.
#                      </ul>
#Par:  directions    - If to be used, a record with a list of directions w.r.t.
#                      to a certain position on the map.
#                      <ul>
#                        <li><i>pos</i> (Vector&lt;Double&gt;[2]) - The
#                            2-dimensional position from which the direction
#                            is plotted.
#                        <li><i>angle</i> (Double) - Direction angle.
#                      </ul>
#                      If no directions are added to the plot set directions=F
#Par:  observatory   - Name of the observatory for which this plot is made.
#Example: direction:=[=];
#         direction.pos:=[6.869115,52.908815];
#         direction.angle:=270;
#         plotGeographicLocations (locationsITS(),direction,T);
#-------------------------------------------------------------------------------

plotGeographicLocations := function (ref locations, 
                                     directions=F,
                                     observatory='LOFAR-ITS')
{
  nofLocations := len(locations);

  # --------------------------------------------------------
  # Determine the maximum sidelength of the map

  print 'Extracting bounding box for the plot ...';

  blc := locations[1].pos;
  trc := locations[1].pos;

  for (n in [2:nofLocations]) {
    # track bottom-left-corner
    if (locations[n].pos[1] < blc[1]) {
      blc[1] := locations[n].pos[1];
    }
    if (locations[n].pos[2] < blc[2]) {
      blc[2] := locations[n].pos[2];
    }
    # track top-right-corner
    if (locations[n].pos[1] > trc[1]) {
      trc[1] := locations[n].pos[1];
    }
    if (locations[n].pos[2] > trc[2]) {
      trc[2] := locations[n].pos[2];
    }
  }

  # --------------------------------------------------------
  # Create PGPlotter tool and set the basic display parameters

  print 'Creating PGPlotter tool ...';

  sidelength := 0.1*(trc-blc);

  blc -:= sidelength;
  trc +:= sidelength;

  pg := pgplotter();
  pg.title(spaste('Site map : ',observatory));
#  pg.sch(1.5);
#  pg.slw(2);
  pg.env (blc[1],trc[1],blc[2],trc[2],1,0)
  pg.lab('W->E Longitude','S->N Latitude','Site map')
#  pg.sch(2);

  # --------------------------------------------------------
  # Fill in the locations from the record

  print 'Filling in points ...';

  for (n in seq(1:nofLocations)) {
    pg.setcolor(2);
    pg.plotxy1 (locations[n].pos[1],
                locations[n].pos[2], 
                plotlines=F,
                ptsymbol=locations[n].symbol);
    pg.setcolor(3);
    pg.ptxt(locations[n].pos[1]+0.1*sidelength[1],
            locations[n].pos[2],
            0,
            0,
            locations[n].label);
  }

  # --------------------------------------------------------
  # Requested add direction vectors to the plot

  if (is_boolean(directions) && directions == F) {
    print 'No direction vector(s) added.';
  }
  else if (is_record (directions)) {
    print 'Adding direction vector(s) to the plot ...';
    nofDirections := len(directions);
    for (n in [1:nofDirections]) {
      x := directions[n].pos[1]+2*sidelength[1]*sin(directions[n].angle);
      y := directions[n].pos[2]-2*sidelength[2]*cos(directions[n].angle);
      pg.setcolor(8);
      pg.arro(directions[n].pos[1],directions[n].pos[2],x,y);
    }
  }

  print 'Completed.'

}