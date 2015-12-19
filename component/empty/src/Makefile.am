
BASE_DIR          = .
ENTRY_DIR         = $(BASE_DIR)/entry
READER_ZERO_DIR   = $(BASE_DIR)/reader_zero
RENDERER_NULL_DIR = $(BASE_DIR)/renderer_null
FILTER_COPY_DIR   = $(BASE_DIR)/filter_copy

omxil_mf_emptydir = $(libdir)
omxil_mf_empty_LTLIBRARIES = libomxil-mf-empty.la

libomxil_mf_empty_ladir = $(includedir)

libomxil_mf_empty_la_SOURCES = \
	$(ENTRY_DIR)/entry.cpp \
	$(ENTRY_DIR)/windll.cpp \
	$(READER_ZERO_DIR)/reader_zero.cpp \
	$(RENDERER_NULL_DIR)/renderer_null.cpp \
	$(FILTER_COPY_DIR)/filter_copy.cpp

EXTRA_libomxil_mf_empty_la_SOURCES = \
	$(ENTRY_DIR)/entry.h \
	$(READER_ZERO_DIR)/reader_zero.hpp \
	$(RENDERER_NULL_DIR)/renderer_null.hpp \
	$(FILTER_COPY_DIR)/filter_copy.hpp

libomxil_mf_empty_la_HEADERS = 

libomxil_mf_empty_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-Wall -Werror
libomxil_mf_empty_la_CFLAGS   = \
	$(AM_CFLAGS)
libomxil_mf_empty_la_CXXFLAGS = \
	$(AM_CXXFLAGS) \
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
