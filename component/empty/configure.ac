
AC_PREREQ(2.61)

dnl ----------------------------------------
dnl Information on this package.
dnl ----------------------------------------
AC_INIT([omxil-mf-empty], [0.1])

AC_ARG_PROGRAM()

AC_CONFIG_SRCDIR([src/entry/entry.cpp])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([.])
dnl AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign ])
LT_INIT()

dnl ----------------------------------------
dnl Checks for programs.
dnl ----------------------------------------
AC_PROG_CC()
AC_PROG_CXX()
AC_PROG_AWK()
AC_PROG_INSTALL()
AC_PROG_LN_S()
AC_PROG_LIBTOOL()
AC_PROG_SED()
AM_PROG_CC_C_O()
AM_PROG_LIBTOOL()

dnl AC_SUBST(VAR, [var_value])


dnl ----------------------------------------
dnl Checks for libraries.
dnl ----------------------------------------


dnl ----------------------------------------
dnl Checks for header files.
dnl ----------------------------------------
AC_HEADER_STDC()

dnl AC_CHECK_HEADERS([fcntl.h stdlib.h string.h strings.h unistd.h])


dnl ----------------------------------------
dnl Checks for types, structures, compiler characteristics.
dnl ----------------------------------------
AC_C_CONST()
AC_C_INLINE()


dnl ----------------------------------------
dnl Checks for library functions.
dnl ----------------------------------------
AC_FUNC_MEMCMP()

dnl AC_CHECK_LIB([xxxx], [func_xxxx], [], 
dnl	[AC_MSG_ERROR([cannot find libxxxx])])


dnl ----------------------------------------
dnl Checks for system services.
dnl ----------------------------------------


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

AC_ARG_ENABLE(use_inner_lib, 
	AS_HELP_STRING([--enable-use-inner-lib], 
		[use inner library for debug [default=no]]),
	, enable_use_inner_lib=no)
AC_MSG_CHECKING(whether to enable using inner library)
AC_MSG_RESULT($enable_use_inner_lib)
AM_CONDITIONAL(USE_INNER_LIB, [test x$enable_use_inner_lib = xyes])


dnl ----------------------------------------
dnl Output the configure.
dnl ----------------------------------------
AC_CONFIG_FILES([Makefile src/Makefile 
	src/entry/Makefile src/reader_zero/Makefile 
	src/renderer_null/Makefile src/filter_copy/Makefile 
	tests/Makefile ])

AC_OUTPUT()
