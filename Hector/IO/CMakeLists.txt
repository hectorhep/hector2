file(GLOB sources *.cc)
include_directories(${PROJECT_SOURCE_DIR})

#----- remove the LHEF parser if the required include is not present

file(GLOB lhef_sources LHEHandler.cc)
if(HEPMC_LIB)
else()
  list(REMOVE_ITEM sources ${lhef_sources})
endif()

#----- build the object

add_library(HectorIO OBJECT ${sources})
set_target_properties(HectorIO PROPERTIES LINKER_LANGUAGE CXX)

