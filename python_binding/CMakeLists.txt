# Partially based on https://github.com/Mizux/cmake-swig/blob/main/Foo/python/CMakeLists.txt

# this defines swig_add_library and was set by FindSWIG aka find_package(SWIG ...)
# see https://cmake.org/cmake/help/latest/module/FindSWIG.html
# and https://cmake.org/cmake/help/latest/module/UseSWIG.html
include(${SWIG_USE_FILE})

# iric.i
set_property(SOURCE iric.i PROPERTY SWIG_MODULE_NAME iric)
set_property(SOURCE iric.i PROPERTY CPLUSPLUS ON)
swig_add_library(iric LANGUAGE python SOURCES ${CMAKE_CURRENT_LIST_DIR}/iric.i)

# this is required to avoid the header file pyconfig.h forcing
# a link against python38_d.lib during debug builds
# Python_LIBRARIES="C:/Users/charlton/Miniconda3/libs/python38.lib"
# vs
# Python_LIBRARIES="optimized;C:/Python38-x64/libs/python38.lib;debug;C:/Python38-x64/libs/python38_d.lib"
#
# 2022-05-18 -- This no longer compiles on VS2019 Debug
# Now we must use a Python distribution with libs/pythonXX_d.lib
#
# if (WIN32)
#   list(FIND Python_LIBRARIES debug debug_index)
#   if (debug_index EQUAL -1)
#     target_compile_definitions(iric
#       PRIVATE
#         SWIG_PYTHON_INTERPRETER_NO_DEBUG
#     )
#   endif()
# endif()

# use imported targets for iric
target_link_libraries(iric
  PRIVATE
    iriclib::iriclib
    Python::Python
)

# this only works with Visual Studio generators
# exclude iric from default ALL_BUILD project (Debug)
# set_target_properties(iric
#   PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_DEBUG TRUE
# )

install(TARGETS iric
  LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}/Lib/site-packages
)

install(FILES ${CMAKE_CURRENT_LIST_DIR}/iric_custom.py
  DESTINATION ${CMAKE_INSTALL_BINDIR}/Lib/site-packages
  RENAME iric.py
)
