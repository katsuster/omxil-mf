
noinst_LTLIBRARIES = libapi.la

libapi_la_SOURCES = \
	omxil.cpp \
	omxil_mf.cpp \
	windll.cpp

EXTRA_libapi_la_SOURCES = \
	consts.hpp

libapi_la_CPPFLAGS = $(omxil_mf_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libapi_la_CFLAGS   = $(omxil_mf_common_cflags)
libapi_la_CXXFLAGS = $(omxil_mf_common_cxxflags)
libapi_la_LDFLAGS  = $(omxil_mf_common_ldflags)
