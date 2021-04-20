cmake_minimum_required(VERSION 2.9 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

option(BUILD_PYTHON "Build the Python bindings" OFF)

#----- include external dependencies, prepare the environment

include(SetEnvironment)

#----- define all individual modules to be built beforehand

set(HECTOR_MODULES Core IO Elements Beamline Propagator Utils)
set(HECTOR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Hector)

#----- build all the intermediate objects

include_directories(${PROJECT_SOURCE_DIR})
foreach(_module ${HECTOR_MODULES})
  include_directories(${HECTOR_SOURCE_DIR}/${_module})
  add_subdirectory(Hector/${_module})
endforeach()

#----- link everything into the library and executable

# do not forget to retrieve
# http://home.thep.lu.se/~leif/LHEF/LHEF.h

add_library(Hector2 SHARED $<TARGET_OBJECTS:HectorCore>
                           $<TARGET_OBJECTS:HectorIO>
                           $<TARGET_OBJECTS:HectorElements>
                           $<TARGET_OBJECTS:HectorBeamline>
                           $<TARGET_OBJECTS:HectorPropagator>
                           $<TARGET_OBJECTS:HectorUtils>)
set_target_properties(Hector2 PROPERTIES LINKER_LANGUAGE CXX)

install(TARGETS Hector2 DESTINATION lib)
install(DIRECTORY Hector DESTINATION include FILES_MATCHING PATTERN "*.h")

#----- python wrapper

if(BUILD_PYTHON AND Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
  python_add_module(pyhector ${HECTOR_SOURCE_DIR}/PythonWrapper.cc ${HECTOR_SOURCE_DIR}/__init__.py)
  target_link_libraries(pyhector Hector2 ${HECTOR_DEPENDENCIES} ${Boost_LIBRARIES})
  #execute_process(
  #  COMMAND python27 -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())"
  #  OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
  #if(PYTHON_SITE_PACKAGES)
  #  message(STATUS "Python site packages directory in ${PYTHON_SITE_PACKAGES}")
  #  configure_file(${HECTOR_SOURCE_DIR}/__init__.py ${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py COPYONLY)
  #  install(TARGETS pyhector ${CMAKE_CURRENT_BINARY_DIR}/src/__init__.py DESTINATION ${PYTHON_SITE_PACKAGES}/hector)
  #endif()
endif()

#----- add the tests

enable_testing()
add_subdirectory(test)

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

