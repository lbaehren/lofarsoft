# If your system is BSD based, please define BSD specifically.
#
# If you are running Solaris, 
# you need to define _NO_PROTO when you compile them
# you need to link your object files with -lsocket and -lnsl to make executable.


CFLAGS = -g -D_NO_PROTO

simpos: simpos.o ../libwcs/libwcs.a
	cc -o simpos simpos.o ../libwcs/libwcs.a -lsocket  -lnsl -lm

