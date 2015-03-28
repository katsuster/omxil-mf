
BASE_DIR        = .
ENTRY_DIR       = $(BASE_DIR)/entry
READER_ZERO_DIR = $(BASE_DIR)/reader_zero
RENDER_NULL_DIR = $(BASE_DIR)/render_null

omxil_mf_emptydir = $(libdir)
omxil_mf_empty_LTLIBRARIES = libomxil-mf-empty.la

libomxil_mf_empty_ladir = $(includedir)

libomxil_mf_empty_la_SOURCES = \
	$(ENTRY_DIR)/entry.cpp \
	$(RENDER_NULL_DIR)/render_null.cpp

EXTRA_libomxil_mf_empty_la_SOURCES = \
	$(ENTRY_DIR)/entry.h \
	$(RENDER_NULL_DIR)/render_null.hpp

libomxil_mf_empty_la_HEADERS = 

libomxil_mf_empty_la_CPPFLAGS = \
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
