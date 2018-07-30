file(GLOB sources *.cc)

include_directories(${PROJECT_SOURCE_DIR})

#----- include the Pythia8 wrapper if present on the system

if(PYTHIA8)
  include_directories(${PYTHIA8_INCLUDE})
  add_definitions(-DPYTHIA8)
else()
  file(GLOB py8_sources Pythia8Generator.cc)
  list(REMOVE_ITEM sources ${py8_sources})
endif()

#----- build the object

add_library(HectorUtils OBJECT ${sources})
set_target_properties(HectorUtils PROPERTIES LINKER_LANGUAGE CXX)
