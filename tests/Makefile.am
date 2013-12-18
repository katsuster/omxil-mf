
check_PROGRAMS = init_deinit_test

study_LDADD = $(top_builddir)/src/libomxil-study.la
study_CFLAGS = -I$(top_srcdir)/include
#common_LDADD = ../common/libtestcommon.la
#common_CFLAGS = -I$(srcdir)/../common

init_deinit_test_SOURCES = test_init_deinit.c
init_deinit_test_LDADD   = $(study_LDADD) $(common_LDADD) -lpthread
init_deinit_test_CFLAGS  = $(study_CFLAGS) $(common_CFLAGS)


