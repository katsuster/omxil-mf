
#SUBDIRS = cppunit-1.12.1
SUBDIRS = api util component regist debug

omxil_mfdir = $(libdir)
omxil_mf_LTLIBRARIES = libomxil-mf.la

libomxil_mf_ladir = $(includedir)

libomxil_mf_la_SOURCES = 

EXTRA_libomxil_mf_la_SOURCES = 

libomxil_mf_la_LIBADD  = \
	api/libapi.la \
	util/libutil.la \
	component/libcomponent.la \
	regist/libregist.la \
	debug/libdebug.la

libomxil_mf_la_CPPFLAGS = $(omxil_mf_common_cppflags) \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/src
libomxil_mf_la_CFLAGS   = $(omxil_mf_common_cflags)
libomxil_mf_la_CXXFLAGS = $(omxil_mf_common_cxxflags)
libomxil_mf_la_LDFLAGS  = $(omxil_mf_common_ldflags) \
	-L$(top_srcdir)/src \
	-lpthread \
	-ldl
