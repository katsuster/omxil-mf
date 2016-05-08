
SUBDIRS = entry reader_binary writer_binary

omxil_mf_simpledir = $(libdir)
omxil_mf_simple_LTLIBRARIES = libomxil-mf-simple.la

libomxil_mf_simple_ladir = $(includedir)

libomxil_mf_simple_la_HEADERS = 

libomxil_mf_simple_la_SOURCES = 

EXTRA_libomxil_mf_simple_la_SOURCES = 

libomxil_mf_simple_la_LIBADD  = \
	entry/libentry.la \
	reader_binary/libreader_binary.la \
	writer_binary/libwriter_binary.la

libomxil_mf_simple_la_CPPFLAGS = $(omxil_mf_simple_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libomxil_mf_simple_la_CFLAGS   = $(omxil_mf_simple_common_cflags)
libomxil_mf_simple_la_CXXFLAGS = $(omxil_mf_simple_common_cxxflags)
libomxil_mf_simple_la_LDFLAGS  = $(omxil_mf_simple_common_ldflags) \
	-lomxil-mf \
	-lpthread \
	-lstdc++

if USE_INNER_LIB
  libomxil_mf_simple_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  libomxil_mf_simple_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
