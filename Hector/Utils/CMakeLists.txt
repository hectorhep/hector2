file(GLOB sources *.cc)

include_directories(${PROJECT_SOURCE_DIR})

#----- build the object

add_library(HectorUtils OBJECT ${sources})
set_target_properties(HectorUtils PROPERTIES LINKER_LANGUAGE CXX)
