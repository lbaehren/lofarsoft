/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

#include <Display/SimplePlot.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  SimplePlot::SimplePlot (){
    init();
  };

  void SimplePlot::init(){
#ifdef HAVE_PGPLOT
     plotter_p = NULL;
#endif
    ppCharacterHeight = 2;
    ppLineWidth = 3;
  };
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  SimplePlot::~SimplePlot ()
  {
    destroy();
  }
  
  void SimplePlot::destroy () {
#ifdef HAVE_PLPLOT
      plend();
#endif
#ifdef HAVE_PGPLOT
     if (plotter_p != NULL) {
      delete plotter_p;
      plotter_p = NULL;
    };
#endif
  };
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void SimplePlot::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ------------------------------------------------------------------- InitPlot
  
  Bool SimplePlot::InitPlot(String file,
			    Double xmin,
			    Double xmax,
			    Double ymin, 
			    Double ymax,
			    Int axis,
			    Int just,
			    Int col,
			    Int cheight, 
			    Int linewidth){
    try {
#ifdef HAVE_PLPLOT
      plend();
      // set device to postscript-color
      plsdev("psc");
      // set output-filename
      plsfnam(file.c_str());   
      // set the background to white, and foreground to black
      plscol0(0, 255, 255, 255);
      plscol0(1, 0, 0, 0);
      //initialize the plot
      plinit();
      plschr(0.,cheight);
      plcol0(col);
      plwid(linewidth);
      plenv(xmin, xmax, ymin, ymax, just, axis);
      
#endif
#ifdef HAVE_PGPLOT
      //initialize a PGPlotter
       if (plotter_p != NULL) {
	delete plotter_p;
	plotter_p = NULL;
      };
      file += "/cps";
      plotter_p = new PGPlotterLocal(file);
      plotter_p->slw(linewidth);
      plotter_p->sch(cheight);
      plotter_p->sci(col); 
      plotter_p->env(xmin, xmax, ymin, ymax, just, axis);
      
#endif
    } catch (AipsError x) {
      cerr << "SimplePlot::InitPlot: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  };
  
  // ------------------------------------------------------------------- PlotLine

  Bool SimplePlot::PlotLine(Vector<Double> xvals,
			    Vector<Double> yvals,
			    Int col,
			    Int style){
    try {
#ifdef HAVE_PLPLOT
      int i,npoints;
      PLFLT *plxval, *plyval;

      if (xvals.nelements() != yvals.nelements()){
	cerr << "SimplePlot::PlotLine: " << "x- and y-vector of different length!" << endl;
	return False;
      };
      npoints = xvals.nelements();
      plxval = new PLFLT[npoints];
      plyval = new PLFLT[npoints];

      for (i=0; i<npoints; i++){
	plxval[i] = xvals(i);
	plyval[i] = yvals(i);
      };

      plcol0(col);
      pllsty(style);
      plline(npoints, plxval, plyval);

      delete [] plxval;
      delete [] plyval;
#endif
#ifdef HAVE_PGPLOT
       if (plotter_p == NULL){
	cerr << "SimplePlot::PlotLine: " << "plotter not initialized!" << endl;
	return False;
      }; 
      if (xvals.nelements() != yvals.nelements()){
	cerr << "SimplePlot::PlotLine: " << "x- and y-vector of different length!" << endl;
	return False;
      }; 
      Vector<Float> xval_(xvals.shape()),yval_(yvals.shape());
      convertArray(xval_,xvals);
      convertArray(yval_,yvals);

      plotter_p->sci(col); 
      plotter_p->sls(style);
      plotter_p->line(xval_, yval_);
      plotter_p->sls(1);

#endif
    } catch (AipsError x) {
      cerr << "SimplePlot::PlotLine: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  };

  // ---------------------------------------------------------------- PlotSymbols

  Bool SimplePlot::PlotSymbols (Vector<Double> xvals,
				Vector<Double> yvals, 
				Vector<Double> yerrs,
				Vector<Double> xerrs, 
				Int col,
				Int symbol,
				Int ticklength){
    try {
#ifdef HAVE_PLPLOT
      int i,npoints;
      PLFLT *plxval, *plyval, *plerrlow, *plerrhigh;

      if (xvals.nelements() != yvals.nelements()){
	cerr << "SimplePlot::PlotLine: " << "x- and y-vector of different length!" << endl;
	return False;
      };
      npoints = xvals.nelements();
      plxval = new PLFLT[npoints];
      plyval = new PLFLT[npoints];
      plerrlow = new PLFLT[npoints];
      plerrhigh = new PLFLT[npoints];

      for (i=0; i<npoints; i++){
	plxval[i] = xvals(i);
	plyval[i] = yvals(i);
      };

      plcol0(col);
      plpoin(npoints, plxval, plyval, symbol);
      if (yerrs.nelements() != 0) {
	if (xvals.nelements() != yerrs.nelements()){
	  cerr << "SimplePlot::PlotSymbols: " << "yerror-vector of different length!" << endl;
	  return False;
	}; 
	for (i=0; i<npoints; i++){
	  plerrlow[i] = yvals(i)-yerrs(i);
	  plerrhigh[i] = yvals(i)+yerrs(i);
	};
	plsmin(0., ticklength);
	plerry(npoints, plxval, plerrlow, plerrhigh);
      };
      if (xerrs.nelements() != 0) {
	if (xvals.nelements() != xerrs.nelements()){
	  cerr << "SimplePlot::PlotSymbols: " << "xerror-vector of different length!" << endl;
	  return False;
	}; 
	for (i=0; i<npoints; i++){
	  plerrlow[i] = xvals(i)-xerrs(i);
	  plerrhigh[i] = xvals(i)+xerrs(i);
	};
	plsmin(0., ticklength);
	plerrx(npoints, plerrlow, plerrhigh, plyval);
      };
      
      delete []plxval;
      delete []plyval;
      delete []plerrlow;
      delete []plerrhigh;
#endif
#ifdef HAVE_PGPLOT
       if (plotter_p == NULL){
	cerr << "SimplePlot::PlotSymbols: plotter not initialized!" << endl;
	return False;
      }; 
      if (xvals.nelements() != yvals.nelements()){
	cerr << "SimplePlot::PlotSymbols: x- and y-vector of different length!" << endl;
	return False;
      }; 
      Vector<Float> xval_(xvals.shape()),yval_(yvals.shape());
      convertArray(xval_,xvals);
      convertArray(yval_,yvals);
      plotter_p->sci(col); 
      plotter_p->pt(xval_, yval_, symbol);
      if (yerrs.nelements() != 0) {
	if (xvals.nelements() != yerrs.nelements()){
	  cerr << "SimplePlot::PlotSymbols: yerror-vector of different length!" << endl;
	  return False;
	}; 
	Vector<Float> err1,err2,yerr_(yerrs.shape());
	convertArray(yerr_,yerrs);
	err1 = yval_-yerr_;
	err2 = yval_+yerr_;
	plotter_p->erry(xval_,err1,err2,ticklength);
      }
      if (xerrs.nelements() != 0) {
	if (xvals.nelements() != xerrs.nelements()){
	  cerr << "SimplePlot::PlotSymbols: xerror-vector of different length!" << endl;
	  return False;
	}; 
	Vector<Float> err1,err2,xerr_(xerrs.shape());
	convertArray(xerr_,xerrs);
	err1 = xval_-xerr_;
	err2 = xval_+xerr_;
	plotter_p->errx(err1,err2,yval_,ticklength);
      }
#endif
    } catch (AipsError x) {
      cerr << "SimplePlot::PlotSymbols: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  };
  
  // ------------------------------------------------------------------ AddLabels

  Bool SimplePlot::AddLabels(String xlabel,
			     String ylabel, 
			     String toplabel,
			     Int col){
    try {
#ifdef HAVE_PLPLOT
      plcol0(col);
      pllab(xlabel.c_str(), ylabel.c_str(), toplabel.c_str());
#endif
#ifdef HAVE_PGPLOT
       if (plotter_p == NULL){
	cerr << "SimplePlot::AddLabels: " << "plotter not initialized!" << endl;
	return False;
      }; 
      plotter_p->sci(col);
      plotter_p->lab(xlabel,ylabel,toplabel);
#endif
    } catch (AipsError x) {
      cerr << "SimplePlot::AddLabels: " << x.getMesg() << endl;
      return False;
    }; 
    return True;    
  };

  // ------------------------------------------------------------------ quickPlot

  Bool SimplePlot::quickPlot(String file,
			     Vector<Double> xvals,
			     Vector<Double> yvals, 
			     Vector<Double> yerrs,
			     Vector<Double> xerrs,
			     String xlabel, 
			     String ylabel,
			     String toplabel,
			     Int linecol,
			     Bool plotASline, 
			     Int style,
			     Bool logx,
			     Bool logy,
			     Bool printingplot){
    Bool status=True;
    try {
      if (logx || logy) {
	cout  << "SimplePlot::quickPlots: " << "Sorry, logarithmic axes are not implemented yet!" << endl;
      }
      Double xmin,xmax,ymin,ymax,tmpval;
      xmin = min(xvals); xmax = max(xvals);
      tmpval = (xmax-xmin)*0.05;
      xmin -= tmpval;
      xmax += tmpval;
      ymin = min(yvals); ymax = max(yvals);
      tmpval = (ymax-ymin)*0.05;
      ymin -= tmpval;
      ymax += tmpval;
      if (printingplot) {
	status = InitPlot(file, xmin, xmax, ymin, ymax, 0, 0, 1, ppCharacterHeight, ppLineWidth);
      } else {
	status = InitPlot(file, xmin, xmax, ymin, ymax);	
      };
      if (plotASline) {
	status = status && PlotLine(xvals, yvals, linecol, style);
      } else {
	status = status && PlotSymbols(xvals, yvals, yerrs, xerrs, linecol, style);
      }
      status = status && AddLabels(xlabel, ylabel, toplabel);
    } catch (AipsError x) {
      cerr << "SimplePlot::quickPlots: " << x.getMesg() << endl;
      return False;
    }; 
    return status;        
  };

  // ------------------------------------------------------------------ quickPlot

  Bool SimplePlot::quick2Dplot(String file, Matrix<Double> zvals, Double xmin, Double xmax, 
			       Double ymin, Double ymax, String xlabel, String ylabel, String toplabel,
			       Bool printingplot, int nLevels, int nClevels, int cCol){
    Bool status=True;
    try {
#ifdef HAVE_PGPLOT
      cerr << "SimplePlot::quick2Dplot: " << "Sorry, 2d-plotting with pgplotter not implemented!" << endl;
      return False;
#endif
#ifdef HAVE_PLPLOT
      // initialize the plot
      if (printingplot) {
	status = InitPlot(file, xmin, xmax, ymin, ymax, 0, 0, 1, ppCharacterHeight, ppLineWidth);
      } else {
	status = InitPlot(file, xmin, xmax, ymin, ymax);	
      };
      int i,j;
      PLINT nx,ny;
      PLFLT **z, zmin, zmax, *level_edges;
      PLFLT *pos,*r,*g,*b;
      PLcGrid  cgrid; //data for the pltr1 function

      //set the color-map
      //currently only default black-white is supported
      pos = new PLFLT[2];
      r = new PLFLT[2];
      g = new PLFLT[2];
      b = new PLFLT[2];
      pos[0] = 0.; pos[1] = 1.;
      r[0] = 0.; r[1] = 1.;
      g[0] = 0.; g[1] = 1.;
      b[0] = 0.; b[1] = 1.;
      plscmap1l( True, 2, pos, r, g, b, NULL);
      delete [] pos;
      delete [] r;
      delete [] g;
      delete [] b;

      nx = zvals.ncolumn();
      ny = zvals.nrow();
      zmin = min(zvals);
      zmax = max(zvals);
      // allocate memory for the data
      plAlloc2dGrid(&z, nx, ny);
      level_edges = new PLFLT[(nLevels+1)];
      cgrid.nx = nx; cgrid.xg = new PLFLT[nx];
      cgrid.ny = ny; cgrid.yg = new PLFLT[ny];

      // fill the coordinate-conversion array
      for (i = 0; i < nx; i++) { cgrid.xg[i] = xmin + i*(xmax-xmin)/(nx-1.); };
      for (i = 0; i < ny; i++) { cgrid.yg[i] = ymin + i*(ymax-ymin)/(ny-1.); };
 
      // copy data from CASA-array to plplot-array
      for (i = 0; i < nx; i++) {
	for (j = 0; j < ny; j++) {
	  z[i][j] = zvals(j,i); //CASA ordering is "FORTRAN-style"
	};
      };
      // fill the level_edges array
      for (i = 0; i <= nLevels; i++) {
	level_edges[i] = zmin + i*(zmax-zmin)/(PLFLT)nLevels;
      }
      
      // do the raster-plot
      plshades(z, nx, ny, NULL, xmin, xmax, ymin, ymax,
      	       level_edges, nLevels+1, 
      	       2, 0, 0, //fill_width, cont_color, cont_width 
      	       plfill, 1, pltr1, &cgrid); // I think the last two parameters aren't used...

      //do the contour-plot
      if (nClevels>0) {
	PLFLT *Clevels;
	Clevels = new PLFLT[nClevels];
	for (i = 0; i < nClevels; i++) {
	  Clevels[i] = zmin + (i+0.5)*(zmax-zmin)/(PLFLT)nClevels;
	}
	plcol0(cCol);
	plcont(z, nx, ny, 1, nx, 1, ny, Clevels, nClevels, pltr1, &cgrid);   
	delete [] Clevels;
      };
      
      // free the allocated memory
      plFree2dGrid(z, nx, ny);
      delete [] level_edges;
      delete [] cgrid.xg;
      delete [] cgrid.yg;

      status = status && AddLabels(xlabel, ylabel, toplabel);
#endif
    } catch (AipsError x) {
      cerr << "SimplePlot::quick2Dplot: " << x.getMesg() << endl;
      return False;
    }; 
    return status;        
  };
  
  
} // Namespace CR -- end
