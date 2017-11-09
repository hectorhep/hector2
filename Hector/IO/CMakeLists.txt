file(GLOB sources *Handler.cc)
include_directories(${PROJECT_SOURCE_DIR})

#----- remove the LHEF parser if the required include is not present

file(GLOB lhef_sources LHEHandler.cc)
file(GLOB lhef_include LHEF.h)
if(NOT EXISTS ${lhef_include})
  list(REMOVE_ITEM sources ${lhef_sources})
endif()

#----- include the Pythia8 wrapper if present on the system

if(PYTHIA8_FOUND)
  include_directories(${PYTHIA8_INCLUDE_DIRS})
  list(APPEND sources Pythia8Generator.cc)
endif()

#----- build the object

add_library(HectorIO OBJECT ${sources})
set_target_properties(HectorIO PROPERTIES LINKER_LANGUAGE CXX)
if(PYTHIA8_FOUND)
  link_libraries(HectorIO ${PYTHIA8_LIBRARIES})
endif()
