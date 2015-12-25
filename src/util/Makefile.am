
noinst_LTLIBRARIES = libutil.la

libutil_la_SOURCES = \
	util.cpp \
	omx_types_enum_name.cpp \
	omx_index_enum_name.cpp \
	omx_core_enum_name.cpp \
	omx_component_enum_name.cpp \
	omx_ivcommon_enum_name.cpp \
	omx_audio_enum_name.cpp \
	omx_video_enum_name.cpp \
	omx_image_enum_name.cpp \
	omx_other_enum_name.cpp

EXTRA_libutil_la_SOURCES = \
	util.hpp \
	omx_enum_name.hpp

libutil_la_CPPFLAGS = $(omxil_mf_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libutil_la_CFLAGS   = $(omxil_mf_common_cflags)
libutil_la_CXXFLAGS = $(omxil_mf_common_cxxflags)
