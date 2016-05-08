
noinst_LTLIBRARIES = libreader_binary.la

libreader_binary_la_SOURCES = \
	reader_binary.cpp \
	audio_reader_binary.cpp \
	video_reader_binary.cpp

EXTRA_libreader_binary_la_SOURCES = \
	reader_binary.hpp \
	audio_reader_binary.hpp \
	video_reader_binary.hpp

libreader_binary_la_CPPFLAGS = $(omxil_mf_simple_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libreader_binary_la_CFLAGS   = $(omxil_mf_simple_common_cflags)
libreader_binary_la_CXXFLAGS = $(omxil_mf_simple_common_cxxflags)
libreader_binary_la_LDFLAGS  = $(omxil_mf_simple_common_ldflags)

if USE_INNER_LIB
  libreader_binary_la_CPPFLAGS += \
  	-I$(top_srcdir)/../../include
  libreader_binary_la_LDFLAGS  += \
  	-L$(top_srcdir)/../../src/.libs
endif
