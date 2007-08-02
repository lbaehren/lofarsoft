### $Id: Makefile 3605 2006-05-29 15:02:45Z sarod $
CC=gcc
CXX=g++
CFLAGS= -Wall -O2 -g
CLIBS= -lm -lcfitsio  
LIBPATH= -L/aips++/cfitsio/lib 
INCLUDES= -I. -I/aips++/cfitsio/include
WCSI= -I/aips++/sys1/code/casa/wcslib
WCSL= -L/aips++/sys1/linux_gnu/lib
WCSLIBS= -lwcs


convert:convert.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $@.c
	$(CC) $(CFLAGS) $(INCLUDES) $(CLIBS) $(LIBPATH) -o $@ $@.o
modlist:modlist.cc
	$(CXX) $(CFLAGS) $(INCLUDES) $(WCSI) -c $@.cc
	$(CXX) $(CFLAGS) $(INCLUDES) $(WCSI) $(CLIBS) $(WCSLIBS) $(WCSL) $(LIBPATH) -o $@ $@.o
