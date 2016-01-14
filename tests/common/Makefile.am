
noinst_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES = \
	omxil_utils.c \
	omxil_comp.cpp

EXTRA_libentry_la_SOURCES = \
	test_omxil.h \
	omxil_utils.h \
	omxil_comp.hpp

libcommon_la_LDADD    += -lomxil-bellagio

libcommon_la_CPPFLAGS = $(omxil_mf_common_cppflags) \
	-I$(top_srcdir)/tests
libcommon_la_CFLAGS   = $(omxil_mf_common_cflags)
libcommon_la_CXXFLAGS = $(omxil_mf_common_cxxflags)
libcommon_la_LDFLAGS  = $(omxil_mf_common_ldflags)

if USE_BELLAGIO
  libcommon_la_CPPFLAGS += -DUSE_BELLAGIO
  libcommon_la_LDFLAGS  += -lomxil-bellagio
else
  libcommon_la_LDADD    += $(top_builddir)/src/libomxil-mf.la
  libcommon_la_CPPFLAGS += -DUSE_MF -I$(top_srcdir)/include
endif
