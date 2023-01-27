# -*- mode: cmake; -*-
cmake_minimum_required(VERSION 2.6)

set (CMAKE_VERBOSE_MAKEFILE  ON)

project(LuaBind)

# LUA_INCLUDE is defined in papercrop/CMakeList. 

# Make sure the compiler can find include files from our Hello library.
include_directories (
${LuaBind_SOURCE_DIR}
${boost_INCLUDE}
${LUA_INCLUDE}
#${LuaBind_SOURCE_DIR}/../../gsl-1.6
)
message("luainclude=${LUA_INCLUDE}")

#-DLUABIND_NO_ERROR_CHECKING 

if(UNIX)
add_definitions(-DNO_GUI -DNDEBUG -Wall -ftemplate-depth-100)
else()
add_definitions(-DNO_GUI -DWIN32 -DNDEBUG -D_WINDOWS -Wall -ftemplate-depth-100)
endif()

set(src
"src/class.cpp"
"src/class_info.cpp"
"src/class_registry.cpp"
"src/class_rep.cpp"
"src/create_class.cpp"
"src/error.cpp"
"src/exception_handler.cpp"
"src/function.cpp"
"src/inheritance.cpp"
"src/link_compatibility.cpp"
"src/object_rep.cpp"
"src/open.cpp"
"src/pcall.cpp"
"src/scope.cpp"
"src/stack_content_by_name.cpp"
"src/weak_ref.cpp"
"src/wrapper_base.cpp"
)


if (LUABIND_TEST)
set(test_simple_class
"test/test_simple_class.cpp"
"test/main.cpp"
)

set(test_scope
"test/test_scope.cpp"
"test/main.cpp"
)

set(other_tests
"test/test_typetraits.cpp"
"test/test_yield.cpp"
"test/test_null_pointer.cpp"
"test/test_policies.cpp"
"test/test_free_functions.cpp"
"test/test_iterator.cpp"
"test/test_abstract_base.cpp"
"test/test_attributes.cpp"
"test/test_const.cpp"
"test/test_construction.cpp"
"test/test_exceptions.cpp"
"test/test_free_functions.cpp"
"test/test_held_type.cpp"
"test/test_implicit_cast.cpp"
"test/test_iterator.cpp"
"test/test_lua_classes.cpp"
"test/test_null_pointer.cpp"
"test/test_object.cpp"
"test/test_operators.cpp"
"test/test_policies.cpp"
"test/test_scope.cpp"
"test/test_separation.cpp"
)
endif(LUABIND_TEST)

add_library(luabind
${src}
)

if(UNIX)
if(LUABIND_TEST)
add_executable(test_simple_class
${test_simple_class}
)

add_executable(test_scope
${test_scope}
)


#target_link_libraries(test_simple_class
target_link_libraries(test_simple_class
${LuaBind_SOURCE_DIR}/libLuaBind.a
${LUA_LIB}
m 
dl 
readline 
history 
ncurses
)

target_link_libraries(test_scope
${LuaBind_SOURCE_DIR}/libLuaBind.a
${LUA_LIB}
m 
dl 
readline 
history 
ncurses
)

endif(LUABIND_TEST)
endif(UNIX)
