file(GLOB sources *.cc)

include_directories(${PROJECT_SOURCE_DIR})

#----- remove the LHEF parser if the required include is not present

if(HEPMC_LIB)
else()
  file(GLOB lhef_sources LHEHandler.cc)
  list(REMOVE_ITEM sources ${lhef_sources})
endif()

#----- include the Pythia8 wrapper if present on the system

if(PYTHIA8_FOUND)
  include_directories(${PYTHIA8_INCLUDE_DIRS})
else()
  file(GLOB py8_sources Pythia8Generator.cc)
  list(REMOVE_ITEM sources ${py8_sources})
endif()

#----- build the object

add_library(HectorUtils OBJECT ${sources})
set_target_properties(HectorUtils PROPERTIES LINKER_LANGUAGE CXX)
