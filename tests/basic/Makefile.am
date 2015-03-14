
TEST_DIR   = ..
COMMON_DIR = $(TEST_DIR)/common

check_PROGRAMS = \
	init_deinit \
	enum_components \
	get_roles_of_component \
	get_components_of_role \
	get_handle \
	send_command

if USE_BELLAGIO
  test_LDADD    = -lomxil-bellagio
  test_CPPFLAGS = \
  	-I/usr/include \
  	-DUSE_BELLAGIO
  test_CFLAGS   = 
else
  test_LDADD    = $(top_builddir)/src/libomxil-mf.la
  test_CPPFLAGS = \
  	-I$(top_srcdir)/include \
  	-DUSE_MF
  test_CFLAGS   = 
endif

common_SOURCES = $(COMMON_DIR)/omxil_utils.c
#common_LDADD  = ../common/libtestcommon.la
common_CPPFLAGS = -I$(TEST_DIR)
#common_CFLAGS = -I$(srcdir)/../common

init_deinit_SOURCES   = test_init_deinit.c $(common_SOURCES)
init_deinit_LDADD     = $(test_LDADD) $(common_LDADD)
init_deinit_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
init_deinit_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

enum_components_SOURCES   = test_enum_components.c $(common_SOURCES)
enum_components_LDADD     = $(test_LDADD) $(common_LDADD)
enum_components_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
enum_components_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

get_roles_of_component_SOURCES   = test_get_roles_of_component.c $(common_SOURCES)
get_roles_of_component_LDADD     = $(test_LDADD) $(common_LDADD)
get_roles_of_component_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
get_roles_of_component_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

get_components_of_role_SOURCES   = test_get_components_of_role.c $(common_SOURCES)
get_components_of_role_LDADD     = $(test_LDADD) $(common_LDADD)
get_components_of_role_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
get_components_of_role_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

get_handle_SOURCES   = test_get_handle.c $(common_SOURCES)
get_handle_LDADD     = $(test_LDADD) $(common_LDADD)
get_handle_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
get_handle_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

send_command_SOURCES   = test_send_command.c $(common_SOURCES)
send_command_LDADD     = $(test_LDADD) $(common_LDADD)
send_command_CPPFLAGS  = $(test_CPPFLAGS) $(common_CPPFLAGS)
send_command_CFLAGS    = $(test_CFLAGS) $(common_CFLAGS)

TESTS = \
	init_deinit \
	enum_components \
	get_roles_of_component.sh \
	get_components_of_role.sh \
	get_handle \
	send_command

