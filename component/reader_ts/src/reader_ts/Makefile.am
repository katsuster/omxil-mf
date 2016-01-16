
noinst_LTLIBRARIES = libreader_ts.la

libreader_ts_la_SOURCES = \
	reader_ts.cpp

EXTRA_libreader_ts_la_SOURCES = \
	reader_ts.hpp

libreader_ts_la_CPPFLAGS = $(omxil_mf_reader_ts_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libreader_ts_la_CFLAGS   = $(omxil_mf_reader_ts_common_cflags)
libreader_ts_la_CXXFLAGS = $(omxil_mf_reader_ts_common_cxxflags)
libreader_ts_la_LDFLAGS  = $(omxil_mf_reader_ts_common_ldflags)

if USE_INNER_LIB
  libreader_ts_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  libreader_ts_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
