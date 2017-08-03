file(GLOB sources *.cc)
file(GLOB headers *.h)

include_directories(${PROJECT_SOURCE_DIR})

#----- build the object

add_library(HectorPropagator OBJECT ${sources})
set_target_properties(HectorPropagator PROPERTIES LINKER_LANGUAGE CXX)

#----- build the ROOT dictionary

root_dictionary(PropagatorDict ${headers} LINKDEF LinkDef.hh)
list(APPEND sources PropagatorDict.cxx)
add_library(HectorPropagatorROOT SHARED ${sources} PropagatorDict.cxx)
target_link_libraries(HectorPropagatorROOT CLHEP ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})
