
noinst_LTLIBRARIES = libreader_zero.la

libreader_zero_la_SOURCES = \
	reader_zero.cpp

EXTRA_libreader_zero_la_SOURCES = \
	reader_zero.hpp

libreader_zero_la_CPPFLAGS = $(omxil_mf_empty_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libreader_zero_la_CFLAGS   = $(omxil_mf_empty_common_cflags)
libreader_zero_la_CXXFLAGS = $(omxil_mf_empty_common_cxxflags)
libreader_zero_la_LDFLAGS  = $(omxil_mf_empty_common_ldflags)

if USE_INNER_LIB
  libreader_zero_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  libreader_zero_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
