TEST_DIR   = $(top_srcdir)/tests
COMMON_DIR = $(TEST_DIR)/common

check_PROGRAMS = \
	init_deinit \
	init_deinit_multi \
	enum_components \
	get_roles_of_component \
	get_components_of_role \
	get_handle \
	get_param_init \
	send_command \
	empty_buffer \
	fill_buffer

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

init_deinit_SOURCES   = test_init_deinit.c $(common_SOURCES)
init_deinit_LDADD     = $(common_LDADD)
init_deinit_CPPFLAGS  = $(common_CPPFLAGS)
init_deinit_CFLAGS    = $(common_CFLAGS)
init_deinit_CXXFLAGS  = $(common_CXXFLAGS)

init_deinit_multi_SOURCES   = test_init_deinit_multi.c $(common_SOURCES)
init_deinit_multi_LDADD     = $(common_LDADD)
init_deinit_multi_CPPFLAGS  = $(common_CPPFLAGS)
init_deinit_multi_CFLAGS    = $(common_CFLAGS)
init_deinit_multi_CXXFLAGS  = $(common_CXXFLAGS)

enum_components_SOURCES   = test_enum_components.c $(common_SOURCES)
enum_components_LDADD     = $(common_LDADD)
enum_components_CPPFLAGS  = $(common_CPPFLAGS)
enum_components_CFLAGS    = $(common_CFLAGS)
enum_components_CXXFLAGS  = $(common_CXXFLAGS)

get_roles_of_component_SOURCES   = test_get_roles_of_component.c $(common_SOURCES)
get_roles_of_component_LDADD     = $(common_LDADD)
get_roles_of_component_CPPFLAGS  = $(common_CPPFLAGS)
get_roles_of_component_CFLAGS    = $(common_CFLAGS)
get_roles_of_component_CXXFLAGS  = $(common_CXXFLAGS)

get_components_of_role_SOURCES   = test_get_components_of_role.c $(common_SOURCES)
get_components_of_role_LDADD     = $(common_LDADD)
get_components_of_role_CPPFLAGS  = $(common_CPPFLAGS)
get_components_of_role_CFLAGS    = $(common_CFLAGS)
get_components_of_role_CXXFLAGS  = $(common_CXXFLAGS)

get_handle_SOURCES   = test_get_handle.cpp $(common_SOURCES)
get_handle_LDADD     = $(common_LDADD)
get_handle_CPPFLAGS  = $(common_CPPFLAGS)
get_handle_CFLAGS    = $(common_CFLAGS)
get_handle_CXXFLAGS  = $(common_CXXFLAGS)

get_param_init_SOURCES   = test_get_param_init.cpp $(common_SOURCES)
get_param_init_LDADD     = $(common_LDADD)
get_param_init_CPPFLAGS  = $(common_CPPFLAGS)
get_param_init_CFLAGS    = $(common_CFLAGS)
get_param_init_CXXFLAGS  = $(common_CXXFLAGS)

send_command_SOURCES   = test_send_command.cpp $(common_SOURCES)
send_command_LDADD     = $(common_LDADD)
send_command_CPPFLAGS  = $(common_CPPFLAGS)
send_command_CFLAGS    = $(common_CFLAGS)
send_command_CXXFLAGS  = $(common_CXXFLAGS)

empty_buffer_SOURCES   = test_empty_buffer.cpp $(common_SOURCES)
empty_buffer_LDADD     = $(common_LDADD)
empty_buffer_CPPFLAGS  = $(common_CPPFLAGS)
empty_buffer_CFLAGS    = $(common_CFLAGS)
empty_buffer_CXXFLAGS  = $(common_CXXFLAGS)

fill_buffer_SOURCES   = test_fill_buffer.cpp $(common_SOURCES)
fill_buffer_LDADD     = $(common_LDADD)
fill_buffer_CPPFLAGS  = $(common_CPPFLAGS)
fill_buffer_CFLAGS    = $(common_CFLAGS)
fill_buffer_CXXFLAGS  = $(common_CXXFLAGS)

TESTS = \
	init_deinit \
	init_deinit_multi \
	enum_components \
	get_roles_of_component.sh \
	get_components_of_role.sh \
	get_handle.sh \
	get_param_init.sh \
	send_command.sh \
	empty_buffer.sh \
	fill_buffer.sh

