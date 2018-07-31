file(GLOB sources *.cc)
file(GLOB headers *.h)

include_directories(${PROJECT_SOURCE_DIR})

#----- build the object

root_dictionary(ElementsDict ${headers} LINKDEF LinkDef.hh)
add_library(HectorElements OBJECT ${sources} ElementsDict.cxx)
set_target_properties(HectorElements PROPERTIES LINKER_LANGUAGE CXX)

