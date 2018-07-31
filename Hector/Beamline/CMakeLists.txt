file(GLOB sources *.cc)
file(GLOB headers *.h)

include_directories(${PROJECT_SOURCE_DIR})

#----- build the object

root_dictionary(BeamlineDict ${headers} LINKDEF LinkDef.hh)
add_library(HectorBeamline OBJECT ${sources} BeamlineDict.cxx)
set_target_properties(HectorBeamline PROPERTIES LINKER_LANGUAGE CXX)

