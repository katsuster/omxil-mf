
SUBDIRS = entry reader_ts

omxil_mf_reader_tsdir = $(libdir)
omxil_mf_reader_ts_LTLIBRARIES = libomxil-mf-reader_ts.la

libomxil_mf_reader_ts_ladir = $(includedir)

libomxil_mf_reader_ts_la_HEADERS = 

libomxil_mf_reader_ts_la_SOURCES = 

EXTRA_libomxil_mf_reader_ts_la_SOURCES = 

libomxil_mf_reader_ts_la_LIBADD  = \
	entry/libentry.la \
	reader_ts/libreader_ts.la

libomxil_mf_reader_ts_la_CPPFLAGS = $(omxil_mf_reader_ts_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libomxil_mf_reader_ts_la_CFLAGS   = $(omxil_mf_reader_ts_common_cflags)
libomxil_mf_reader_ts_la_CXXFLAGS = $(omxil_mf_reader_ts_common_cxxflags)
libomxil_mf_reader_ts_la_LDFLAGS  = $(omxil_mf_reader_ts_common_ldflags) \
	-lomxil-mf \
	-lpthread \
	-lstdc++

if USE_INNER_LIB
  libomxil_mf_reader_ts_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  libomxil_mf_reader_ts_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
