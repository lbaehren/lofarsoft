/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                    *
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

#include <Calibration/CalTableInterpolater.h>

namespace CR {  //  Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  template <class T>
  CalTableInterpolater<T>::CalTableInterpolater (){  
    init();
  }
  
  template <class T>
  CalTableInterpolater<T>::CalTableInterpolater(CalTableReader *readerObject){
    init();
    reader_p = readerObject;
  }
  
  template <class T>
  CalTableInterpolater<T>::CalTableInterpolater(CalTableReader *readerObject,
						const String &FieldName, 
						const Vector<String> &AxisNames){
    init();
    reader_p = readerObject;
    FieldName_p = FieldName;
    AxisNames_p = AxisNames;
    NumAxes_p = AxisNames_p.nelements();
  }
  
  template <class T>
  void CalTableInterpolater<T>::init(){
    int i;
    reader_p = NULL;
    for (i=0; i < MAX_NUM_ANTENNAS; i++){
      CachedAntennaIDs_p[i] = 0;
      CachedTime_p[i] = 0;
      CacheInvalid_p[i] = True;
    }
    NumAxes_p = 0;
    AxisNames_p.resize(MAX_NUM_AXIS);
    NextEmptyCache = 0;
    isKeyword = False;
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  template <class T>
  CalTableInterpolater<T>::~CalTableInterpolater ()
  {
    destroy();
  }
  
template <class T>
void CalTableInterpolater<T>::destroy()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================


// ==============================================================================
//
//  Parameters
//
// ==============================================================================

template <class T>  Bool CalTableInterpolater<T>::doCacheInput() {
  if (reader_p == NULL){
    CacheInput_p = True;
    return True;
  } else {
    return False;
  };
}

template <class T>
Bool CalTableInterpolater<T>::AttatchReader(CalTableReader *readerObject){
  
  reader_p = readerObject;
  return True;
}


template <class T>
Bool CalTableInterpolater<T>::SetField(const String &FieldName){
  try {
    if (reader_p->GetFieldType(FieldName) == ""){
      cerr << "CalTableInterpolater::SetField: Field Named: " << FieldName << " not known!" << endl;
      return False;
    } else {
      FieldName_p = FieldName;
    };
  } catch (AipsError x) {
    cerr << "CalTableInterpolater::SetField: " << x.getMesg() << endl;
    return False;
  }; 
 return True;
}

template <class T>
Bool CalTableInterpolater<T>::SetAxis(const String &AxisName, Int AxisNum){
  try {
    if (reader_p->GetFieldType(AxisName) == ""){
      cerr << "CalTableInterpolater::SetAxis: Field Named: " << AxisName << " not known!" << endl;
      return False;
    };
    if (AxisNum == 0) {
      AxisNum = NumAxes_p;
      NumAxes_p++;
    } else if ((AxisNum > (NumAxes_p)) || (AxisNum<0)) {
      cerr << "CalTableInterpolater::SetAxis: Axis index: " << AxisNum << " outside range!" << endl;
      return False;      
    } else {
      AxisNum--;
    };
    if (AxisNum >= MAX_NUM_AXIS) {
      cerr << "CalTableInterpolater::SetAxis: Axis index to high!" << endl;
      if (NumAxes_p > MAX_NUM_AXIS) NumAxes_p--; 
      return False;      
    };
    AxisNames_p(AxisNum) = AxisName;
    // invalidate all cached data
    for (int i=0; i < MAX_NUM_ANTENNAS; i++){
      CacheInvalid_p[i] = True;
    }
  } catch (AipsError x) {
    cerr << "CalTableInterpolater::SetAxis: " << x.getMesg() << endl;
    return False;
  }; 
 return True;
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

template <class T>
Bool CalTableInterpolater<T>::SetAxisValue(Int AxisNum,
					   const Vector<Double> values){
  try {
    if ((AxisNum > NumAxes_p) || ( AxisNum<=0 )) {
      cerr << "CalTableInterpolater::SetAxisValue: Axis index: " << AxisNum << " outside range!" << endl;
      return False;      
    };
    AxisNum--;
    if (!allEQ(AxisValues_p[AxisNum],values)) {
      AxisValues_p[AxisNum] = values;
      // invalidate all cached data
      for (int i=0; i < MAX_NUM_ANTENNAS; i++){
	CacheInvalid_p[i] = True;
      };
    };
  } catch (AipsError x) {
    cerr << "CalTableInterpolater::SetAxisValue: " << x.getMesg() << endl;
    return False;
  }; 
 return True;
}




template <class T>
Bool CalTableInterpolater<T>::GetValues(uInt const date,
					     Int const AntID, 
					     Array<T> *result){
  try {
    int i,cacheIndex;
    for (i=0; i<NumAxes_p; i++){
      if (! (AxisValues_p[i].nelements()>0) ) {
	// This should also mean that AxisNames_p(i) is set
	cerr << "CalTableInterpolater::GetValues: Axis no " << i << " not set!" << endl;
	return False;
      };
    };
    cacheIndex = IndexFromAntID(AntID);
    //    cout << "CalTableInterpolater::GetValues," << FieldName_p <<": AntID:" << AntID <<" cacheIndex:" 
    //	 << cacheIndex << " shape:" << CachedData_p[cacheIndex].shape() << endl;
    if (NumAxes_p == 1) {
      if (CacheInvalid_p[cacheIndex] || (CachedTime_p[cacheIndex] != date && 
			    !reader_p->isIdentical(date,CachedTime_p[cacheIndex],AntID,FieldName_p)) ) {
	// Need to recalculate the interpolated values.
	Vector<Double> x,xtmp;
	Vector<T> y;
	GetInputData(date,AntID,y,x,xtmp,xtmp,xtmp,xtmp,cacheIndex);
	//reader_p->GetData(date,AntID,AxisNames_p(0),&x);
	//reader_p->GetData(date,AntID,FieldName_p,&y);
	InterpolateArray1D<Double, T>::interpolate(CachedData_p[cacheIndex], AxisValues_p[0], x, y, 
						   InterpolateArray1D<Float,Double>::linear);
	CachedTime_p[cacheIndex] = date;
	CacheInvalid_p[cacheIndex] = False;
      };
      result->reference(CachedData_p[cacheIndex]); 
    } else if (NumAxes_p > 1) {
      if (CacheInvalid_p[cacheIndex] || (CachedTime_p[cacheIndex] != date && 
			    !reader_p->isIdentical(date,CachedTime_p[cacheIndex],AntID,FieldName_p)) ) {
	// Need to recalculate the interpolated values.
	if (MAX_NUM_AXIS>5) {
	  cerr << "CalTableInterpolater::GetValues: Compilation error! MAX_NUM_AXIS is defined larger than 5, but this routine only handles 5 dimensions!" << endl;
	}
	Vector<Int> outShape(MAX_NUM_AXIS);
	for (i=0; i<MAX_NUM_AXIS; i++){
	  outShape(i) = AxisValues_p[i].nelements();
	}
	CachedData_p[cacheIndex] = Array<T>(IPosition(NumAxes_p,outShape(0),outShape(1),
						      outShape(2),outShape(3),outShape(4)));
	Int i4,i3,i2,i1;
	i4=i3=i2=i1=0;
	Array<T> y4,y3,y2,y1,y0,tmp3,tmp2,tmp1,tmp0;
	Array<T> refer;
	Vector<Double> x4,x3,x2,x1,x0;
	GetInputData(date,AntID,y4,x0,x1,x2,x3,x4,cacheIndex);
	//reader_p->GetData(date,AntID,FieldName_p,&y4);
	IPosition inShape=y4.shape();
	if (inShape.nelements() != (uInt)NumAxes_p){
	  cerr << "CalTableInterpolater::GetValues: NoDimensions of field don't math NumAxes:"
	       << inShape.nelements() << " != " << NumAxes_p  << endl;
	  return False;
	};
	for (i=0; i< NumAxes_p; i++){
	  if ( !(inShape(i)>1) ) {
	    cerr << "CalTableInterpolater::GetValues: Field has degenerate dimensoins: " 
		 << inShape.asVector() << endl;
	    return False;
	  };
	};	    
	//reader_p->GetData(date,AntID,AxisNames_p(0),&x0);
	//reader_p->GetData(date,AntID,AxisNames_p(1),&x1);
	//if (NumAxes_p>2) {
	//  reader_p->GetData(date,AntID,AxisNames_p(2),&x2);
	//  if (NumAxes_p>3) {
	//    reader_p->GetData(date,AntID,AxisNames_p(3),&x3);
	//    if (NumAxes_p>4) {
	//      reader_p->GetData(date,AntID,AxisNames_p(4),&x4);
	//    };
	//  };
	//};
	
	for (i4=0; (i4<outShape(4)) || (i4==0) ; i4++){
	  if (NumAxes_p>4){
	    InterpolateArray1D<Double, T>::interpolate(tmp3, AxisValues_p[4](IPosition(1,i4),IPosition(1,i4)), 
						       x4, y4, InterpolateArray1D<Float,Double>::linear);
	    y3.reference(tmp3.nonDegenerate(4));
	  } else {
	    if (i4>0) { cerr << "CalTableInterpolater::GetValues: i4>0!!!" << endl; };
	    y3.reference(y4);
	  };
	  for (i3=0; (i3<outShape(3)) || (i3==0) ; i3++){
	    if (NumAxes_p>3){
	      InterpolateArray1D<Double, T>::interpolate(tmp2, AxisValues_p[3](IPosition(1,i3),IPosition(1,i3)), 
							 x3, y3, InterpolateArray1D<Float,Double>::linear);
	      y2.reference(tmp2.nonDegenerate(3));
	    } else {
	      if (i3>0) { cerr << "CalTableInterpolater::GetValues: i3>0!!!" << endl; };
	      y2.reference(y4);
	    };
	    for (i2=0; (i2<outShape(2)) || (i2==0) ; i2++){
	      if (NumAxes_p>2){
		InterpolateArray1D<Double, T>::interpolate(tmp1, AxisValues_p[2](IPosition(1,i2),IPosition(1,i2)), 
							   x2, y2, InterpolateArray1D<Float,Double>::linear);
		y1.reference(tmp1.nonDegenerate(2));
	      } else {
		if (i2>0) { cerr << "CalTableInterpolater::GetValues: i2>0!!!" << endl; };
		y1.reference(y4);
	      };
	      for (i1=0; i1<outShape(1) ; i1++){
		InterpolateArray1D<Double, T>::interpolate(tmp0, AxisValues_p[1](IPosition(1,i1),IPosition(1,i1)), 
							   x1, y1, InterpolateArray1D<Float,Double>::linear);
		y0.reference(tmp0.nonDegenerate(1));
		Slicer slice(IPosition(NumAxes_p,0,i1,i2,i3,i4),
			     IPosition(NumAxes_p, Slicer::MimicSource, 1,1,1,1),
			     IPosition(NumAxes_p,1,1,1,1,1));
		refer.reference(CachedData_p[cacheIndex](slice).nonDegenerate());
		InterpolateArray1D<Double, T>::interpolate(refer, AxisValues_p[0], x0, y0, 
							   InterpolateArray1D<Float,Double>::linear);
	      };
	    };
	  };
	};
	//cout << "CalTableInterpolater::GetValues," << FieldName_p <<": mean:"<<mean(CachedData_p[cacheIndex])<<endl;
	CachedTime_p[cacheIndex] = date;
	CacheInvalid_p[cacheIndex] = False;
      };
      result->reference(CachedData_p[cacheIndex]); 
      //cout << "CalTableInterpolater::GetValues," << FieldName_p <<": mean:"<<mean(CachedData_p[cacheIndex])<<endl;
    } else {
      cerr << "CalTableInterpolater::GetValues: No axis defined! " << endl;
      return False;
    };
  } catch (AipsError x) {
    cerr << "CalTableInterpolater::GetValues: " << x.getMesg() << endl;
    return False;
  }; 
 return True;
}


template <class T>
int CalTableInterpolater<T>::IndexFromAntID(const Int AntID){
  int i;
  for (i =0; i < MAX_NUM_ANTENNAS; i++){ // Exceedingly simple search algorithm...
    if ( CachedAntennaIDs_p[i] == AntID) {
      return i;
    };
  }
  i = NextEmptyCache;
  CachedAntennaIDs_p[i] = AntID;
  CachedTime_p[i] = 0;
  CacheInvalid_p[i] = True;
  NextEmptyCache++;
  if (NextEmptyCache >= MAX_NUM_ANTENNAS){
    cerr << "CalTableInterpolater::IndexFromAntID: NextEmptyCache overflow, reset to zero." << endl;
    NextEmptyCache = 0;
  }
  return i;
}

template <class T>
Bool CalTableInterpolater<T>::GetInputData(const uInt date, const Int AntID, Array<T> &y, 
					   Vector<Double> &x0, Vector<Double> &x1, Vector<Double> &x2,
					   Vector<Double> &x3, Vector<Double> &x4, Int cacheIndex){
  try {
    if (!CacheInput_p) {
      reader_p->GetData(date,AntID,FieldName_p,&y);
      reader_p->GetData(date,AntID,AxisNames_p(0),&x0);
      if (NumAxes_p>1) {
	reader_p->GetData(date,AntID,AxisNames_p(1),&x1);
	if (NumAxes_p>2) {
	  reader_p->GetData(date,AntID,AxisNames_p(2),&x2);
	  if (NumAxes_p>3) {
	    reader_p->GetData(date,AntID,AxisNames_p(3),&x3);
	    if (NumAxes_p>4) {
	      reader_p->GetData(date,AntID,AxisNames_p(4),&x4);
	    };
	  };
	};
      };
    } else {
      if (CachedTime_p[cacheIndex] != date) {
	if (!reader_p->isIdentical(date,CachedTime_p[cacheIndex],AntID,FieldName_p)) {
	  reader_p->GetData(date,AntID,FieldName_p,&CachedInputField_p[cacheIndex]);
	};
	if (!reader_p->isIdentical(date,CachedTime_p[cacheIndex],AntID,AxisNames_p(0))) {
	  reader_p->GetData(date,AntID,AxisNames_p(0),&CachedInputFieldAxis_p[cacheIndex][0]);
	};
	if (NumAxes_p>1) {
	  if (!reader_p->isIdentical(date,CachedTime_p[cacheIndex],AntID,AxisNames_p(1))) {
	    reader_p->GetData(date,AntID,AxisNames_p(1),&CachedInputFieldAxis_p[cacheIndex][1]);
	  };
	  if (NumAxes_p>2) {
	    if (!reader_p->isIdentical(date,CachedTime_p[cacheIndex],AntID,AxisNames_p(2))) {
	      reader_p->GetData(date,AntID,AxisNames_p(2),&CachedInputFieldAxis_p[cacheIndex][2]);
	    };
	    if (NumAxes_p>3) {
	      if (!reader_p->isIdentical(date,CachedTime_p[cacheIndex],AntID,AxisNames_p(3))) {
		reader_p->GetData(date,AntID,AxisNames_p(3),&CachedInputFieldAxis_p[cacheIndex][3]);
	      };
	      if (NumAxes_p>4) {
		if (!reader_p->isIdentical(date,CachedTime_p[cacheIndex],AntID,AxisNames_p(4))) {
		  reader_p->GetData(date,AntID,AxisNames_p(4),&CachedInputFieldAxis_p[cacheIndex][4]);
		};
	      };
	    };
	  };
	};
	//Not all data in the cache belongs to the old "CachedTime" anymore.
	//So unset this. If things go well it is set when the output cache is written.
	CachedTime_p[cacheIndex] = 0;
      };
      y.reference(CachedInputField_p[cacheIndex]);
      x0.reference(CachedInputFieldAxis_p[cacheIndex][0]);
      if (NumAxes_p>1) {
	x1.reference(CachedInputFieldAxis_p[cacheIndex][1]);
	if (NumAxes_p>2) {
	  x2.reference(CachedInputFieldAxis_p[cacheIndex][2]);
 	  if (NumAxes_p>3) {
	    x3.reference(CachedInputFieldAxis_p[cacheIndex][3]);
	    if (NumAxes_p>4) {
	      x4.reference(CachedInputFieldAxis_p[cacheIndex][4]);
	    };
	  };
	};
      };
    };
  } catch (AipsError x) {
    cerr << "CalTableInterpolater::GetInputData: " << x.getMesg() << endl;
    return False;
  }; 
  return True;
}
  
  // ============================================================================
  //
  //  Template instantiations
  //
  // ============================================================================
  
  template class CalTableInterpolater<casa::Double>;
  template class CalTableInterpolater<casa::DComplex>;
  
}  // Namespace CR -- end 
