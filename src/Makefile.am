
omxil_studydir = $(libdir)

omxil_study_LTLIBRARIES = libomxil-study.la


libomxil_study_ladir = $(includedir)

libomxil_study_la_SOURCES = \
	entry.cpp \
	main.cpp \
	debug/dbgprint.c

libomxil_study_la_HEADERS = \
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
	../include/omxilstudy.h

libomxil_study_la_CPPFLAGS  = \
	-I$(top_srcdir)/include \
	-Wall -Werror

libomxil_study_la_CFLAGS  = 
libomxil_study_la_CXXFLAGS  = 

libomxil_study_la_LIBADD  = \
	-lpthread

libomxil_study_la_LDFLAGS = 


