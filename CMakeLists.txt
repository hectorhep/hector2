cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -g -fPIC")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

#----- define all individual modules to be built beforehand

set(MODULES Core IO Elements Beamline Propagator)
set(HECTOR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Hector)

#----- enable fortran (for Pythia/Herwig/...)

enable_language(Fortran OPTIONAL)

#----- include external hadronisers/...

find_package(CLHEP REQUIRED)
#add_subdirectory(external)
find_package(PythonLibs 2.7 REQUIRED)
set(EXT_LIBS CLHEP)

find_package(Pythia8)

#----- build all the intermediate objects

include_directories(${HECTOR_SOURCE_DIR})
foreach(_module ${MODULES})
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

install(TARGETS Hector DESTINATION lib)
install(DIRECTORY ${MODULES} DESTINATION include FILES_MATCHING PATTERN "*.h")

if(DEFINED PYTHIA8_FOUND)
  list(APPEND EXT_LIBS ${PYTHIA8_LIBRARY})
endif()

#----- test executable

add_executable(hector test/test_hector.cpp)
target_link_libraries(hector Hector ${EXT_LIBS})

#----- python wrapping

include(UseCython)
add_custom_target(ReplicatePythonSourceTree ALL ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ReplicatePythonSourceTree.cmake ${CMAKE_CURRENT_BINARY_DIR} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(pyx_files "")
foreach(_mod ${MODULES})
  file(GLOB sources ${HECTOR_SOURCE_DIR}/${_mod}/*.pyx)
  set_source_files_properties(${sources} PROPERTIES CYTHON_IS_CXX 1)
  list(APPEND pyx_files ${sources})
endforeach()
cython_add_module(pyhector ${pyx_files})
target_link_libraries(pyhector ${EXT_LIBS} Hector)

#----- add the tests

add_subdirectory(test)

#----- copy the data files

file(GLOB input RELATIVE ${PROJECT_SOURCE_DIR} data/twiss/*)
foreach(_files ${input})
  configure_file(${_files} ${_files} COPYONLY)
endforeach()

