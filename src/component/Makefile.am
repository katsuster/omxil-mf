
noinst_LTLIBRARIES = libcomponent.la

libcomponent_la_SOURCES = \
	omx_reflector.cpp \
	component.cpp \
	component_worker.cpp \
	port.cpp \
	port_audio.cpp \
	port_video.cpp \
	port_image.cpp \
	port_other.cpp \
	port_buffer.cpp \
	port_format.cpp

EXTRA_libcomponent_la_SOURCES = 

libcomponent_la_CPPFLAGS = $(omxil_mf_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libcomponent_la_CFLAGS   = $(omxil_mf_common_cflags)
libcomponent_la_CXXFLAGS = $(omxil_mf_common_cxxflags)
libcomponent_la_LDFLAGS  = $(omxil_mf_common_ldflags)
