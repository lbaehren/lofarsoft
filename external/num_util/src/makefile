CXXFLAGS=  -Wall -ftemplate-depth-100 -DBOOST_PYTHON_DYNAMIC_LIB -g -O0 -fno-inline -fPIC

INCLUDES= -I "/home/phil/usr243_Fc3/include/python2.4" \
         -I  "/home/phil/usr243_Fc3/lib/python2.4/site-packages/numpy/core/include" \
         -I "/nfs/kite/users/phil/include/boost_1_33_1" \
         -I  "."

LDFLAGS= -fPIC -Wl,-rpath,/home/phil/lib/boost_lib_1_33_1/debug  -shared

LIBDIRS= -L/home/phil/usr243_Fc3/lib/python2.4/config \
         -L/home/phil/lib/boost_lib_1_33_1/debug

PYLIBS= -lboost_python

PYTHON_DIR=/home/phil/usr243_Fc3

all: num_py_test.py simpletest.py

num_py_test.py : num_test_ext.so
	$(PYTHON_DIR)/bin/python num_py_test.py

simpletest.py : simple_ext.so
	$(PYTHON_DIR)/bin/python simpletest.py

num_test_ext.so: num_util.o num_test.o num_test_ext.o

num_test_ext.o : num_test_ext.cpp num_test.h

simple_ext.so : num_util.o simpletest.o
	g++ -$(LDFLAGS) -o $@  $(LIBDIRS)  $(PYLIBS)  $^

num_test.o  : num_test.cpp num_util.o num_test.h

num_util.o  : num_util.cpp  num_util.h

simpletest.o  : num_util.o num_util.h simpletest.cpp


clean:
	rm -f *.o *.so

%.o : %.cpp
	g++ -c $(CXXFLAGS) $(INCLUDES) -o $@ $<

%.so : %.o
	g++ -$(LDFLAGS) -o $@  $(LIBDIRS)  $(PYLIBS)  $^
