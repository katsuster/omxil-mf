
noinst_LTLIBRARIES = libfilter_copy.la

libfilter_copy_la_SOURCES = \
	filter_copy.cpp

EXTRA_libfilter_copy_la_SOURCES = \
	filter_copy.hpp

libfilter_copy_la_CPPFLAGS = $(omxil_mf_empty_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libfilter_copy_la_CFLAGS   = $(omxil_mf_empty_common_cflags)
libfilter_copy_la_CXXFLAGS = $(omxil_mf_empty_common_cxxflags)
libfilter_copy_la_LDFLAGS  = $(omxil_mf_empty_common_ldflags)

if USE_INNER_LIB
  libfilter_copy_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  libfilter_copy_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
