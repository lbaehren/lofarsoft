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
#ifdef HAVE_PGPLOT
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
#ifdef HAVE_PGPLOT
       if (plotter_p == NULL){
	cerr << "SimplePlot::PlotSymbols: " << "plotter not initialized!" << endl;
	return False;
      }; 
      if (xvals.nelements() != yvals.nelements()){
	cerr << "SimplePlot::PlotSymbols: " << "x- and y-vector of different length!" << endl;
	return False;
      }; 
      Vector<Float> xval_(xvals.shape()),yval_(yvals.shape());
      convertArray(xval_,xvals);
      convertArray(yval_,yvals);
      plotter_p->sci(col); 
      plotter_p->pt(xval_, yval_, symbol);
      if (yerrs.nelements() != 0) {
	if (xvals.nelements() != yerrs.nelements()){
	  cerr << "SimplePlot::PlotSymbols: " << "yerror-vector of different length!" << endl;
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
	  cerr << "SimplePlot::PlotSymbols: " << "xerror-vector of different length!" << endl;
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
#ifdef HAVE_PGPLOT
       if (logx || logy) {
	cout  << "SimplePlot::quickPlots: " << "@orry, logarithmic axes are not implemented yet!" << endl;
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
	status = InitPlot(file,
			  xmin,
			  xmax,
			  ymin,
			  ymax,
			  0,
			  0,
			  1,
			  ppCharacterHeight,
			  ppLineWidth);
      } else {
	status = InitPlot(file, xmin, xmax, ymin, ymax);	
      };
      if (plotASline) {
	status = status &&  PlotLine(xvals, yvals, linecol, style);
      } else {
	status = status && PlotSymbols(xvals, yvals, yerrs, xerrs, linecol, style);
      }
      status = status && AddLabels(xlabel, ylabel, toplabel);
#endif
    } catch (AipsError x) {
      cerr << "SimplePlot::quickPlots: " << x.getMesg() << endl;
      return False;
    }; 
    return status;        
  };


} // Namespace CR -- end
