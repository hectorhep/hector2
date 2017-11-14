cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

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
install(DIRECTORY ${HECTOR_MODULES} DESTINATION include FILES_MATCHING PATTERN "*.h")

#----- test executable

add_executable(hector test/test_hector.cpp)
target_link_libraries(hector Hector2 ${HECTOR_DEPENDENCIES})

#----- add the tests

enable_testing()
add_subdirectory(test)

#----- copy the data files

file(GLOB input RELATIVE ${PROJECT_SOURCE_DIR} data/twiss/*)
foreach(_files ${input})
  configure_file(${_files} ${_files} COPYONLY)
endforeach()

