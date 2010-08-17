
lib_LTLIBRARIES = libpic.la

libpic_la_SOURCES = pic.h puma2_dmadb.h pic.c

bin_PROGRAMS = puma2_dmadb

check_PROGRAMS = testpic

INCLUDES = -I../src -I/opt/pic -I/opt/EDTpcd

LDADD = ../src/libdada.la libpic.la -L/opt/EDTpcd -ledt

