file(GLOB sources *.cc)
include_directories(${PROJECT_SOURCE_DIR})

#----- build the object

add_library(HectorIO OBJECT ${sources})
set_target_properties(HectorIO PROPERTIES LINKER_LANGUAGE CXX)

