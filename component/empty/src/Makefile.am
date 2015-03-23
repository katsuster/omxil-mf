
BASE_DIR      = .

omxil_mf_emptydir = $(libdir)
omxil_mf_empty_LTLIBRARIES = libomxil-mf-empty.la

libomxil_mf_empty_ladir = $(includedir)

libomxil_mf_empty_la_SOURCES = \
	$(BASE_DIR)/entry.cpp

EXTRA_libomxil_mf_empty_la_SOURCES = \
	$(BASE_DIR)/entry.hpp

libomxil_mf_empty_la_HEADERS = 

libomxil_mf_empty_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-Wall -Werror
libomxil_mf_empty_la_CFLAGS   =
libomxil_mf_empty_la_CXXFLAGS = \
	-std=c++0x

libomxil_mf_empty_la_LIBADD   = \
	-lpthread \
	-lomxil-mf
libomxil_mf_empty_la_LDFLAGS  =

if USE_INNER_LIB
  libomxil_mf_empty_la_CPPFLAGS += \
  	-I../../../include
  libomxil_mf_empty_la_LDFLAGS += \
  	-L../../../src/.libs
endif
