cmake_minimum_required(VERSION 2.9 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)
if(CMAKE_VERSION VERSION_GREATER 3.1)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_C_STANDARD 11)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
endif()


option(BUILD_PYTHON "Build the Python bindings" OFF)
option(BUILD_TESTS "Build the tests" ON)

#----- include external dependencies, prepare the environment

include(SetEnvironment)

#----- define all individual modules to be built beforehand

set(HECTOR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(HECTOR_TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(HECTOR_DEPENDENCIES ${CLHEP_LIB})
set(HECTOR_INC_DEPENDENCIES ${CLHEP_INCLUDE})

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

# do not forget to retrieve
# http://home.thep.lu.se/~leif/LHEF/LHEF.h

add_library(Hector2 SHARED ${LIB_SRC} ${HECTOR_INCLUDES})
set_target_properties(Hector2 PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Hector2 PUBLIC ${HECTOR_INC_DEPENDENCIES})
target_link_libraries(Hector2 ${HECTOR_DEPENDENCIES})

add_executable(hector ${HECTOR_SOURCE_DIR}/hector.cc)
target_link_libraries(hector Hector2)

install(TARGETS Hector2 DESTINATION lib)
install(TARGETS hector DESTINATION bin)
install(DIRECTORY Hector DESTINATION include FILES_MATCHING PATTERN "*.h")

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

