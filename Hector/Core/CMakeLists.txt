file(GLOB sources *.cc)
file(GLOB headers *.h)

include_directories(${PROJECT_SOURCE_DIR})

#----- build the object

root_dictionary(CoreDict ${headers} LINKDEF LinkDef.hh)
add_library(HectorCore OBJECT ${sources} CoreDict.cxx)
set_target_properties(HectorCore PROPERTIES LINKER_LANGUAGE CXX)

