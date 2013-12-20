
check_PROGRAMS = \
	init_deinit_test \
	enum_components_test \
	get_roles_of_component_test \
	get_components_of_role_test \
	get_handle_test

if USE_BELLAGIO
  test_LDADD    = -lomxil-bellagio
  test_CPPFLAGS = \
  	-I/usr/include \
  	-DUSE_BELLAGIO
  test_CFLAGS   = 
else
  test_LDADD    = $(top_builddir)/src/libomxil-study.la
  test_CPPFLAGS = \
  	-I$(top_srcdir)/include \
  	-DUSE_STUDY
  test_CFLAGS   = 
endif

#common_LDADD  = ../common/libtestcommon.la
#common_CFLAGS = -I$(srcdir)/../common

init_deinit_test_SOURCES   = test_init_deinit.c
init_deinit_test_LDADD     = $(test_LDADD) $(common_LDADD)
init_deinit_test_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
init_deinit_test_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

enum_components_test_SOURCES   = test_enum_components.c
enum_components_test_LDADD     = $(test_LDADD) $(common_LDADD)
enum_components_test_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
enum_components_test_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

get_roles_of_component_test_SOURCES   = test_get_roles_of_component.c
get_roles_of_component_test_LDADD     = $(test_LDADD) $(common_LDADD)
get_roles_of_component_test_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
get_roles_of_component_test_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

get_components_of_role_test_SOURCES   = test_get_components_of_role.c
get_components_of_role_test_LDADD     = $(test_LDADD) $(common_LDADD)
get_components_of_role_test_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
get_components_of_role_test_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

get_handle_test_SOURCES   = test_get_handle.c
get_handle_test_LDADD     = $(test_LDADD) $(common_LDADD)
get_handle_test_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
get_handle_test_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

TESTS = $(check_PROGRAMS)

