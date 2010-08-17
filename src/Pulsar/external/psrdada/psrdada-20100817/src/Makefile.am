
lib_LTLIBRARIES = libpsrdada.la

psrdada_hdr=ascii_header.h command_parse.h command_parse_server.h \
	    dada_client.h dada_def.h dada_generator.h dada_hdu.h  \
	    dada_msg.h dada_ni.h dada_pwc.h dada_pwc_main.h       \
	    dada_pwc_main_multi.h                                 \
	    dada_pwc_nexus.h dada_udp.h daemon.h diff_time.h      \
	    disk_array.h futils.h ipcbuf.h ipcio.h ipcutil.h      \
	    median_smooth_zap.h monitor.h multilog.h nexus.h      \
	    node_array.h sock.h string_array.h tmutil.h udp.h

libpsrdada_la_SOURCES = $(psrdada_hdr) ascii_header.c multilog.c sock.c monitor.c		\
	ipcutil.c ipcbuf.c ipcio.c disk_array.c node_array.c		\
	command_parse.c command_parse_server.c string_array.c		\
	diff_time.c nexus.c filesize.c fileread.c tmutil.c daemon.c	\
	dada_hdu.c dada_pwc.c dada_pwc_nexus.c dada_ni.c		\
	dada_pwc_main.c dada_pwc_main_multi.c dada_client.c					\
	dada_pwc_nexus_config.c dada_pwc_nexus_header_parse.c		\
	multilog_server.c dada_udp.c dada_generator.c

include_HEADERS = ascii_header.h multilog.h sock.h monitor.h	  \
	ipcutil.h ipcbuf.h ipcio.h disk_array.h node_array.h	  \
	command_parse.h command_parse_server.h string_array.h	  \
	diff_time.h nexus.h futils.h tmutil.h daemon.h dada_def.h \
	dada_hdu.h dada_pwc.h dada_pwc_nexus.h dada_ni.h	  \
	dada_pwc_main.h dada_pwc_main_multi.h dada_client.h udp.h \
	dada_udp.h dada_generator.h \
	dada_msg.h

bin_PROGRAMS = dada_db dada_write_test dada_dbdisk dada_diskdb	 \
	dada_dbnic dada_nicdb dada_dbNdb			 \
	dada_install_header dada_pwc_command dada_pwc_demo	 \
	dada_dbnull dada_dbmonitor dada_dbxferinfo dada_dbmetric \
	dada_header dada_edit slow_rm dada_junkdb dada_mem_test \
	daemon_test load_test

bin_SCRIPTS = ipcrme loopssh

check_PROGRAMS = test_multilog test_ipcbuf loop_ipcio	\
	test_command_parser test_multilog_server telnecho		\
	test_command_parse_server test_dada_nexus			\
	dada_writer dada_reader $(TESTS)

TESTS = test_uint64_t test_disk_array test_ipcio test_ascii_header

LDADD = libpsrdada.la 

EXTRA_DIST=ipcrme loopssh
