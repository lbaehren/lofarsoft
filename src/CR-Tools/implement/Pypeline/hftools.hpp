////////////////////////////////////////////////////////////////////////
//Definition of Python bindings for hftools ...
////////////////////////////////////////////////////////////////////////



// Tell the preprocessor (for generating wrappers) that this is a c++
// header file for Python exposure
#undef HFPP_FILETYPE
#define HFPP_FILETYPE hPYTHON

BOOST_PYTHON_MODULE(hftools)
{
    using namespace boost::python;

    class_<CR::DataReader>("DataReader")
      //      .def("read",&hFileRead) -> This is defined in pycrtools.py, since it is templated
      .def("get",&hFileGetParameter)
      .def("set",&hFileSetParameter)
      .def("summary",&hFileSummary)
      ;    

    class_<std::hmatrix<HInteger,allocator<HInteger> > >("IntMatrix")
      .def(vector_indexing_suite<std::hmatrix<HInteger,allocator<HInteger> > >())
      //      .def("setDimension",&std::hmatrix<HInteger,allocator<HInteger> >::setDimension)
      //.def("getDimension",&std::hmatrix<HInteger,allocator<HInteger> >::getDimension)

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

    //boost::python::converter::registry::insert(&extract_swig_wrapped_pointer, type_id<mglData>());
    // def("pytointptr",getPointerFromPythonObject);
    //def("getptr", PyGetPtr);

    def("hgetFiletype",hgetFiletype);
    def("hgetFileExtension",hgetFileExtension);

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
