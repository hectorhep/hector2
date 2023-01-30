#--- searching for Pythia 8
set(PYTHIA8_DIRS $ENV{PYTHIA8_DIR} /usr /usr/local /opt/pythia8)
find_library(PYTHIA8 pythia8 HINTS ${PYTHIA8_DIRS} PATH_SUFFIXES lib)
find_path(PYTHIA8_INCLUDE Pythia8 HINTS ${PYTHIA8_DIRS} PATH_SUFFIXES include/Pythia8 include/pythia8 include)

if(NOT PYTHIA8)
  return()
endif()

hector_build(HectorPythia8 SOURCES *.cc
    EXT_LIBS ${PYTHIA8} dl HectorROOT
    EXT_HEADERS ${PYTHIA8_INCLUDE}
    TESTS test/*.cc
    OPTIONS "-Wno-misleading-indentation"
    INSTALL_COMPONENT pythia8)
