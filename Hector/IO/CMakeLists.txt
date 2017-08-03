file(GLOB sources *Parser.cc)
file(GLOB headers *Parser.h)

include_directories(${PROJECT_SOURCE_DIR})

find_package(Pythia8 QUIET)
if(DEFINED PYTHIA8_FOUND)
  list(APPEND sources Pythia8Generator.cc)
  list(APPEND headers Pythia8Generator.h)
#  message(STATUS ${sources})
endif()

#----- build the object

add_library(HectorIO OBJECT ${sources})
set_target_properties(HectorIO PROPERTIES LINKER_LANGUAGE CXX)
if(DEFINED PYTHIA8_FOUND)
  link_libraries(HectorIO Pythia8)
endif()
