
noinst_LTLIBRARIES = libentry.la

libentry_la_SOURCES = \
	entry.cpp \
	windll.cpp

EXTRA_libentry_la_SOURCES = \
	entry.h

libentry_la_CPPFLAGS = $(omxil_mf_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libentry_la_CFLAGS   = $(omxil_mf_common_cflags)
libentry_la_CXXFLAGS = $(omxil_mf_common_cxxflags)
libentry_la_LDFLAGS  = $(omxil_mf_common_ldflags)

if USE_INNER_LIB
  libentry_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  libentry_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
