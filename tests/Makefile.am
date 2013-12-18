
check_PROGRAMS = init_deinit_test enum_components_test

if USE_BELLAGIO
  test_LDADD    = -lomxil-bellagio
  test_CPPFLAGS = \
  	-I/usr/include \
  	-DUSE_BELLAGIO
  test_CFLAGS   = 
else
  test_LDADD    = $(top_builddir)/src/libomxil-study.la
  test_CPPFLAGS = \
  	-I$(top_srcdir)/include \
  	-DUSE_STUDY
  test_CFLAGS   = 
endif

#common_LDADD  = ../common/libtestcommon.la
#common_CFLAGS = -I$(srcdir)/../common

init_deinit_test_SOURCES   = test_init_deinit.c
init_deinit_test_LDADD     = $(test_LDADD) $(common_LDADD) -lpthread
init_deinit_test_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
init_deinit_test_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

enum_components_test_SOURCES   = test_enum_components.c
enum_components_test_LDADD     = $(test_LDADD) $(common_LDADD)
enum_components_test_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
enum_components_test_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

TESTS = $(check_PROGRAMS)

