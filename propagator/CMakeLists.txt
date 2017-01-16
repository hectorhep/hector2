file(GLOB sources *.cc)
file(GLOB headers *.h)

include_directories(${PROJECT_SOURCE_DIR})

#----- build the object

add_library(HectorPropagator OBJECT ${sources})
set_target_properties(HectorPropagator PROPERTIES LINKER_LANGUAGE CXX)
