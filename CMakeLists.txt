cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -g -fPIC")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

#----- define all individual modules to be built beforehand

set(MODULES Core IO Elements Beamline Propagator)

#----- enable fortran (for Pythia/Herwig/...)

enable_language(Fortran OPTIONAL)

#----- include external hadronisers/...

find_package(CLHEP REQUIRED)
#add_subdirectory(external)
#
#if(EXISTS $ENV{has_pythia6})
#  add_definitions(-DPYTHIA6)
#endif()

#----- build all the intermediate objects

include_directories(${PROJECT_SOURCE_DIR})
foreach(_module ${MODULES})
  include_directories(${PROJECT_SOURCE_DIR}/${_module})
  add_subdirectory(${_module})
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

find_package(Pythia8 QUIET)
if(DEFINED PYTHIA8_FOUND)
  target_link_libraries(Hector CLHEP ${PYTHIA8_LIBRARY})
else()
  target_link_libraries(Hector CLHEP)
endif()

install(TARGETS Hector DESTINATION lib)
install(DIRECTORY ${MODULES} DESTINATION include/Hector FILES_MATCHING PATTERN "*.h")

add_executable(hector main.cpp)
target_link_libraries(hector Hector)

#----- add the tests

add_subdirectory(test)

#----- copy the data files

file(GLOB input RELATIVE ${PROJECT_SOURCE_DIR} data/twiss/*)
foreach(_files ${input})
  configure_file(
    ${_files}
    ${_files}
    COPYONLY
)
endforeach()

