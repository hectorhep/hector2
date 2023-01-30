#--- searching for ROOT
find_package(ROOT QUIET)

if(NOT ${ROOT_FOUND})
  return()
endif()

hector_build(HectorROOT SOURCES *.cc
    EXT_LIBS ${ROOT_LIBRARIES}
    EXT_HEADERS ${ROOT_INCLUDE_DIRS}
    DEFINITIONS ${ROOT_DEFINITIONS}
    TESTS test/*.cc
    UTILS utils/*.cc
    INSTALL_COMPONENT root)
