cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -g -fPIC")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

#----- define all individual modules to be built beforehand

set(HECTOR_MODULES Core IO Elements Beamline Propagator)
set(HECTOR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Hector)

#----- enable fortran (for Pythia/Herwig/...)

enable_language(Fortran OPTIONAL)

#----- include external dependencies

find_package(CLHEP REQUIRED)
set(EXT_LIBS CLHEP)

find_package(Pythia8)

#----- build all the intermediate objects

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${HECTOR_SOURCE_DIR})
foreach(_module ${HECTOR_MODULES})
  include_directories(${HECTOR_SOURCE_DIR}/${_module})
  add_subdirectory(Hector/${_module})
endforeach()

#----- link everything into the library and executable

# do not forget to retrieve
# http://home.thep.lu.se/~leif/LHEF/LHEF.h

add_library(Hector SHARED $<TARGET_OBJECTS:HectorCore>
                          $<TARGET_OBJECTS:HectorIO>
                          $<TARGET_OBJECTS:HectorElements>
                          $<TARGET_OBJECTS:HectorBeamline>
                          $<TARGET_OBJECTS:HectorPropagator>)
set_target_properties(Hector PROPERTIES LINKER_LANGUAGE CXX)

if(DEFINED PYTHIA8_FOUND)
  list(APPEND EXT_LIBS ${PYTHIA8_LIBRARY})
endif()

install(TARGETS Hector DESTINATION lib)
install(DIRECTORY ${HECTOR_MODULES} DESTINATION include FILES_MATCHING PATTERN "*.h")

#----- test executable

add_executable(hector test/test_hector.cpp)
target_link_libraries(hector Hector ${EXT_LIBS})

#----- add the tests

enable_testing()
add_subdirectory(test)

#----- copy the data files

file(GLOB input RELATIVE ${PROJECT_SOURCE_DIR} data/twiss/*)
foreach(_files ${input})
  configure_file(${_files} ${_files} COPYONLY)
endforeach()

