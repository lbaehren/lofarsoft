#define DEF_DATA_OID_NAME_FUNC_0( DTYPE, DFUNC) \
DTYPE Data :: DFUNC##_0_  (HString x){return Object(x)->DFUNC();}\
DTYPE Data :: DFUNC##_0_  (objectid x){return Object(x)->DFUNC();}\
DTYPE Data :: DFUNC##_0_  (){return DFUNC();}
#define DEF_DATA_OID_NAME_FUNC_0b( DTYPE, DFUNC) \
DTYPE Data :: DFUNC  (HString x){return Object(x)->DFUNC();}\
DTYPE Data :: DFUNC  (objectid x){return Object(x)->DFUNC();}\
DTYPE Data :: DFUNC  ()

#define DEF_DATA_OID_NAME_FUNC_1( DTYPE, DFUNC, TYPE1, PAR1) \
DTYPE Data :: DFUNC##_1_  (TYPE1 PAR1){return DFUNC(PAR1);}\
DTYPE Data :: DFUNC##_1_  (HString x, TYPE1 PAR1){return Object(x)->DFUNC(PAR1);}\
DTYPE Data :: DFUNC##_1_  (objectid x, TYPE1 PAR1){return Object(x)->DFUNC(PAR1);}
#define DEF_DATA_OID_NAME_FUNC_1b( DTYPE, DFUNC, TYPE1, PAR1) \
DTYPE Data :: DFUNC  (HString x, TYPE1 PAR1){return Object(x)->DFUNC(PAR1);}\
DTYPE Data :: DFUNC  (objectid x, TYPE1 PAR1){return Object(x)->DFUNC(PAR1);}\
DTYPE Data :: DFUNC  (TYPE1 PAR1)

#define DEF_DATA_OID_NAME_FUNC_2( DTYPE, DFUNC, TYPE1, PAR1, TYPE2, PAR2  ) \
DTYPE Data :: DFUNC##_2_  (HString x, TYPE1 PAR1, TYPE2 PAR2){return Object(x)->DFUNC(PAR1,PAR2);}\
DTYPE Data :: DFUNC##_2_  (objectid x, TYPE1 PAR1, TYPE2 PAR2){return Object(x)->DFUNC(PAR1,PAR2);}\
DTYPE Data :: DFUNC##_2_  (TYPE1 PAR1, TYPE2 PAR2){return Object(x)->DFUNC(PAR1,PAR2);}

#define DEF_DATA_OID_NAME_FUNC_2b( DTYPE, DFUNC, TYPE1, PAR1, TYPE2, PAR2  ) \
DTYPE Data :: DFUNC  (HString x, TYPE1 PAR1, TYPE2 PAR2){return Object(x)->DFUNC(PAR1,PAR2);}\
DTYPE Data :: DFUNC  (objectid x, TYPE1 PAR1, TYPE2 PAR2){return Object(x)->DFUNC(PAR1,PAR2);}\
DTYPE Data :: DFUNC  (TYPE1 PAR1, TYPE2 PAR2)

#define DEF_DATA_OID_NAME_FUNC_3( DTYPE, DFUNC, TYPE1, PAR1, TYPE2, PAR2, TYPE3, PAR3) \
DTYPE Data :: DFUNC##_3_  (HString x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return Object(x)->DFUNC(PAR1,PAR2,PAR3);}\
DTYPE Data :: DFUNC##_3_  (objectid x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return Object(x)->DFUNC(PAR1,PAR2,PAR3);}\
DTYPE Data :: DFUNC##_3_  (TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return Object(x)->DFUNC(PAR1,PAR2,PAR3);}

#define DEF_DATA_OID_NAME_FUNC_3b( DTYPE, DFUNC, TYPE1, PAR1, TYPE2, PAR2, TYPE3, PAR3) \
DTYPE Data :: DFUNC  (HString x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return Object(x)->DFUNC(PAR1,PAR2,PAR3);}\
DTYPE Data :: DFUNC  (objectid x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return Object(x)->DFUNC(PAR1,PAR2,PAR3);}\
DTYPE Data :: DFUNC  (TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3)

#define DEF_DATA_OID_NAME_FUNC_DEF_0( TYPE, FUNC) \
TYPE  FUNC##_0_  ();\
TYPE  FUNC##_0_  (objectid);\
TYPE  FUNC##_0_  (HString);
#define DEF_DATA_OID_NAME_FUNC_DEF_0b( TYPE, FUNC) \
TYPE  FUNC  ();\
TYPE  FUNC  (objectid);\
TYPE  FUNC  (HString);\

