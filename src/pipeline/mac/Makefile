LOFARROOT = /home/swinbank/Work/LOFAR
LOFARCONF = -I/home/swinbank/include

MACSRC    = $(LOFARROOT)/MAC/MACIO/src/EventPort.cc $(LOFARROOT)/MAC/MACIO/src/GCF_Event.cc
MACINC    = -I$(LOFARROOT)/MAC/MACIO/include

COMMONSRC = $(LOFARROOT)/LCS/Common/src/SymbolTable.cc $(LOFARROOT)/LCS/Common/src/AddressTranslator.cc $(LOFARROOT)/LCS/Common/src/hexdump.cc $(LOFARROOT)/LCS/Common/src/SystemUtil.cc $(LOFARROOT)/LCS/Common/src/StringUtil.cc $(LOFARROOT)/LCS/Common/src/Backtrace.cc $(LOFARROOT)/LCS/Common/src/Net/Socket.cc $(LOFARROOT)/LCS/Common/src/LofarLogger.cc
COMMONINC = -I$(LOFARROOT)/LCS/Common/include

LOG4CPLUS = -I/data/sys/opt/LofIm/external/log4cplus/builds/log4cplus-1.0.2-3/include -L/data/sys/opt/LofIm/external/log4cplus/builds/log4cplus-1.0.2-3/lib/ -llog4cplus
BPYTHON   = -I/usr/include/python2.5 -lboost_python -lpython2.5

ep_echo: echo_protocol sb_protocol
	g++ -fPIC -shared -o ep_echo.so ep_echo.cc Echo_Protocol.cc SB_Protocol.cc -I. $(MACSRC) $(COMMONSRC) $(LOFARCONF) $(COMMONINC) $(MACINC) $(LOG4CPLUS) $(BPYTHON) -lbfd

echo_protocol:
	autogen -L $(LOFARROOT)/MAC/MACIO/autogen/ $(LOFARROOT)/MAC/MACIO/test/Echo_Protocol.prot
	
sb_protocol:
	autogen -L $(LOFARROOT)/MAC/MACIO/autogen/ $(LOFARROOT)/MAC/MACIO/src/SB_Protocol.prot
	
clean:
	\rm -f Echo_Protocol.cc SB_Protocol.cc Echo_Protocol.ph SB_Protocol.ph ep_echo.so
