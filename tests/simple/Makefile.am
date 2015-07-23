TEST_DIR   = $(top_srcdir)/tests
COMMON_DIR = $(TEST_DIR)/common

check_PROGRAMS = \
	renderer_null

common_SOURCES  = $(COMMON_DIR)/omxil_utils.c $(COMMON_DIR)/omxil_comp.cpp
common_CPPFLAGS = -Wall -Werror \
	-I$(TEST_DIR)
common_CFLAGS   = 
common_CXXFLAGS = 
common_LDADD    = 

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

renderer_null_SOURCES   = test_renderer_null.cpp $(common_SOURCES)
renderer_null_LDADD     = $(common_LDADD)
renderer_null_CPPFLAGS  = $(common_CPPFLAGS)
renderer_null_CFLAGS    = $(common_CFLAGS)
renderer_null_CXXFLAGS  = $(common_CXXFLAGS)

TESTS = \
	renderer_null

