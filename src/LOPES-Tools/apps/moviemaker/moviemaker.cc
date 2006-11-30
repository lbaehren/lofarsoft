/***************************************************************************
 *   Copyright (C) 2004,2005                                               *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: moviemaker.cc,v 1.1 2005/06/28 15:37:29 bahren Exp $ */

#include <moviemaker.h>

const Double PI = 3.14159265358979323846;

Bool interpolateFrames (GlishSysEvent &event, void *) {

  // --- Preparation: check information received on Glish bus -- //
  
  GlishSysEventSource *glishBus = event.glishSource();
  GlishValue glishVal = event.val();

  /* check that the incoming argument is a glish record; is this is not the 
     case and the glishBus is still pending, then send back a message. */
  if (glishVal.type() != GlishValue::RECORD) {
    cout << "interpolateFrames() : incoming argument is no Glish record." << endl;
    if (glishBus->replyPending())
      glishBus->reply(GlishArray(False));
    return True;
  }

  // OK - its a Glish record so get contents
  GlishRecord glishRec = glishVal;
  GlishArray gtmp;

  // --- Preparation: declaration of the variables --- //
  Int arrayIndex;
  Int frames,flagframes,nf=0,nf0=0,nf1=0,nf2=0;
  Bool addframes_linear,addframes_cosine,skipframe;
  Float stepwidth=0.0,alpha=0.0;
  Vector<Int> shape,shapeOUT,flag,framelist;
  Cube<Float> pixels,pixelsOUT;
  
  // the number of additional frames introduced between to original frames
  if (glishRec.exists("frames")) {
    gtmp = glishRec.get("frames");
    gtmp.get(frames);
  } else {
    cerr << "interpolateFrames() : variable frames missing!" << endl;
    glishBus->reply(GlishArray(False));
  }
  // get the shape of the incoming data cube
  if (glishRec.exists("shape")) {
    gtmp = glishRec.get("shape");
    gtmp.get(shape);
  } else {
    cerr << "interpolateFrames() : variable shape missing!" << endl;
    glishBus->reply(GlishArray(False));
  }
  if (glishRec.exists("pixels")) {
    gtmp = glishRec.get("pixels");
    gtmp.get(pixels);
  } else {
    cerr << "interpolateFrames() : variable pixels missing!" << endl;
    glishBus->reply(GlishArray(False));
  }
  // linear interpolation of frames?
  if (glishRec.exists("linear")) {
    gtmp = glishRec.get("linear");
    gtmp.get(addframes_linear);
  } else {
    cerr << "interpolateFrames() : variable linear missing!" << endl;
    glishBus->reply(GlishArray(False));
  }
  // cosine slope interpolation of frames?
  if (glishRec.exists("cosine")) {
    gtmp = glishRec.get("cosine");
    gtmp.get(addframes_cosine);
  } else {
    cerr << "interpolateFrames() : variable cosine missing!" << endl;
    glishBus->reply(GlishArray(False));
  }
  // get the vector of frames to exclude
  if (glishRec.exists("flag")) {
    gtmp = glishRec.get("flag");
    gtmp.get(flag);
    flag.shape(flagframes);
  } else {
    cerr << "interpolateFrames() : variable flag missing!" << endl;
    glishBus->reply(GlishArray(False));
  }

  // --- set variable values and bring array up to the correct size --- //

  stepwidth = 1/(frames+1.0);
  shapeOUT = shape;
  shapeOUT(2) = (shape(2)-1)*(frames+1)+1;
  pixelsOUT.resize(shapeOUT(0),shapeOUT(1),shapeOUT(2));

  // --- Computation --- //

  /* Make sure that at least one 
     interpolation scheme is active. */

  if (! (addframes_linear && addframes_cosine)) addframes_linear = True;

  //create a list with the flagged frames being excluded  
  framelist.resize(shape(2)-flagframes);

  nf=0;
  for (Int sample=0; sample<shape(2); ++sample) {
    skipframe = False;
    for (Int nflag=0; nflag<flagframes; ++nflag) {
      if ( (flag[nflag]-1) == sample ) skipframe=True ; 
    }
    if (!skipframe) {
      framelist[nf]=sample; 
      ++nf;
    }
  }
   
  for (Int az=0; az<shape(0); ++az) {

    for (Int el=0; el<shape(1); ++el) {
      arrayIndex = 0;
      for (Int sample=1; sample<nf; ++sample) {
        nf1 = framelist[sample-1];
        nf2 = framelist[sample];
        nf0 = nf2-nf1;
	for (Int frame=0; frame<(frames+1)*nf0; ++frame) {
	  stepwidth = 1.0/((frames+1)*nf0);
	  if (addframes_linear) alpha = frame*stepwidth;
	  // (b) transition using a cosine slope
	  if (addframes_cosine) alpha = cos((frame*stepwidth-1)*PI/2);
	  pixelsOUT(az,el,arrayIndex) = (1-alpha)*pixels(az,el,nf1) + alpha*pixels(az,el,nf2);
	  ++arrayIndex;
	}  // end loop : frame
      }  // end loop : sample
      pixelsOUT(az,el,arrayIndex) = pixels(az,el,nf2);
      ++arrayIndex;
    }  // end loop : el

  }  // end loop : az

  cout << "Done." << endl;

  // --- transmit computation result through the Glish bus --- //

  if (glishBus->replyPending()) {
    GlishArray pixelsOUTGlishArray = pixelsOUT;
    glishBus->reply(pixelsOUTGlishArray);
  }

  // ... and if all went well ...
  return True;
  
}

// =============================================================================
//
//  Main routine
//
// =============================================================================

int main (int argc, char *argv[]) {

  GlishSysEventSource glishStream(argc,argv);

  // Definition of the callback event handlers.
  glishStream.addTarget(interpolateFrames,"interpolateFrames");
  
  // Loop and handle incoming events as they received.
  glishStream.loop();

}
