file(GLOB sources *.cc)
file(GLOB headers *.h)

include_directories(${PROJECT_SOURCE_DIR})

#----- build the object

root_dictionary(PropagatorDict ${headers} LINKDEF LinkDef.hh)
add_library(HectorPropagator OBJECT ${sources} PropagatorDict.cxx)
set_target_properties(HectorPropagator PROPERTIES LINKER_LANGUAGE CXX)

