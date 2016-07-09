
check_PROGRAMS = \
	tunnel_setup \
	disable_port

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

tunnel_setup_SOURCES   = test_tunnel_setup.cpp
tunnel_setup_CPPFLAGS  = $(common_cppflags)
tunnel_setup_CFLAGS    = $(common_cflags)
tunnel_setup_CXXFLAGS  = $(common_cxxflags)
tunnel_setup_LDFLAGS   = $(common_ldflags)

disable_port_SOURCES   = test_disable_port.cpp
disable_port_CPPFLAGS  = $(common_cppflags)
disable_port_CFLAGS    = $(common_cflags)
disable_port_CXXFLAGS  = $(common_cxxflags)
disable_port_LDFLAGS   = $(common_ldflags)

TESTS = \
	tunnel_setup.sh \
	disable_port.sh

