LOFARROOT = /home/swinbank/Work/LOFAR

MACPRTSRC = $(LOFARROOT)/MAC/MACIO/src/EventPort.cc
MACEVTSRC = $(LOFARROOT)/MAC/MACIO/src/GCF_Event.cc
MACINC    = -I$(LOFARROOT)/MAC/MACIO/include

COMMONSRC = $(LOFARROOT)/LCS/Common/src/SymbolTable.cc $(LOFARROOT)/LCS/Common/src/AddressTranslator.cc $(LOFARROOT)/LCS/Common/src/hexdump.cc $(LOFARROOT)/LCS/Common/src/SystemUtil.cc $(LOFARROOT)/LCS/Common/src/StringUtil.cc $(LOFARROOT)/LCS/Common/src/Backtrace.cc $(LOFARROOT)/LCS/Common/src/Net/Socket.cc $(LOFARROOT)/LCS/Common/src/LofarLogger.cc
COMMONINC = -I$(LOFARROOT)/LCS/Common/include

APLINC    = -I$(LOFARROOT)/MAC/APL/APLCommon/include
GCFINC    = -I$(LOFARROOT)/MAC/GCF/TM/include

LOG4CPLUS = -I/data/sys/opt/LofIm/external/log4cplus/builds/log4cplus-1.0.2-3/include -L/data/sys/opt/LofIm/external/log4cplus/builds/log4cplus-1.0.2-3/lib/ -llog4cplus
BPYTHON   = -I/usr/include/python2.5 -lboost_python -lpython2.5

OUTPUTDIR = ep

default: ep_interface ep_echo ep_control

ep_interface: sb_protocol
	g++ -fPIC -shared -o $(OUTPUTDIR)/interface/_ep_interface.so src/ep_interface.cc src/SB_Protocol.cc -I./include $(MACPRTSRC) $(MACEVTSRC) $(COMMONSRC) $(LOFARCONF) $(COMMONINC) $(MACINC) $(LOG4CPLUS) $(BPYTHON) -lbfd

ep_echo: echo_protocol ep_interface
	g++ -fPIC -shared -o $(OUTPUTDIR)/echo/_ep_echo.so src/ep_echo.cc src/eventwrappers.cc src/Echo_Protocol.cc -I./include $(MACEVTSRC) $(COMMONSRC) $(LOFARCONF) $(COMMONINC) $(MACINC) $(LOG4CPLUS) $(BPYTHON) -lbfd

ep_control: controller_protocol ep_interface
	g++ -fPIC -shared -o $(OUTPUTDIR)/control/_ep_control.so src/ep_control.cc src/Controller_Protocol.cc -I./include $(MACEVTSRC) $(COMMONSRC) $(LOFARCONF) $(COMMONINC) $(MACINC) $(APLINC) $(GCFINC) $(LOG4CPLUS) $(BPYTHON) -lbfd

sb_protocol:
	autogen -L $(LOFARROOT)/MAC/MACIO/autogen/ $(LOFARROOT)/MAC/MACIO/src/SB_Protocol.prot
	mv -f SB_Protocol.cc  src/
	mv -f SB_Protocol.ph include/
	
echo_protocol:
	autogen -L $(LOFARROOT)/MAC/MACIO/autogen/ $(LOFARROOT)/MAC/MACIO/test/Echo_Protocol.prot
	mv -f Echo_Protocol.cc  src/
	mv -f Echo_Protocol.ph include/

controller_protocol:
	autogen -L $(LOFARROOT)/MAC/MACIO/autogen/ $(LOFARROOT)/MAC/APL/APLCommon/src/Controller_Protocol.prot
	mv -f Controller_Protocol.cc  src/
	mv -f Controller_Protocol.ph include/

clean:
	\rm -f src/Echo_Protocol.cc src/SB_Protocol.cc src/Controller_Protocol.cc include/Echo_Protocol.ph include/Controller_Protocol.ph include/SB_Protocol.ph ep/interface/_ep_interface.so ep/echo/_ep_echo.so ep/control/_ep_control.so
