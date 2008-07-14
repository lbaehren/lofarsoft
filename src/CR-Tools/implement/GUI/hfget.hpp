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



//Assignment of function pointers
#define DEF_DATA_OID_NAME_FUNC_PY( TYPE, FUNC  ) \
TYPE (Data::*Data_##FUNC)() = &Data::FUNC;\
TYPE (Data::*Data_##FUNC##_Name)(String) = &Data::FUNC;\
TYPE (Data::*Data_##FUNC##_ID)(objectid) = &Data::FUNC;

//Definition of python objects, pointing to the pointer
#define DEF_DATA_OID_NAME_FUNC_PYDEF( FUNC  ) \
.def(#FUNC ,Data_##FUNC)\
.def(#FUNC ,Data_##FUNC##_Name)\
.def(#FUNC ,Data_##FUNC##_ID)

//------------------------------------------------------------------------

#define DEF_DATA_OID_NAME_FUNC_PY_0( EXT, TYPE, FUNC) \
TYPE (Data::*Data_##FUNC##EXT)() = &Data::FUNC##EXT;\
TYPE (Data::*Data_##FUNC##_Name##EXT)(String) = &Data::FUNC##EXT;\
TYPE (Data::*Data_##FUNC##_ID##EXT)(objectid) = &Data::FUNC##EXT;\

#define DEF_DATA_OID_NAME_FUNC_PY_1( EXT, TYPE, FUNC, TYPE1 ) \
TYPE (Data::*Data_##FUNC##EXT)(TYPE1) = &Data::FUNC##EXT;\
TYPE (Data::*Data_##FUNC##_Name##EXT)(String,TYPE1) = &Data::FUNC##EXT;\
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



  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_printStatus_overloads,printStatus,0,1)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_printDecendants_overloads,printDecendants,0,0)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_setLink_Name_overloads,setLink_Name,1,3)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_setLink_ID_overloads,setLink_ID,1,3)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_setLink_Ref_overloads,setLink_Ref,1,3)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_setFunction_overloads,setFunction,2,3)
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(Data_recalc_overloads,recalc,0,1)

    void (Data::*Data_get_I)(vector<Integer>&) = &Data::get_1_;
    void (Data::*Data_get_N)(vector<Number>&) = &Data::get_1_;
    void (Data::*Data_get_C)(vector<Complex>&) = &Data::get_1_;
    void (Data::*Data_get_S)(vector<String>&) = &Data::get_1_;

    void (Data::*Data_put_I)(vector<Integer>&) = &Data::put;
    void (Data::*Data_put_N)(vector<Number>&) = &Data::put;
    void (Data::*Data_put_C)(vector<Complex>&) = &Data::put;
    void (Data::*Data_put_S)(vector<String>&) = &Data::put;

    void (Data::*Data_put_name_I)(String, vector<Integer>&) = &Data::put;
    void (Data::*Data_put_name_N)(String, vector<Number>&) = &Data::put;
    void (Data::*Data_put_name_C)(String, vector<Complex>&) = &Data::put;
    void (Data::*Data_put_name_S)(String, vector<String>&) = &Data::put;

    void (Data::*Data_put_oid_I)(objectid, vector<Integer>&) = &Data::put;
    void (Data::*Data_put_oid_N)(objectid, vector<Number>&) = &Data::put;
//   void (Data::*Data_put_oid_C)(objectid, vector<Complex>&) = &Data::put;
    void (Data::*Data_put_oid_S)(objectid, vector<String>&) = &Data::put;

    void (Data::*Data_putOne_I)(Integer) = &Data::putOne;
    void (Data::*Data_putOne_N)(Number) = &Data::putOne;
//    void (Data::*Data_putOne_C)(Complex) = &Data::putOne;
    void (Data::*Data_putOne_S)(String) = &Data::putOne;

    void (Data::*Data_putOne_name_I)(String, Integer) = &Data::putOne;
    void (Data::*Data_putOne_name_N)(String, Number) = &Data::putOne;
    void (Data::*Data_putOne_name_C)(String, Complex) = &Data::putOne;
    void (Data::*Data_putOne_name_S)(String, String) = &Data::putOne;

    void (Data::*Data_putOne_oid_I)(objectid, Integer) = &Data::putOne;
    void (Data::*Data_putOne_oid_N)(objectid, Number) = &Data::putOne;
    void (Data::*Data_putOne_oid_C)(objectid, Complex) = &Data::putOne;
    void (Data::*Data_putOne_oid_S)(objectid, String) = &Data::putOne;

    Integer (Data::*Data_getOne_I_1)(address) = &Data::getOne_1_<Integer>;
    Number  (Data::*Data_getOne_N_1)(address) = &Data::getOne_1_<Number>;
    Complex (Data::*Data_getOne_C_1)(address) = &Data::getOne_1_<Complex>;
    String  (Data::*Data_getOne_S_1)(address) = &Data::getOne_1_<String>;

    Integer(Data::*Data_getOne_I_0)() = &Data::getOne_0_<Integer>;
    Number (Data::*Data_getOne_N_0)() = &Data::getOne_0_<Number>;
    Complex(Data::*Data_getOne_C_0)() = &Data::getOne_0_<Complex>;
    String (Data::*Data_getOne_S_0)() = &Data::getOne_0_<String>;

    
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
      .value("UNDEF",UNDEF)
      .value("INTEGER",INTEGER)
      .value("NUMBER", NUMBER)
      .value("COMPLEX",COMPLEX)
      .value("STRING",STRING);

    class_<Data>("Data", init<std::string>())
      //overloads the [] operator for indexing of objects
      .def("__getitem__",&Data::Object_ID,return_internal_reference<>())
      .def("__getitem__",&Data::Object_Name,return_internal_reference<>())

      .def("getType", &Data::getType)
      .def("getName", &Data::getName)
      .def("getOid", &Data::getOid)
      .def("setModification", &Data::setModification)
      .def("Empty", &Data::Empty)
      .def("Silent", &Data::Silent)
      .def("clearModification", &Data::clearModification)
      .def("checkModification", &Data::checkModification)

      .def("sendMessage", &Data::sendMessage)
      .def("getMessage", &Data::getMessage)
      .def("setPort", &Data::setPort)
      .def("touch", &Data::touch)
      .def("recalc", &Data::recalc, Data_recalc_overloads())
      .def("redo", &Data::redo)//is actually almost the same


      .def("printStatus", &Data::printStatus,Data_printStatus_overloads())
      .def("printDecendants", &Data::printDecendants,Data_printDecendants_overloads())

      .def("delLink", &Data::delLink_ID)
      .def("setLink", &Data::setLink_Name, Data_setLink_Name_overloads())
      .def("setLink", &Data::setLink_ID,   Data_setLink_ID_overloads())
      .def("setLink", &Data::setLink_Ref,  Data_setLink_Ref_overloads())

      .def("get", Data_get_I)
      .def("get", Data_get_N)
      .def("get", Data_get_C)
      .def("get", Data_get_S)

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
      //      .def("__setitem__", Data_put_oid_C)
      .def("__setitem__", Data_put_oid_S)

      .def("__setitem__", Data_putOne_name_I)
      .def("__setitem__", Data_putOne_name_N)
      //      .def("__setitem__", Data_putOne_name_C)
      .def("__setitem__", Data_putOne_name_S)

      .def("__setitem__", Data_putOne_oid_I)
      .def("__setitem__", Data_putOne_oid_N)
      .def("__setitem__", Data_putOne_oid_C)
      .def("__setitem__", Data_putOne_oid_S)


      .def("putI", Data_putOne_I)
      .def("putN", Data_putOne_N)
      //      .def("putC", Data_putOne_C)
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
      .def("setFunction", &Data::setFunction,Data_setFunction_overloads())
      .def("printAllStatus", &Data::printAllStatus)
      .def("delObject", &Data::delObject_ID)
      .def("delObject", &Data::delObject_Name)
      .def("newObjects", &Data::newObjects_ID)
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
    
    class_<std::vector<Integer> >("IntVec")
        .def(vector_indexing_suite<std::vector<Integer> >())
    ;

    class_<std::vector<Number> >("FloatVec")
        .def(vector_indexing_suite<std::vector<Number> >())
    ;
    
    class_<std::vector<bool> >("BoolVec")
        .def(vector_indexing_suite<std::vector<bool> >())
    ;
    
    class_<std::vector<Complex> >("ComplexVec")
        .def(vector_indexing_suite<std::vector<Complex> >())
    ;

    class_<std::vector<String> >("StringVec")
        .def(vector_indexing_suite<std::vector<String> >())
    ;

    def("mytest", mytest);
    def("mytest2", mytest2);
    def("mytest3", mytest3);
    //    def("mycast", mycast);
    //    def("printvec", printvec);
    //def("split_str_into_vector", split_str_into_vector);
}
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
