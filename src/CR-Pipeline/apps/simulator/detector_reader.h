#ifndef DETEADER_HH
#define DETREADER_HH

#include <fstream>
#include <iostream>
#include "Detector.hh"

using namespace std;
class DetectorReaderException {
public:
  
  DetectorReaderException(const char* error):_what(error){}
  
  const char* what() const {
    return _what.c_str();
  }
  
  
private:
  string _what;
  
};

ostream& operator<<(ostream& os, const DetectorReaderException& dre) {
  return (os <<"ERROR:"<<endl<< dre.what()<<endl) ;
}

class DetectReader {
  
public:
  
  DetectReader(const char* file) : _detector(0) { //constructor
    _file.open(file) ;
    cout<<"loading "<<file<<"..."<<endl;
    if(_file.fail()) throw DetectorReaderException("File Error. Check that the file exists and is not corrupted.");
    _file>>word;
    if(word!="BEGIN_DETECTOR") throw DetectorReaderException("Type Error. The file does not start in an appropriate way. Are you sure that you are using a detector file?");
    _file>>word;

    if(word!="SIZE") throw DetectorReaderException("No Size specified. Add a SIZE parameter to your input file.") ;
    else _file>>size ;
    _detector = new Detector(size);
    _file>>word;
    int i=0;

    while((word=="ANT") ||( word=="#") ){

      while(word=="#") {
	_detector->AddFlag();
	_file.ignore(5);
	_file>>readoutID;
	cout<<"Flagging antenna "<<readoutID<<endl;
	_file.ignore(100,'\n');
	_file>>word;
      }

      _file>>readoutID>>ix>>iy;
      (_detector->dipole(i))->setReadoutID(readoutID);
      (_detector->dipole(i))->setCoords(ix, iy) ;
      _file>>word;
      i++ ;
      if(i>size) throw DetectorReaderException ("More dipoles than expected from the size parameter."); 
    }
        
    if(word!="END_DETECTOR") throw DetectorReaderException("Type Error. Detector definition has no endmark or a position is specified without keyword.");
    cout<<"File loaded without errors, returning to main program"<<endl<<endl;
  }
  
  ~DetectReader(){}; //Destructor
  
  Detector* array() {return _detector ;}
  
  
  
private:
  Detector* _detector;
  ifstream _file;
  string word;
  int size ;
  double ix,iy;
  int readoutID;

};


#endif
