////////////////////////////////////////////////////////////////////////
//Definition of Python bindings for hfget functions ...
////////////////////////////////////////////////////////////////////////


/*
run python with:

python -i hfget.py
or
python
execfile("hfget.py")
*/
#undef HF_PP_FILETYPE
#define HF_PP_FILETYPE() (PYTHON)  // Tell the preprocessor (for generating wrappers) that this is a c++ header file for Python exposure (brackets are crucial)

#define hPyExposeINCS(FUNC)  def(#FUNC,FUNC##_I);def(#FUNC,FUNC##_N);def(#FUNC,FUNC##_C);def(#FUNC,FUNC##_S)
#define hPyExposeINC(FUNC)  def(#FUNC,FUNC##_I);def(#FUNC,FUNC##_N);def(#FUNC,FUNC##_C)

//Assignment of function pointers
#define DEF_DATA_OID_NAME_FUNC_PY( TYPE, FUNC  ) \
TYPE (Data::*Data_##FUNC)() = &Data::FUNC;\
TYPE (Data::*Data_##FUNC##_Name)(HString) = &Data::FUNC;\
TYPE (Data::*Data_##FUNC##_ID)(objectid) = &Data::FUNC;

//Definition of python objects, pointing to the pointer
#define DEF_DATA_OID_NAME_FUNC_PYDEF( FUNC  ) \
.def(#FUNC ,Data_##FUNC)\
.def(#FUNC ,Data_##FUNC##_Name)\
.def(#FUNC ,Data_##FUNC##_ID)

//------------------------------------------------------------------------

#define DEF_DATA_OID_NAME_FUNC_PY_0( EXT, TYPE, FUNC) \
TYPE (Data::*Data_##FUNC##EXT)() = &Data::FUNC##EXT;\
TYPE (Data::*Data_##FUNC##_Name##EXT)(HString) = &Data::FUNC##EXT;\
TYPE (Data::*Data_##FUNC##_ID##EXT)(objectid) = &Data::FUNC##EXT;\

#define DEF_DATA_OID_NAME_FUNC_PY_1( EXT, TYPE, FUNC, TYPE1 ) \
TYPE (Data::*Data_##FUNC##EXT)(TYPE1) = &Data::FUNC##EXT;\
TYPE (Data::*Data_##FUNC##_Name##EXT)(HString,TYPE1) = &Data::FUNC##EXT;\
TYPE (Data::*Data_##FUNC##_ID##EXT)(objectid,TYPE1) = &Data::FUNC##EXT;

#define DEF_DATA_OID_NAME_FUNC_PYDEF_EXT( EXT, FUNC  ) \
.def(#FUNC ,Data_##FUNC##EXT)\
.def(#FUNC ,Data_##FUNC##_Name##EXT)\
.def(#FUNC ,Data_##FUNC##_ID##EXT)

//------------------------------------------------------------------------

#define DEF_DATA_OID_NAME_FUNC_PY_0_1( TYPE, FUNC, TYPE1 ) \
DEF_DATA_OID_NAME_FUNC_PY_0(_0_, TYPE, FUNC)\
DEF_DATA_OID_NAME_FUNC_PY_1( ,TYPE, FUNC, TYPE1 )


#define DEF_DATA_OID_NAME_FUNC_PYDEF_0_1( FUNC  ) \
DEF_DATA_OID_NAME_FUNC_PYDEF_EXT(_0_, FUNC  ) \
DEF_DATA_OID_NAME_FUNC_PYDEF_EXT( , FUNC  ) 


//========================================================================

//BOOST_PYTHON_FUNCTION_OVERLOADS(hRunningAverageVec_N,hRunningAverageVec<HNumber>,2,4)

  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_listFunctions_overloads,listFunctions,0,2)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_printStatus_overloads,printStatus,0,1)
  //  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_touch_overloads,touch,0,1)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_printAllStatus_overloads,printAllStatus,0,3)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_Status_overloads,Status,0,1)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_getName_overloads,getName,0,1)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_getSearchName_overloads,getSearchName,0,1)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_FindChainID_overloads,FindChainID,2,4)

  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_listNeighbourIDs_overloads,listNeighbourIDs,1,2)


  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_printDecendants_overloads,printDecendants,0,0)
  //  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_setLink_Name_overloads,setLink_Name,1,4)
  //  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_setLink_ID_overloads,setLink_ID,1,4)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_setLink_Ref_overloads,setLink_Ref,1,3)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_setFunction_overloads,setFunction,2,3)
  //BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_update_overloads,update,0,1)

  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_newObjects_ID_overloads,newObjects_ID,1,2)


  

    Data& (Data::*Data_touch1)(bool) = &Data::touch;

    void (Data::*Data_delLink)(Data &) = &Data::delLink;

    void (Data::*Data_get_I)(vector<HInteger>&) /*const*/ = &Data::get_1_;
    void (Data::*Data_get_N)(vector<HNumber>&) /*const*/ = &Data::get_1_;
    void (Data::*Data_get_C)(vector<HComplex>&) /*const*/ = &Data::get_1_;
    void (Data::*Data_get_S)(vector<HString>&) /*const*/ = &Data::get_1_;

    void (Data::*Data_inspect_I)(vector<HInteger>&) = &Data::inspect;
    void (Data::*Data_inspect_N)(vector<HNumber>&) = &Data::inspect;
    void (Data::*Data_inspect_C)(vector<HComplex>&) = &Data::inspect;
    void (Data::*Data_inspect_S)(vector<HString>&) = &Data::inspect;

    void (Data::*Data_put_I)(vector<HInteger>&) = &Data::put;
    void (Data::*Data_put_N)(vector<HNumber>&) = &Data::put;
    void (Data::*Data_put_C)(vector<HComplex>&) = &Data::put;
    void (Data::*Data_put_S)(vector<HString>&) = &Data::put;

    void (Data::*Data_put_name_I)(HString, vector<HInteger>&) = &Data::put;
    void (Data::*Data_put_name_N)(HString, vector<HNumber>&) = &Data::put;
    void (Data::*Data_put_name_C)(HString, vector<HComplex>&) = &Data::put;
    void (Data::*Data_put_name_S)(HString, vector<HString>&) = &Data::put;

    void (Data::*Data_put_oid_I)(objectid, vector<HInteger>&) = &Data::put;
    void (Data::*Data_put_oid_N)(objectid, vector<HNumber>&) = &Data::put;
    void (Data::*Data_put_oid_C)(objectid, vector<HComplex>&) = &Data::put;
    void (Data::*Data_put_oid_S)(objectid, vector<HString>&) = &Data::put;

    void (Data::*Data_putOne_I)(HInteger) = &Data::putOne;
    void (Data::*Data_putOne_N)(HNumber) = &Data::putOne;
    void (Data::*Data_putOne_C)(HComplex) = &Data::putOne;
    void (Data::*Data_putOne_S)(HString) = &Data::putOne;

    void (Data::*Data_putOne_name_I)(HString, HInteger) = &Data::putOne;
    void (Data::*Data_putOne_name_N)(HString, HNumber) = &Data::putOne;
    void (Data::*Data_putOne_name_C)(HString, HComplex) = &Data::putOne;
    void (Data::*Data_putOne_name_S)(HString, HString) = &Data::putOne;

    void (Data::*Data_putOne_oid_I)(objectid, HInteger) = &Data::putOne;
    void (Data::*Data_putOne_oid_N)(objectid, HNumber) = &Data::putOne;
    void (Data::*Data_putOne_oid_C)(objectid, HComplex) = &Data::putOne;
    void (Data::*Data_putOne_oid_S)(objectid, HString) = &Data::putOne;

    HInteger (Data::*Data_getOne_I_1)(address)  /*const*/ = &Data::getOne<HInteger>;
    HNumber  (Data::*Data_getOne_N_1)(address)  /*const*/ = &Data::getOne<HNumber>;
    HComplex (Data::*Data_getOne_C_1)(address)  /*const*/ = &Data::getOne<HComplex>;
    HString  (Data::*Data_getOne_S_1)(address)  /*const*/ = &Data::getOne<HString>;

    HInteger(Data::*Data_getOne_I_0)() /*const*/ = &Data::getOne_0_<HInteger>;
    HNumber (Data::*Data_getOne_N_0)() /*const*/ = &Data::getOne_0_<HNumber>;
    HComplex(Data::*Data_getOne_C_0)() /*const*/ = &Data::getOne_0_<HComplex>;
    HString (Data::*Data_getOne_S_0)() /*const*/ = &Data::getOne_0_<HString>;

/*
HInteger(Data::*Data_getParameter_I)(HString, HInteger) = &Data::getParameter;
HNumber (Data::*Data_getParameter_N)(HString, HNumber) = &Data::getParameter;
HComplex(Data::*Data_getParameter_C)(HString, HComplex) = &Data::getParameter;
HString (Data::*Data_getParameter_S)(HString, HString) = &Data::getParameter;
*/


    
//used to be BOOST_PYTHON_MODULE(hfget)
BOOST_PYTHON_MODULE(libhfget)
{

    using namespace boost::python;

    enum_<DIRECTION>("DIR")
      .value("NONE",DIR_NONE)
      .value("FROM",DIR_FROM)
      .value("TO", DIR_TO)
      .value("BOTH",DIR_BOTH);

    enum_<DATATYPE>("TYPE")
      .value("POINTER",POINTER)
      .value("INTEGER",INTEGER)
      .value("NUMBER", NUMBER)
      .value("COMPLEX",COMPLEX)
      .value("STRING",STRING)
	.value("UNDEF",UNDEF);

    class_<Data>("Data", init<std::string>())
      //overloads the [] operator for indexing of objects
      .def("__getitem__",&Data::Object_ID,return_internal_reference<>())
      .def("__getitem__",&Data::Object_Name,return_internal_reference<>())
      .def("Find",&Data::Object_Name,return_internal_reference<>())
      .def("Find",&Data::Object_ID,return_internal_reference<>())
      .def("FindChainID", &Data::FindChainID,Data_FindChainID_overloads())

      .def("getType", &Data::getType)
      .def("setType", &Data::setType)
      .def("getName", &Data::getName,Data_getName_overloads())
      .def("getSearchName", &Data::getSearchName,Data_getSearchName_overloads())
      .def("getNetLevel", &Data::getNetLevel)
      .def("setNetLevel", &Data::setNetLevel,return_internal_reference<>())
      .def("getVersion", &Data::getVersion)
      .def("setVersion", &Data::setVersion)
      .def("getOid", &Data::getOid)
      .def("getMod", &Data::getMod)
      .def("strModFlag", &Data::strModFlag)
      .def("getModFlags", &Data::getModFlagsStr)
      .def("setDefaultDirection", &Data::setDefaultDirection)
      .def("getDefaultDirection", &Data::getDefaultDirection)
      .def("isModified", &Data::isModified)
      .def("getFuncName", &Data::getFuncName)
      .def("__len__", &Data::len)
      .def("Empty", &Data::Empty)
      .def("hasFunc", &Data::hasFunc)
      .def("hasData", &Data::hasData)
      .def("Silent", &Data::Silent)
      .def("doSilent", &Data::doSilent_Ref,return_internal_reference<>())
      .def("noMod", &Data::noMod_Ref,return_internal_reference<>())
      .def("noSignal", &Data::noSignal,return_internal_reference<>())
      .def("needsUpdate", &Data::needsUpdate)
      .def("doAutoUpdate", &Data::doAutoUpdate)
      .def("doesAutoUpdate", &Data::doesAutoUpdate)
      .def("setAutoUpdate", &Data::setAutoUpdate)
      .def("Updateable", &Data::Updateable)
      .def("setUpdateable", &Data::setUpdateable)
      .def("setVerbose", &Data::setVerbose)
      .def("Verbose", &Data::Verbose)
      .def("setDebugGUI", &Data::setDebugGUI)
      .def("getDebugGUI", &Data::getDebugGUI)
      .def("setDebug", &Data::setDebug)
      .def("getDebug", &Data::getDebug)
      .def("printWorm", &Data::printWorm)

      .def("AllVerbose", &Data::AllVerbose)
      .def("clearModification", &Data::clearModification)

      .def("sendMessage", &Data::sendMessage) //not really implemented
      .def("getMessage", &Data::getMessage) //not really implemented
      .def("setPort", &Data::setPort)
      .def("touch", &Data::touch_0,return_internal_reference<>())
      .def("touch", Data_touch1,return_internal_reference<>())
      .def("update", &Data::update,return_internal_reference<>())
      .def("wakeUp", &Data::wakeUp,return_internal_reference<>())
      .def("updateAll", &Data::updateAll)

      .def("Status", &Data::Status,Data_Status_overloads())
      .def("printStatus", &Data::printStatus,Data_printStatus_overloads())
      .def("printAllStatus", &Data::printAllStatus,Data_printAllStatus_overloads())
      .def("printDecendants", &Data::printDecendants,Data_printDecendants_overloads())
      .def("listNeighbourNames", &Data::listNeighbourNames)
      .def("listNeighbourIDs", &Data::listNeighbourIDs,Data_listNeighbourIDs_overloads())
      .def("listNeighbourDirs", &Data::listNeighbourDirs)
      .def("listNeighbourModFlags", &Data::listNeighbourModFlags)
      .def("getAllIDs", &Data::getAllIDs)
      .def("getNeighbourNames", &Data::getNeighbourNames)
      .def("getNeighboursList", &Data::getNeighboursList)
      .def("getLinkDirection", &Data::getLinkDirection)
      .def("getLinkDirectionType", &Data::getLinkDirectionType)
 
      .def("delLink", Data_delLink)
      .def("delLink", &Data::delLink_ID)
      //      .def("setLink", &Data::setLink_Name, Data_setLink_Name_overloads())
      //.def("setLink", &Data::setLink_ID,   Data_setLink_ID_overloads())
      .def("setLink", &Data::setLink_Ref_3,return_internal_reference<>())
      .def("setLink", &Data::setLink_Ref_2,return_internal_reference<>())
      .def("setLink", &Data::setLink_Ref_1,return_internal_reference<>())
      .def("resetLink_cc", &Data::resetLink_Ref_3,return_internal_reference<>())
      .def("resetLink_cc", &Data::resetLink_Ref_2,return_internal_reference<>())
      .def("resetLink_cc", &Data::resetLink_Ref_1,return_internal_reference<>())

      .def("get", Data_get_I)
      .def("get", Data_get_N)
      .def("get", Data_get_C)
      .def("get", Data_get_S)

      /*      .def("getParameter", Data_getParameter_I)
      .def("getParameter", Data_getParameter_N)
      .def("getParameter", Data_getParameter_C)
      .def("getParameter", Data_getParameter_S)
      */

      .def("inspect", Data_inspect_I)
      .def("inspect", Data_inspect_N)
      .def("inspect", Data_inspect_C)
      .def("inspect", Data_inspect_S)

      .def("put", Data_put_I)
      .def("put", Data_put_N)
      .def("put", Data_put_C)
      .def("put", Data_put_S)


      .def("__setitem__", Data_put_name_I)
      .def("__setitem__", Data_put_name_N)
      .def("__setitem__", Data_put_name_C)
      .def("__setitem__", Data_put_name_S)

      .def("__setitem__", Data_put_oid_I)
      .def("__setitem__", Data_put_oid_N)
      .def("__setitem__", Data_put_oid_C)
      .def("__setitem__", Data_put_oid_S)

      .def("__setitem__", Data_putOne_name_I)
      .def("__setitem__", Data_putOne_name_N)
      .def("__setitem__", Data_putOne_name_C)
      .def("__setitem__", Data_putOne_name_S)

      .def("__setitem__", Data_putOne_oid_I)
      .def("__setitem__", Data_putOne_oid_N)
      .def("__setitem__", Data_putOne_oid_C)
      .def("__setitem__", Data_putOne_oid_S)

      .def("putPyList", &Data::putPyList,return_internal_reference<>())
      .def("getPyList", &Data::getPyList)
      .def("putPy_silent", &Data::putPy_silent,return_internal_reference<>())
      .def("putPy", &Data::putPy,return_internal_reference<>())
      .def("getPy", &Data::getPy)

      .def("storePyFunc", &Data::storePyFunc,return_internal_reference<>())
      .def("deletePyFunc", &Data::deletePyFunc,return_internal_reference<>())
      .def("retrievePyFuncObject", &Data::retrievePyFuncObject)

      .def("storePyQt", &Data::storePyQt,return_internal_reference<>())
      .def("deletePyQt", &Data::deletePyQt,return_internal_reference<>())
      .def("retrievePyQtObject", &Data::retrievePyQtObject)
      .def("signalPyQt", &Data::signalPyQt)

      .def("AllStorePyDBG", &Data::AllStorePyDBG,return_internal_reference<>())
      .def("storePyDBG", &Data::storePyDBG,return_internal_reference<>())
      .def("deletePyDBG", &Data::deletePyDBG,return_internal_reference<>())
      .def("retrievePyDBGObject", &Data::retrievePyDBGObject)
      .def("signalPyDBG", &Data::signalPyDBG)

      .def("putI", Data_putOne_I)
      .def("putN", Data_putOne_N)
      .def("putC", Data_putOne_C)
      .def("putS", Data_putOne_S)

      .def("getI", Data_getOne_I_0)
      .def("getN", Data_getOne_N_0)
      .def("getC", Data_getOne_C_0)
      .def("getS", Data_getOne_S_0)
      .def("getI", Data_getOne_I_0)
      .def("getN", Data_getOne_N_0)
      .def("getC", Data_getOne_C_0)
      .def("getS", Data_getOne_S_0)

      .def("delData", &Data::delData)
      .def("delFunction", &Data::delFunction)
      .def("listFunctions", &Data::listFunctions,Data_listFunctions_overloads())
      .def("setFunction", &Data::setFunction,return_internal_reference<>(),Data_setFunction_overloads())
      //      .def("listFunctions", &Data::listFunctions)
      .def("delObject", &Data::delObject_ID)
      .def("delObject", &Data::delObject_Name)
      .def("delObject", &Data::delObject)
      .def("erase", &Data::erase,return_internal_reference<>())
      .def("erase", &Data::erase_1,return_internal_reference<>())
      .def("__ifloordiv__", &Data::erase_1,return_internal_reference<>()) // this is the "//=" operator
      .def("insert", &Data::insert_Ref,return_internal_reference<>())
      .def("insertNew", &Data::insertNew_Ref,return_internal_reference<>())
      .def("newObjects", &Data::newObjects_ID,Data_newObjects_ID_overloads())
      .def("newObject", &Data::newObject_Ref,return_internal_reference<>())
      .def("newObject", &Data::newObject_Ref_1,return_internal_reference<>())
      .def("create", &Data::create_Ref,return_internal_reference<>())
      .def("IDs", &Data::IDs)
      .def("ID", &Data::ID)

      .def("Object", &Data::Object_ID,return_internal_reference<>())
      .def("Object", &Data::Object_Name,return_internal_reference<>())
      .def("Object", &Data::Object_Ref,return_internal_reference<>())


      /*
#undef RPT_TEMPLATED_MEMBER_FUNCTION
#define RPT_TEMPLATED_MEMBER_FUNCTION( TT, IDNAME, INCS, DTYP ) \
      .def("getOne", &Data::getOne< DTYP >)
DEF_TEMPLATED_MEMBER_FUNCTIONS

#undef RPT_TEMPLATED_MEMBER_FUNCTION
#define RPT_TEMPLATED_MEMBER_FUNCTION( TT, IDNAME, INCS, DTYP ) \
      .def("putOne", &Data::putOne< DTYP >)
DEF_TEMPLATED_MEMBER_FUNCTIONS


      
#undef RPT_TEMPLATED_MEMBER_FUNCTION
#define RPT_TEMPLATED_MEMBER_FUNCTION( TT, IDNAME, INCS, DTYP ) \
      .def("put", Data_put_ ## INCS)
DEF_TEMPLATED_MEMBER_FUNCTIONS

#undef RPT_TEMPLATED_MEMBER_FUNCTION
#define RPT_TEMPLATED_MEMBER_FUNCTION( TT, IDNAME, INCS, ZZ ) \
      .def("put", Data_put_ ## IDNAME ## _ ## INCS)
DEF_TEMPLATED_MEMBER_FUNCTIONS
      */

        ;

    //Define vectors in Python ...

/*    class_<casa::Vector<casa::Double> >("FloatAVec")
        .def(vector_indexing_suite<casa::Vector<casa::Double> >())
    ;
*/

    class_<std::vector<DIRECTION> >("DirVec")
        .def(vector_indexing_suite<std::vector<DIRECTION> >())
    ;
    class_<std::vector<HInteger> >("IntVec")
        .def(vector_indexing_suite<std::vector<HInteger> >())
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

boost::python::converter::registry::insert(&extract_swig_wrapped_pointer, type_id<mglData>());

/*
 def("hVecBuffer",hVecBuffer);
 def("mglDataSetVecN", mglDataSetVecN);
 def("mglDataSetVecNSwig", mglDataSetVecNSwig);
 def("mglDataSetVecNi", mglDataSetVecNi);
 def("mglDataPtr", mglDataPtr);
 def("hAbsComplexVec", hAbsComplexVec);
*/
 def("mglDataGetVecPos", mglDataGetVecPos);
 def("mglDataGetYfromX",mglDataGetYfromX);
 def("mglDataGetVecElement",mglDataGetVecElement);
 def("pytointptr",getPointerFromPythonObject);
 def("getptr", PyGetPtr);
 def("setDebug", setDebug);
 def("file_get_extension",file_get_extension);
 def("determine_filetype",determine_filetype);
 def("hOpenFile",hOpenFile);
 def("hCloseFile",hCloseFile);

 def("hRunningAverage",hRunningAverageVec_N);
 def("hRunningAverage",hRunningAverageVec_I);
 def("hRunningAverage",hRunningAverageVec_C);
 def("hWeights",hWeights);

#include "hfwrappers-hfanalysis.cc.h"
 hPyExposeINCS(hReadFile);
 

 enum_<hWEIGHTS>("hWEIGHTS")
   .value("FLAT",WEIGHTS_FLAT)
   .value("LINEAR",WEIGHTS_LINEAR)
   .value("GAUSSIAN", WEIGHTS_GAUSSIAN);


//    def("setwidget", setwidget);
//    def("mytest", mytest);
//    def("mytest2", mytest2);
//    def("mytest3", mytest3);
//    def("mytest4", mytest4,return_internal_reference<>());
//    def("mytest6", mytest6,return_internal_reference<>());
//    def("mytest5", &mytest5);
    //    def("mycast", mycast);
    //    def("printvec", printvec);
    //def("split_str_into_vector", split_str_into_vector);
}

/*
boost::python::object pytst(){
  Data d;
  boost::python::object o(d);
  //  PyObject p(o);
  //  return &o;
}
def("pytst",pytst);
*/

    /*
    class_<X>("X")
        .def(init<>())
        .def(init<X>())
        .def(init<std::string>())
        .def("__repr__", &X::repr)
        .def("reset", &X::reset)
        .def("foo", &X::foo)
    ;

    def("x_value", x_value);
    implicitly_convertible<std::string, X>();
    
    class_<std::vector<X> >("XVec")
        .def(vector_indexing_suite<std::vector<X> >())
    ;
        
    // Compile check only...
}
    */

#undef HF_PP_FILETYPE
