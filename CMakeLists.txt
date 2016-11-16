cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -g -fPIC")

#----- define all individual modules to be built beforehand

set(MODULES core io elements beamline)

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

add_library(Hector SHARED $<TARGET_OBJECTS:HectorCore>
                          $<TARGET_OBJECTS:HectorIO>
                          $<TARGET_OBJECTS:HectorElements>
                          $<TARGET_OBJECTS:HectorBeamline>)
set_target_properties(Hector PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Hector CLHEP)
install(TARGETS Hector DESTINATION lib)

add_executable(hector main.cpp)
target_link_libraries(hector Hector)

file(GLOB input RELATIVE ${PROJECT_SOURCE_DIR} data/twiss/*)
foreach(_files ${input})
  configure_file(
    ${_files}
    ${_files}
    COPYONLY
)
endforeach()

