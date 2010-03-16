////////////////////////////////////////////////////////////////////////
//Definition of Python bindings for hftools ...
////////////////////////////////////////////////////////////////////////



// Tell the preprocessor (for generating wrappers) that this is a c++
// header file for Python exposure
#undef HFPP_FILETYPE
#define HFPP_FILETYPE hPYTHON

//BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(hArray_setDimensions_overloads,setDimensions<HNumber>,1,2)

/*
std::hArray<HNumber>& (*fptr_hArray_setDimensions_overloads_1)(HInteger dim0) = &std::hArray<HNumber>::setDimensions1;
std::hArray<HNumber>& (*fptr_hArray_setDimensions_overloads_2)(HInteger dim0, HInteger dim1)= &std::hArray<HNumber>::setDimensions;
*/
//std::hArray<HNumber>& (*fptr_hArray_setDimensions_overloads_1)(HInteger dim0) = &std::hArray::setDimensions;
//std::hArray<HNumber>& (*fptr_hArray_setDimensions_overloads_2)(HInteger dim0, HInteger dim1) = &std::hArray::setDimensions;

BOOST_PYTHON_MODULE(hftools)
{
    using namespace boost::python;

    class_<CR::DataReader>("DataReader")
      //      .def("read",&hFileRead) -> This is defined in pycrtools.py, since it is templated
      .def("get",&hFileGetParameter)
      .def("set",&hFileSetParameter,return_internal_reference<>())
      .def("summary",&hFileSummary)
      ;    

    /*
  class_<casa::Vector<HInteger> >("CasaIntVec")
    .def(vector_indexing_suite<casa::Vector<HInteger> >())
    ;
    */

  class_<std::vector<HInteger> >("IntVec")
    .def(vector_indexing_suite<std::vector<HInteger> >())

      /*.def("thiAdd",fptr_hiAdd_HComplexHComplex12)
	.def("thiAdd",fptr_hiAdd_HComplexHNumber12)
	.def("thiAdd",fptr_hiAdd_HComplexHInteger12)
	.def("thiAdd",fptr_hiAdd_HNumberHComplex12)
	.def("thiAdd",fptr_hiAdd_HNumberHNumber12)
	.def("thiAdd",fptr_hiAdd_HNumberHInteger12)
	.def("thiAdd",fptr_hiAdd_HIntegerHComplex12)
	.def("thiAdd",fptr_hiAdd_HIntegerHNumber12)
	.def("thiAdd",fptr_hiAdd_HIntegerHInteger12)
      */
      ;

    class_<std::vector<HNumber> >("FloatVec")
        .def(vector_indexing_suite<std::vector<HNumber> >())
      ;

    class_<std::vector<bool> >("BoolVec")
        .def(vector_indexing_suite<std::vector<bool> >())
      ;

    class_<std::vector<HComplex> >("ComplexVec")
        .def(vector_indexing_suite<std::vector<HComplex> >())
      ;

    class_<std::vector<HString> >("StringVec")
        .def(vector_indexing_suite<std::vector<HString> >())
      ;
    class_<hArray<HNumber> >("FloatAry")
      .def("setVector",&hArray<HNumber>::setVector,return_internal_reference<>())
      .def("Vector",&hArray<HNumber>::Vector,return_internal_reference<>())
      .def("getDimensions",&hArray<HNumber>::getDimensions)
      .def("setDimensions",&hArray<HNumber>::setDimensions1,return_internal_reference<>())
      .def("setDimensions",&hArray<HNumber>::setDimensions2,return_internal_reference<>())
      .def("setDimensions",&hArray<HNumber>::setDimensions3,return_internal_reference<>())
      .def("setDimensions",&hArray<HNumber>::setDimensions4,return_internal_reference<>())
      .def("setDimensions",&hArray<HNumber>::setDimensions5,return_internal_reference<>())
      //      .def("setDimensions",fptr_hArray_setDimensions_overloads_1,return_internal_reference<>())
      //.def("setDimensions",fptr_hArray_setDimensions_overloads_2,return_internal_reference<>())
      //      .def("setDimensions",&hArray<HNumber>::setDimensions,return_internal_reference<>(),hArray_setDimensions_overloads())
      .def("setSlice",&hArray<HNumber>::setSlice,return_internal_reference<>())
      .def("getNumberOfDimensions",&hArray<HNumber>::getNumberOfDimensions)
      .def("getBegin",&hArray<HNumber>::getBegin)
      .def("getEnd",&hArray<HNumber>::getEnd)
      ;

    //boost::python::converter::registry::insert(&extract_swig_wrapped_pointer, type_id<mglData>());
    // def("pytointptr",getPointerFromPythonObject);
    //def("getptr", PyGetPtr);

    def("hgetFiletype",hgetFiletype);
    def("hgetFileExtension",hgetFileExtension);

    enum_<DATATYPE>("TYPE")
      .value("POINTER",POINTER)
      .value("INTEGER",INTEGER)
      .value("NUMBER", NUMBER)
      .value("COMPLEX",COMPLEX)
      .value("STRING",STRING)
      .value("BOOLEAN",BOOLEAN)
      .value("UNDEF",UNDEF);

    enum_<hWEIGHTS>("hWEIGHTS")
      .value("FLAT",WEIGHTS_FLAT)
      .value("LINEAR",WEIGHTS_LINEAR)
      .value("GAUSSIAN", WEIGHTS_GAUSSIAN);

    enum_<CR::CoordinateType::Types>("CoordinateTypes")
      //! Azimuth-Elevation-Height, \f$ \vec x = (Az,El,H) \f$
      .value("AzElHeight",CR::CoordinateType::AzElHeight)
      //! Azimuth-Elevation-Radius, \f$ \vec x = (Az,El,R) \f$
      .value("AzElRadius",CR::CoordinateType::AzElRadius)
      //! Cartesian coordinates, \f$ \vec x = (x,y,z) \f$
      .value("Cartesian",CR::CoordinateType::Cartesian)
      //! Cylindrical coordinates, \f$ \vec x = (r,\phi,h) \f$
      .value("Cylindrical",CR::CoordinateType::Cylindrical)
      //! Direction on the sky, \f$ \vec x = (Lon,Lat) \f$
      .value("Direction",CR::CoordinateType::Direction)
      //! Direction on the sky with radial distance, \f$ \vec x = (Lon,Lat,R) \f$
      .value("DirectionRadius",CR::CoordinateType::DirectionRadius)
      //! Frquency
      .value("Frequency",CR::CoordinateType::Frequency)
      //! Longitude-Latitude-Radius
      .value("LongLatRadius",CR::CoordinateType::LongLatRadius)
      //! North-East-Height
      .value("NorthEastHeight",CR::CoordinateType::NorthEastHeight)
      //! Spherical coordinates, \f$ \vec x = (r,\phi,\theta) \f$
      .value("Spherical",CR::CoordinateType::Spherical)
      //! Time
      .value("Time",CR::CoordinateType::Time);

    //The following will include the automatically generated python wrappers
#include "../../../../build/cr/implement/Pypeline/hftools.def.h"

}    

#undef HFPP_FILETYPE
