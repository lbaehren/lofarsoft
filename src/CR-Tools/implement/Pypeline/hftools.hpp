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

#include "hftools.def.h"

}
#undef HFPP_FILETYPE
