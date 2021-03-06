
AC_PREREQ(2.61)

dnl ----------------------------------------
dnl Information on this package.
dnl ----------------------------------------
AC_INIT([omxil-mf], [0.1])

AC_ARG_PROGRAM()

AC_CONFIG_SRCDIR([src/api/omxil.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([conf])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign ])
AM_SILENT_RULES([yes])
LT_INIT()

dnl ----------------------------------------
dnl Checks for programs.
dnl ----------------------------------------
AC_PROG_CC()
AC_PROG_CXX()
dnl AC_PROG_AWK()
dnl AC_PROG_INSTALL()
dnl AC_PROG_LN_S()
dnl AC_PROG_LIBTOOL()
dnl AC_PROG_SED()
dnl AM_PROG_CC_C_O()
dnl AM_PROG_LIBTOOL()

dnl AC_SUBST(VAR, [var_value])


dnl ----------------------------------------
dnl Checks for libraries.
dnl ----------------------------------------


dnl ----------------------------------------
dnl Checks for header files.
dnl ----------------------------------------
dnl AC_HEADER_STDC()

dnl AC_CHECK_HEADERS([fcntl.h stdlib.h string.h strings.h unistd.h])


dnl ----------------------------------------
dnl Checks for types, structures, compiler characteristics.
dnl ----------------------------------------
dnl AC_C_CONST()
dnl AC_C_INLINE()

dnl AC_TYPE_OFF_T()
dnl AC_TYPE_SIZE_T()
dnl AC_TYPE_SSIZE_T()
dnl AC_CHECK_SIZEOF(unsigned long long int)


dnl ----------------------------------------
dnl Checks for library functions.
dnl ----------------------------------------
dnl AC_FUNC_MEMCMP()

dnl AC_CHECK_FUNCS([memmove memset strcasecmp])
dnl AC_CHECK_LIB([xxxx], [func_xxxx], [], 
dnl	[AC_MSG_ERROR([cannot find libxxxx])])


dnl ----------------------------------------
dnl Checks for system services.
dnl ----------------------------------------
AC_CHECK_PROGS([DOXYGEN], [doxygen])
AM_CONDITIONAL([HAVE_DOXYGEN], [test x$DOXYGEN != x])
if test x$DOXYGEN = x; then
	AC_MSG_WARN([doxygen not found, continuing without docs])
fi


dnl ----------------------------------------
dnl Checks for arguments.
dnl ----------------------------------------
AC_ARG_ENABLE(debug, 
	AS_HELP_STRING([--enable-debug], 
		[turn on debugging [default=no]]),
	, enable_debug=no)
AC_MSG_CHECKING(whether to enable debuging)
AC_MSG_RESULT($enable_debug)
AM_CONDITIONAL(ENABLE_DEBUG, [test x$enable_debug = xyes])
if test x$enable_debug = xyes; then
	AC_DEFINE(ENABLE_DEBUG, 1, Define to enable the debug functions)
	CFLAGS="$CFLAGS -g -O0 "
	CXXFLAGS="$CXXFLAGS -g -O0 "
else
	CFLAGS="$CFLAGS -O2 "
	CXXFLAGS="$CXXFLAGS -O2 "
fi

AC_ARG_ENABLE(use_bellagio, 
	AS_HELP_STRING([--enable-use-bellagio],
		[use libomxil-bellagio [default=no]]),
	, enable_use_bellagio=no)
AC_MSG_CHECKING(whether to use libomxil-bellagio)
AC_MSG_RESULT($enable_use_bellagio)
AM_CONDITIONAL(USE_BELLAGIO, test x$enable_use_bellagio = xyes)
if test x$enable_use_bellagio = xyes; then
	AC_CHECK_HEADERS([OMX_Core.h OMX_Component.h OMX_Types.h OMX_Audio.h OMX_Video.h], [], 
		[AC_MSG_ERROR([cannot find OMX_Core.h])])
	AC_CHECK_LIB([omxil-bellagio], [OMX_Init], [], 
		[AC_MSG_ERROR([cannot find libomxil-bellagio])])
else
	:;
fi


dnl ----------------------------------------
dnl Output the configure.
dnl ----------------------------------------
AC_CONFIG_FILES([Makefile include/Makefile src/Makefile lib/Makefile 
	src/api/Makefile src/util/Makefile src/component/Makefile 
	src/regist/Makefile src/debug/Makefile 
	component/Makefile 
	doc/Makefile 
	tests/Makefile tests/common/Makefile 
	tests/basic/Makefile tests/interop/Makefile tests/simple/Makefile ])

AC_CONFIG_SUBDIRS([lib/cppunit-1.12.1])
AC_CONFIG_SUBDIRS([component/empty])
AC_CONFIG_SUBDIRS([component/simple])
AC_CONFIG_SUBDIRS([component/reader_ts])

AC_OUTPUT()
