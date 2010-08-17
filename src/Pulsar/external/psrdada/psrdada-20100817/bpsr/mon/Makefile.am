
lib_LTLIBRARIES = libplot4mon.la

libplot4mon_la_SOURCES = plot4mon.h get_commandline.c check_file_exists.c display_help.c \
        compute_extremes.c compute_margin.c strings_compare.c \
        read_params.c read_stream.c create_xaxis.c set_array_dim.c \
        create_outputname.c create_labels.c four1.c realft.c do_powerspec.c \
        work_on_data.c whiten.c selectmed.c plot_stream_1D.c plot_stream_2D.c \
        set_dimensions.c do_mmm.c

bin_PROGRAMS = plot4mon
AM_CPPFLAGS = @PGPLOT_CFLAGS@
LDADD = libplot4mon.la @PGPLOT_LIBS@

