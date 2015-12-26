EXTRA_DIST = DllPlugInTester.dsp DllPlugInTesterTest.dsp


bin_PROGRAMS=DllPlugInTester

TESTS = DllPlugInTesterTest
check_PROGRAMS = $(TESTS)

DllPlugInTester_SOURCES= DllPlugInTester.cpp \
	CommandLineParser.h \
	CommandLineParser.cpp

DllPlugInTester_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include

DllPlugInTester_LDADD= \
  $(top_builddir)/src/cppunit/libcppunit.la

DllPlugInTester_LDFLAGS = $(LIBADD_DL)

DllPlugInTesterTest_SOURCES = DllPlugInTesterTest.cpp \
	CommandLineParser.cpp \
	CommandLineParser.h \
	CommandLineParserTest.cpp \
	CommandLineParserTest.h

DllPlugInTesterTest_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include

DllPlugInTesterTest_LDADD= \
  $(top_builddir)/src/cppunit/libcppunit.la

DllPlugInTesterTest_LDFLAGS = $(LIBADD_DL)
