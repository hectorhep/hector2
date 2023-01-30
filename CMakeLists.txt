cmake_minimum_required(VERSION 2.9 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE BOOL "RPATH USE LINK PATH")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake $ENV{ROOTSYS}/cmake)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install" CACHE PATH "default install path" FORCE)
endif()

if(CMAKE_VERSION VERSION_GREATER 3.1)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_C_STANDARD 11)
  find_program(CCACHE ccache HINTS $ENV{CCACHE_PATH} PATH_SUFFIXES bin)
  if(CCACHE)
    message(STATUS "CCache found in ${CCACHE}")
    if(IS_LXPLUS)
      set(CCACHE_DIR "${CMAKE_SOURCE_DIR}/ccache")
      file(MAKE_DIRECTORY ${CCACHE_DIR})
      message(STATUS "CCache output directory set to ${CCACHE_DIR}")
    endif(IS_LXPLUS)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE})
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE}) # Less useful to do it for linking, see edit2
  endif(CCACHE)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
endif()

include(GNUInstallDirs)

# Give user the choice to override installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")

option(BUILD_PYTHON "Build the Python bindings" OFF)
option(BUILD_TESTS "Build the tests" ON)

#----- include external dependencies, prepare the environment

include(SetEnvironment)
include(BuildUtils)

#----- define all individual modules to be built beforehand

set(HECTOR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(HECTOR_TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(HECTOR_DEPENDENCIES ${EIGEN3})
set(HECTOR_INC_DEPENDENCIES ${EIGEN3_INCLUDE_DIR})

set(PYHECTOR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/python)

#----- build all the intermediate objects

include_directories(${PROJECT_SOURCE_DIR})
file(GLOB LIB_SRC ${HECTOR_SOURCE_DIR}/*.cc
                  ${HECTOR_SOURCE_DIR}/IO/*.cc
                  ${HECTOR_SOURCE_DIR}/Elements/*.cc
                  ${HECTOR_SOURCE_DIR}/Apertures/*.cc
                  ${HECTOR_SOURCE_DIR}/Utils/*.cc)
file(GLOB_RECURSE HECTOR_INCLUDES ${PROJECT_SOURCE_DIR}/Hector/*.h)

#----- link everything into the library and executable

if(HEPMC_LIB)
  list(APPEND HECTOR_INC_DEPENDENCIES ${HEPMC_INCLUDE})
  list(APPEND HECTOR_DEPENDENCIES ${HEPMC_LIB})
  add_definitions(-DHEPMC)
endif()
if(PYTHIA8)
  message(STATUS "Pythia8 found in ${PYTHIA8}")
  message(STATUS "Pythia8 includes located in ${PYTHIA8_INCLUDE}")
  list(APPEND HECTOR_DEPENDENCIES ${PYTHIA8} dl)
  list(APPEND HECTOR_INC_DEPENDENCIES ${PYTHIA8_INCLUDE})
  if(LHAPDF)
    list(APPEND HECTOR_DEPENDENCIES ${LHAPDF})
  else()
    list(APPEND HECTOR_DEPENDENCIES lhapdfdummy)
  endif()
else()
  list(REMOVE_ITEM LIB_SRC ${HECTOR_SOURCE_DIR}/IO/Pythia8Generator.cc)
  list(REMOVE_ITEM HECTOR_INCLUDES ${PROJECT_SOURCE_DIR}/Hector/IO/Pythia8Generator.h)
endif()

add_library(Hector2 SHARED ${LIB_SRC} ${HECTOR_INCLUDES})
set_target_properties(Hector2 PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Hector2 PUBLIC ${HECTOR_INC_DEPENDENCIES})
target_link_libraries(Hector2 ${HECTOR_DEPENDENCIES})

add_executable(hector ${HECTOR_SOURCE_DIR}/hector.cc)
target_link_libraries(hector Hector2)
set_target_properties(hector PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

install(TARGETS Hector2 DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(TARGETS hector DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY Hector DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h")

#----- add the tests

enable_testing()
add_subdirectory(test)

#----- add-ons

add_subdirectory(HectorAddOns)

#----- python wrapper

if(BUILD_PYTHON AND Boost_FOUND)
  python_add_module(pyhector ${PYHECTOR_SOURCE_DIR}/PythonWrapper.cc ${PYHECTOR_SOURCE_DIR}/__init__.py)
  target_link_libraries(pyhector Hector2 ${HECTOR_DEPENDENCIES} ${Boost_LIBRARIES})
  target_include_directories(pyhector INTERFACE ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
  #execute_process(
  #  COMMAND python27 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
  #  OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
  #if(PYTHON_SITE_PACKAGES)
  #  message(STATUS "Python site packages directory in ${PYTHON_SITE_PACKAGES}")
  #  configure_file(${HECTOR_SOURCE_DIR}/__init__.py ${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py COPYONLY)
  #  install(TARGETS pyhector ${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py DESTINATION ${PYTHON_SITE_PACKAGES}/hector)
  #endif()
endif()

#----- copy the data files

file(GLOB input RELATIVE ${PROJECT_SOURCE_DIR} data/twiss/*)
foreach(_files ${input})
  configure_file(${_files} ${_files} COPYONLY)
endforeach()

#----- documentation

find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  message(STATUS "Doxygen build started")
  add_custom_target(doc_doxygen
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating Hector documentation with Doxygen"
    VERBATIM)
endif()

