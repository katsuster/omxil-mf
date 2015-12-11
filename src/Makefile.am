
BASE_DIR      = .
HEADER_DIR    = $(BASE_DIR)/../include
MF_HEADER_DIR = $(BASE_DIR)/../include/omxil_mf
API_DIR       = $(BASE_DIR)/api
UTIL_DIR      = $(BASE_DIR)/util
RING_DIR      = $(BASE_DIR)/ring
COMPONENT_DIR = $(BASE_DIR)/component
REGIST_DIR    = $(BASE_DIR)/regist
DEBUG_DIR     = $(BASE_DIR)/debug

omxil_mfdir = $(libdir)
omxil_mf_LTLIBRARIES = libomxil-mf.la

libomxil_mf_ladir = $(includedir)

libomxil_mf_la_SOURCES = \
	$(API_DIR)/omxil.cpp \
	$(API_DIR)/omxil_mf.cpp \
	$(UTIL_DIR)/util.cpp \
	$(UTIL_DIR)/omx_types_enum_name.cpp \
	$(UTIL_DIR)/omx_index_enum_name.cpp \
	$(UTIL_DIR)/omx_core_enum_name.cpp \
	$(UTIL_DIR)/omx_component_enum_name.cpp \
	$(UTIL_DIR)/omx_ivcommon_enum_name.cpp \
	$(UTIL_DIR)/omx_audio_enum_name.cpp \
	$(UTIL_DIR)/omx_video_enum_name.cpp \
	$(UTIL_DIR)/omx_image_enum_name.cpp \
	$(UTIL_DIR)/omx_other_enum_name.cpp \
	$(COMPONENT_DIR)/omx_reflector.cpp \
	$(COMPONENT_DIR)/component.cpp \
	$(COMPONENT_DIR)/component_worker.cpp \
	$(COMPONENT_DIR)/port.cpp \
	$(COMPONENT_DIR)/port_audio.cpp \
	$(COMPONENT_DIR)/port_video.cpp \
	$(COMPONENT_DIR)/port_image.cpp \
	$(COMPONENT_DIR)/port_other.cpp \
	$(COMPONENT_DIR)/port_buffer.cpp \
	$(COMPONENT_DIR)/port_format.cpp \
	$(REGIST_DIR)/register_component.cpp \
	$(DEBUG_DIR)/dprint.cpp
EXTRA_libomxil_mf_la_SOURCES = \
	$(API_DIR)/consts.hpp \
	$(UTIL_DIR)/util.hpp \
	$(UTIL_DIR)/omx_enum_name.hpp \
	$(REGIST_DIR)/register_component.hpp

libomxil_mf_la_CPPFLAGS = \
	$(AM_CPPFLAGS) \
	-I$(top_srcdir)/include \
	-Wall -Werror
libomxil_mf_la_CFLAGS   = \
	$(AM_CFLAGS)
libomxil_mf_la_CXXFLAGS = \
	$(AM_CXXFLAGS) \
	-std=c++0x

libomxil_mf_la_LIBADD   = \
	-lpthread \
	-ldl
libomxil_mf_la_LDFLAGS  = \
	$(AM_LDFLAGS)

