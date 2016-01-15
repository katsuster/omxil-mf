
SUBDIRS = entry reader_zero renderer_null filter_copy

omxil_mf_emptydir = $(libdir)
omxil_mf_empty_LTLIBRARIES = libomxil-mf-empty.la

libomxil_mf_empty_ladir = $(includedir)

libomxil_mf_empty_la_HEADERS = 

libomxil_mf_empty_la_SOURCES = 

EXTRA_libomxil_mf_empty_la_SOURCES = 

libomxil_mf_empty_la_LIBADD  = \
	entry/libentry.la \
	reader_zero/libreader_zero.la \
	renderer_null/librenderer_null.la \
	filter_copy/libfilter_copy.la

libomxil_mf_empty_la_CPPFLAGS = $(omxil_mf_empty_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libomxil_mf_empty_la_CFLAGS   = $(omxil_mf_empty_common_cflags)
libomxil_mf_empty_la_CXXFLAGS = $(omxil_mf_empty_common_cxxflags)
libomxil_mf_empty_la_LDFLAGS  = $(omxil_mf_empty_common_ldflags) \
	-lomxil-mf \
	-lpthread \
	-lstdc++

if USE_INNER_LIB
  libomxil_mf_empty_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  libomxil_mf_empty_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
