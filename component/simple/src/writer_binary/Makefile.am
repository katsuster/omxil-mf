
noinst_LTLIBRARIES = libwriter_binary.la

libwriter_binary_la_SOURCES = \
	writer_binary.cpp \
	audio_writer_binary.cpp \
	video_writer_binary.cpp

EXTRA_libwriter_binary_la_SOURCES = \
	writer_binary.hpp \
	audio_writer_binary.hpp \
	video_writer_binary.hpp

libwriter_binary_la_CPPFLAGS = $(omxil_mf_simple_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libwriter_binary_la_CFLAGS   = $(omxil_mf_simple_common_cflags)
libwriter_binary_la_CXXFLAGS = $(omxil_mf_simple_common_cxxflags)
libwriter_binary_la_LDFLAGS  = $(omxil_mf_simple_common_ldflags)

if USE_INNER_LIB
  libwriter_binary_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  libwriter_binary_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