#define DEF_DATA_OID_NAME_FUNC_DEF_1( TYPE, FUNC, TYPE1 ) \
TYPE  FUNC##_1_  (TYPE1);\
TYPE  FUNC##_1_  (objectid, TYPE1);\
TYPE  FUNC##_1_  (HString, TYPE1);
#define DEF_DATA_OID_NAME_FUNC_DEF_1b( TYPE, FUNC, TYPE1 ) \
TYPE  FUNC  (TYPE1);\
TYPE  FUNC  (objectid, TYPE1);\
TYPE  FUNC  (HString, TYPE1);\

#define DEF_DATA_OID_NAME_FUNC_DEF_2( TYPE, FUNC, TYPE1, TYPE2 ) \
TYPE  FUNC##_2_  (TYPE1, TYPE2);\
TYPE  FUNC##_2_  (objectid, TYPE1, TYPE2);\
TYPE  FUNC##_2_  (HString, TYPE1, TYPE2);
#define DEF_DATA_OID_NAME_FUNC_DEF_2b( TYPE, FUNC, TYPE1, TYPE2 ) \
TYPE  FUNC  (TYPE1, TYPE2);\
TYPE  FUNC  (objectid, TYPE1, TYPE2);\
TYPE  FUNC  (HString, TYPE1, TYPE2);

#define DEF_DATA_OID_NAME_FUNC_DEF_3( TYPE, FUNC, TYPE1, TYPE2, TYPE3) \
TYPE  FUNC##_3_  (TYPE1, TYPE2, TYPE3);\
TYPE  FUNC##_3_  (objectid, TYPE1, TYPE2, TYPE3);\
TYPE  FUNC##_3_  (HString, TYPE1, TYPE2, TYPE3);
#define DEF_DATA_OID_NAME_FUNC_DEF_3b( TYPE, FUNC, TYPE1, TYPE2, TYPE3) \
TYPE  FUNC  (TYPE1, TYPE2, TYPE3);\
TYPE  FUNC  (objectid, TYPE1, TYPE2, TYPE3);\
TYPE  FUNC  (HString, TYPE1, TYPE2, TYPE3);

//------------------------------------------------------------------------

#define DEF_DATA_OID_NAME_FUNC_DEF_0_1( TYPE, FUNC, TYPE1 ) \
DEF_DATA_OID_NAME_FUNC_DEF_0( TYPE, FUNC) \
DEF_DATA_OID_NAME_FUNC_DEF_1b( TYPE, FUNC, TYPE1 )

#define DEF_DATA_OID_NAME_FUNC_0_1( DTYPE, DFUNC, TYPE1, PAR1) \
DEF_DATA_OID_NAME_FUNC_0( DTYPE, DFUNC) \
DEF_DATA_OID_NAME_FUNC_1b( DTYPE, DFUNC, TYPE1, PAR1) 

#define DEF_DATA_OID_NAME_FUNC_DEF( TYPE, FUNC ) DEF_DATA_OID_NAME_FUNC_DEF_0b( TYPE, FUNC)
#define DEF_DATA_OID_NAME_FUNC( DTYPE, DFUNC ) DEF_DATA_OID_NAME_FUNC_0b( DTYPE, DFUNC) 

//=======================================================================================================================
//=======================================================================================================================


#define DEF_DATA_FUNC_OF_DPTR_3PARS( FTYPE, FUNC, TYPE1, PAR1, TYPE2, PAR2, TYPE3, PAR3) \
FTYPE Data :: FUNC##_Name  (HString x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return FUNC(Ptr(x),PAR1,PAR2,PAR3);}\
FTYPE Data :: FUNC##_ID  (objectid x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return FUNC(Ptr(x),PAR1,PAR2,PAR3);}\
FTYPE Data :: FUNC##_Ref  (Data &x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return FUNC(&x,PAR1,PAR2,PAR3);}\
FTYPE Data :: FUNC  (HString x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return FUNC(Ptr(x),PAR1,PAR2,PAR3);}\
FTYPE Data :: FUNC  (objectid x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return FUNC(Ptr(x),PAR1,PAR2,PAR3);}\
FTYPE Data :: FUNC  (Data &x, TYPE1 PAR1, TYPE2 PAR2, TYPE3 PAR3){return FUNC(&x,PAR1,PAR2,PAR3);}\

#define DEF_DATA_FUNC_DEF_OF_DPTR_3PARS( FTYPE, FUNC, TYPE1, TYPE2, TYPE3) \
FTYPE FUNC##_Name  (HString  , TYPE1, TYPE2, TYPE3);\
FTYPE FUNC##_ID  (objectid, TYPE1, TYPE2, TYPE3);\
FTYPE FUNC##_Ref  (Data&   , TYPE1, TYPE2, TYPE3);\
FTYPE FUNC  (HString  , TYPE1, TYPE2, TYPE3);\
FTYPE FUNC  (objectid, TYPE1, TYPE2, TYPE3);\
FTYPE FUNC  (Data&   , TYPE1, TYPE2, TYPE3);

