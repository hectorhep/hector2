cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -g")

#----- define all individual modules to be built beforehand

set(MODULES core elements)

#----- enable fortran (for Pythia/Herwig/...)

enable_language(Fortran OPTIONAL)

#----- include external hadronisers/...

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

add_library(Hector SHARED $<TARGET_OBJECTS:HectorCore> $<TARGET_OBJECTS:HectorElements>)
set_target_properties(Hector PROPERTIES LINKER_LANGUAGE CXX)

add_executable(hector main.cpp)
target_link_libraries(hector Hector)

