
noinst_LTLIBRARIES = librenderer_null.la

librenderer_null_la_SOURCES = \
	renderer_null.cpp

EXTRA_librenderer_null_la_SOURCES = \
	renderer_null.hpp

librenderer_null_la_CPPFLAGS = $(omxil_mf_empty_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
librenderer_null_la_CFLAGS   = $(omxil_mf_empty_common_cflags)
librenderer_null_la_CXXFLAGS = $(omxil_mf_empty_common_cxxflags)
librenderer_null_la_LDFLAGS  = $(omxil_mf_empty_common_ldflags)

if USE_INNER_LIB
  librenderer_null_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  librenderer_null_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
