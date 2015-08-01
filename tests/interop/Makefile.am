TEST_DIR   = $(top_srcdir)/tests
COMMON_DIR = $(TEST_DIR)/common

check_PROGRAMS = \
	tunnel_setup

common_SOURCES  = $(COMMON_DIR)/omxil_utils.c $(COMMON_DIR)/omxil_comp.cpp
common_CPPFLAGS = -Wall -Werror \
	-I$(TEST_DIR)
common_CFLAGS   = 
common_CXXFLAGS = 
common_LDADD    = -lpthread

if USE_BELLAGIO
  common_CPPFLAGS += -DUSE_BELLAGIO
  common_CFLAGS   += 
  common_CXXFLAGS += -std=c++0x
  common_LDADD    += -lomxil-bellagio
else
  common_CPPFLAGS += -DUSE_MF -I$(top_srcdir)/include
  common_CFLAGS   += 
  common_CXXFLAGS += -std=c++0x
  common_LDADD    += $(top_builddir)/src/libomxil-mf.la
endif

tunnel_setup_SOURCES   = test_tunnel_setup.cpp $(common_SOURCES)
tunnel_setup_LDADD     = $(common_LDADD)
tunnel_setup_CPPFLAGS  = $(common_CPPFLAGS)
tunnel_setup_CFLAGS    = $(common_CFLAGS)
tunnel_setup_CXXFLAGS  = $(common_CXXFLAGS)

TESTS = \
	tunnel_setup.sh

