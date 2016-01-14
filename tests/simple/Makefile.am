
check_PROGRAMS = \
	renderer_null

common_cppflags = $(omxil_mf_common_cppflags) \
	-I$(top_srcdir)/tests
common_cflags   = $(omxil_mf_common_cflags)
common_cxxflags = $(omxil_mf_common_cxxflags)
common_ldflags  = $(omxil_mf_common_ldflags) \
	../common/libcommon.la \
	-lstdc++

if USE_BELLAGIO
  common_cppflags += -DUSE_BELLAGIO
  common_ldflags  += -lomxil-bellagio
else
  common_cppflags += -DUSE_MF -I$(top_srcdir)/include
  common_ldflags  += $(top_builddir)/src/libomxil-mf.la
endif

renderer_null_SOURCES   = test_renderer_null.cpp
renderer_null_CPPFLAGS  = $(common_cppflags)
renderer_null_CFLAGS    = $(common_cflags)
renderer_null_CXXFLAGS  = $(common_cxxflags)
renderer_null_LDFLAGS   = $(common_ldflags)

TESTS = \
	renderer_null

