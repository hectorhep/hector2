file(GLOB sources *Handler.cc)

include_directories(${PROJECT_SOURCE_DIR})

if(DEFINED PYTHIA8_FOUND)
  list(APPEND sources Pythia8Generator.cc)
endif()

#----- build the object

add_library(HectorIO OBJECT ${sources})
set_target_properties(HectorIO PROPERTIES LINKER_LANGUAGE CXX)
if(DEFINED PYTHIA8_FOUND)
  link_libraries(HectorIO Pythia8)
endif()
