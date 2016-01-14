
check_PROGRAMS = \
	init_deinit \
	init_deinit_multi \
	enum_components \
	get_roles_of_component \
	get_components_of_role \
	get_handle \
	get_param_init \
	set_port_format \
	send_command \
	empty_buffer \
	fill_buffer \
	empty_fill \
	empty_fill_flush

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

init_deinit_SOURCES   = test_init_deinit.c
init_deinit_LDADD     = $(common_ldadd)
init_deinit_CPPFLAGS  = $(common_cppflags)
init_deinit_CFLAGS    = $(common_cflags)
init_deinit_CXXFLAGS  = $(common_cxxflags)
init_deinit_LDFLAGS   = $(common_ldflags)

init_deinit_multi_SOURCES   = test_init_deinit_multi.c
init_deinit_multi_LDADD     = $(common_ldadd)
init_deinit_multi_CPPFLAGS  = $(common_cppflags)
init_deinit_multi_CFLAGS    = $(common_cflags)
init_deinit_multi_CXXFLAGS  = $(common_cxxflags)
init_deinit_multi_LDFLAGS   = $(common_ldflags)

enum_components_SOURCES   = test_enum_components.c
enum_components_LDADD     = $(common_ldadd)
enum_components_CPPFLAGS  = $(common_cppflags)
enum_components_CFLAGS    = $(common_cflags)
enum_components_CXXFLAGS  = $(common_cxxflags)
enum_components_LDFLAGS   = $(common_ldflags)

get_roles_of_component_SOURCES   = test_get_roles_of_component.cpp
get_roles_of_component_LDADD     = $(common_ldadd)
get_roles_of_component_CPPFLAGS  = $(common_cppflags)
get_roles_of_component_CFLAGS    = $(common_cflags)
get_roles_of_component_CXXFLAGS  = $(common_cxxflags)
get_roles_of_component_LDFLAGS   = $(common_ldflags)

get_components_of_role_SOURCES   = test_get_components_of_role.cpp
get_components_of_role_LDADD     = $(common_ldadd)
get_components_of_role_CPPFLAGS  = $(common_cppflags)
get_components_of_role_CFLAGS    = $(common_cflags)
get_components_of_role_CXXFLAGS  = $(common_cxxflags)
get_components_of_role_LDFLAGS   = $(common_ldflags)

get_handle_SOURCES   = test_get_handle.cpp
get_handle_LDADD     = $(common_ldadd)
get_handle_CPPFLAGS  = $(common_cppflags)
get_handle_CFLAGS    = $(common_cflags)
get_handle_CXXFLAGS  = $(common_cxxflags)
get_handle_LDFLAGS   = $(common_ldflags)

get_param_init_SOURCES   = test_get_param_init.cpp
get_param_init_LDADD     = $(common_ldadd)
get_param_init_CPPFLAGS  = $(common_cppflags)
get_param_init_CFLAGS    = $(common_cflags)
get_param_init_CXXFLAGS  = $(common_cxxflags)
get_param_init_LDFLAGS   = $(common_ldflags)

set_port_format_SOURCES   = test_set_port_format.cpp
set_port_format_LDADD     = $(common_ldadd)
set_port_format_CPPFLAGS  = $(common_cppflags)
set_port_format_CFLAGS    = $(common_cflags)
set_port_format_CXXFLAGS  = $(common_cxxflags)
set_port_format_LDFLAGS   = $(common_ldflags)

send_command_SOURCES   = test_send_command.cpp
send_command_LDADD     = $(common_ldadd)
send_command_CPPFLAGS  = $(common_cppflags)
send_command_CFLAGS    = $(common_cflags)
send_command_CXXFLAGS  = $(common_cxxflags)
send_command_LDFLAGS   = $(common_ldflags)

empty_buffer_SOURCES   = test_empty_buffer.cpp
empty_buffer_LDADD     = $(common_ldadd)
empty_buffer_CPPFLAGS  = $(common_cppflags)
empty_buffer_CFLAGS    = $(common_cflags)
empty_buffer_CXXFLAGS  = $(common_cxxflags)
empty_buffer_LDFLAGS   = $(common_ldflags)

fill_buffer_SOURCES   = test_fill_buffer.cpp
fill_buffer_LDADD     = $(common_ldadd)
fill_buffer_CPPFLAGS  = $(common_cppflags)
fill_buffer_CFLAGS    = $(common_cflags)
fill_buffer_CXXFLAGS  = $(common_cxxflags)
fill_buffer_LDFLAGS   = $(common_ldflags)

empty_fill_SOURCES   = test_empty_fill.cpp
empty_fill_LDADD     = $(common_ldadd)
empty_fill_CPPFLAGS  = $(common_cppflags)
empty_fill_CFLAGS    = $(common_cflags)
empty_fill_CXXFLAGS  = $(common_cxxflags)
empty_fill_LDFLAGS   = $(common_ldflags)

empty_fill_flush_SOURCES   = test_empty_fill_flush.cpp
empty_fill_flush_LDADD     = $(common_ldadd)
empty_fill_flush_CPPFLAGS  = $(common_cppflags)
empty_fill_flush_CFLAGS    = $(common_cflags)
empty_fill_flush_CXXFLAGS  = $(common_cxxflags)
empty_fill_flush_LDFLAGS   = $(common_ldflags)

TESTS = \
	init_deinit \
	init_deinit_multi \
	enum_components \
	get_roles_of_component.sh \
	get_components_of_role.sh \
	get_handle.sh \
	get_param_init.sh \
	set_port_format.sh \
	send_command.sh \
	empty_buffer.sh \
	fill_buffer.sh \
	empty_fill.sh \
	empty_fill_flush.sh

