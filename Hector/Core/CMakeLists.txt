file(GLOB sources *.cc)
file(GLOB headers *.h)

include_directories(${PROJECT_SOURCE_DIR})

#----- build the object

add_library(HectorCore OBJECT ${sources})
set_target_properties(HectorCore PROPERTIES LINKER_LANGUAGE CXX)

#----- build the ROOT dictionary

root_dictionary(CoreDict ${headers} LINKDEF LinkDef.hh)
list(APPEND sources CoreDict.cxx)
add_library(HectorCoreROOT SHARED ${sources} CoreDict.cxx)
target_link_libraries(HectorCoreROOT CLHEP ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})
