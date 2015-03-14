
omxil_mfdir = $(libdir)

omxil_mf_LTLIBRARIES = libomxil-mf.la


libomxil_mf_ladir = $(includedir)

libomxil_mf_la_SOURCES = \
	entry.cpp \
	main.cpp \
	util/omx_core_enum_name.cpp \
	debug/dbgprint.c
EXTRA_libomxil_mf_la_SOURCES = \
	main.hpp \
	util/omx_enum_name.hpp \
	debug/dbgprint.h

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


