
BASE_DIR      = .
UTIL_DIR      = $(BASE_DIR)/util
RING_DIR      = $(BASE_DIR)/ring
COMPONENT_DIR = $(BASE_DIR)/component
DEBUG_DIR     = $(BASE_DIR)/debug

omxil_mfdir = $(libdir)
omxil_mf_LTLIBRARIES = libomxil-mf.la

libomxil_mf_ladir = $(includedir)

libomxil_mf_la_SOURCES = \
	$(BASE_DIR)/main.cpp \
	$(BASE_DIR)/entry.cpp \
	$(UTIL_DIR)/util.cpp \
	$(UTIL_DIR)/omx_types_enum_name.cpp \
	$(UTIL_DIR)/omx_index_enum_name.cpp \
	$(UTIL_DIR)/omx_core_enum_name.cpp \
	$(UTIL_DIR)/omx_component_enum_name.cpp \
	$(UTIL_DIR)/omx_video_enum_name.cpp \
	$(COMPONENT_DIR)/component.cpp \
	$(COMPONENT_DIR)/port.cpp \
	$(DEBUG_DIR)/dbgprint.c
EXTRA_libomxil_mf_la_SOURCES = \
	$(BASE_DIR)/consts.h \
	$(BASE_DIR)/main.hpp \
	$(BASE_DIR)/entry.hpp \
	$(UTIL_DIR)/util.hpp \
	$(UTIL_DIR)/omx_enum_name.hpp \
	$(RING_DIR)/bit_buffer.hpp \
	$(RING_DIR)/bounded_bit_buffer.hpp \
	$(RING_DIR)/bounded_buffer.hpp \
	$(RING_DIR)/buffer_base.hpp \
	$(RING_DIR)/ring_buffer.hpp \
	$(COMPONENT_DIR)/omx_reflector.hpp \
	$(COMPONENT_DIR)/component.hpp \
	$(COMPONENT_DIR)/port.hpp \
	$(DEBUG_DIR)/dbgprint.h \
	$(DEBUG_DIR)/scoped_log.hpp

libomxil_mf_la_HEADERS = \
	../include/OMX_Audio.h \
	../include/OMX_Component.h \
	../include/OMX_ContentPipe.h \
	../include/OMX_Core.h \
	../include/OMX_Image.h \
	../include/OMX_Index.h \
	../include/OMX_IVCommon.h \
	../include/OMX_Other.h \
	../include/OMX_Types.h \
	../include/OMX_Video.h \
	../include/omxil_mf.h

libomxil_mf_la_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-Wall -Werror
libomxil_mf_la_CFLAGS   =
libomxil_mf_la_CXXFLAGS = \
	-std=c++0x

libomxil_mf_la_LIBADD   = \
	-lpthread
libomxil_mf_la_LDFLAGS  =
