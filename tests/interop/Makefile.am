
check_PROGRAMS = \
	tunnel_setup

common_ldadd    = $(top_srcdir)/common/libcommon.la

common_cppflags = $(omxil_mf_common_cppflags) \
	-I$(top_srcdir)/tests
common_cflags   = $(omxil_mf_common_cflags)
common_cxxflags = $(omxil_mf_common_cxxflags)
common_ldflags  = $(omxil_mf_common_ldflags)

if USE_BELLAGIO
  common_cppflags += -DUSE_BELLAGIO
  common_ldflags  += -lomxil-bellagio
else
  common_ldadd    += $(top_builddir)/src/libomxil-mf.la
  common_cppflags += -DUSE_MF -I$(top_srcdir)/include
endif

tunnel_setup_SOURCES   = test_tunnel_setup.cpp
tunnel_setup_LDADD     = $(common_ldadd)
tunnel_setup_CPPFLAGS  = $(common_cppflags)
tunnel_setup_CFLAGS    = $(common_cflags)
tunnel_setup_CXXFLAGS  = $(common_cxxflags)
tunnel_setup_LDFLAGS   = $(common_ldflags)

TESTS = \
	tunnel_setup.sh

