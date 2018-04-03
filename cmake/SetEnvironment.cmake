#----- check if we are building from CERN's lxplus machines

if($ENV{HOSTNAME} MATCHES "^lxplus[0-9]+.cern.ch")
  set(LXPLUS 1)
endif()

#----- check the gcc/clang version

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic-errors -std=c++11 -g -fPIC")
else()
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic-errors -std=c++11 -g -fPIC")
  else()
    if(LXPLUS)
      message(FATAL_ERROR "gcc version >= 4.9 is required.\nYou may try to source:\n\t/cvmfs/sft.cern.ch/lcg/external/gcc/4.8.4/x86_64-slc6/setup.sh")
    else()
      message(FATAL_ERROR "gcc version >= 4.9 is required")
    endif()
  endif()
endif()

#----- CLHEP for physics

if(LXPLUS)
  set(BASE_DIR "/cvmfs/sft.cern.ch/lcg/external")
  find_library(CLHEP_LIB CLHEP REQUIRED HINTS "${BASE_DIR}/clhep/2.2.0.4/x86_64-slc6-gcc48-opt/lib")
  find_path(CLHEP_INCLUDE CLHEP HINTS "${BASE_DIR}/clhep/2.2.0.4/x86_64-slc6-gcc48-opt/include")
else()
  find_library(CLHEP_LIB CLHEP REQUIRED)
  find_path(CLHEP_INCLUDE CLHEP)
endif()

set(HECTOR_DEPENDENCIES ${CLHEP_LIB})
include_directories(${CLHEP_INCLUDE})

#----- Pythia 8 for physics samples generation and/or LHE files parsing

if(LXPLUS)
  set(BASE_DIR "/afs/cern.ch/sw/lcg/external")
  set(PYTHIA8_DIR "${BASE_DIR}/MCGenerators_lcgcmt67c/pythia8/230/x86_64-slc6-gcc48-opt/")
endif()
find_library(PYTHIA8 pythia8 HINTS $ENV{PYTHIA8_DIR} ${PYTHIA8_DIR} /usr /opt/pythia8 PATH_SUFFIXES lib)
find_path(PYTHIA8_INCLUDE Pythia8 HINTS $ENV{PYTHIA8_DIR} ${PYTHIA8_DIR} /usr /opt/pythia8 PATH_SUFFIXES include include/Pythia8 include/pythia8)

if(PYTHIA8)
  message(STATUS "Pythia8 found in ${PYTHIA8}")
  list(APPEND HECTOR_DEPENDENCIES ${PYTHIA8} dl)
  include_directories(${PYTHIA8_INCLUDE})
  find_library(LHAPDF LHAPDF)
  if(LHAPDF)
    list(APPEND HECTOR_DEPENDENCIES ${LHAPDF})
  else()
    list(APPEND HECTOR_DEPENDENCIES lhapdfdummy)
  endif()
endif()

#----- HepMC for I/O

find_library(HEPMC_LIB HepMC)
find_path(HEPMC_INCLUDE HepMC)

if(HEPMC_LIB)
  include_directories(${HEPMC_INCLUDE})
  list(APPEND HECTOR_DEPENDENCIES ${HEPMC_LIB})
  add_definitions(-DHEPMC)
endif()

#----- Boost for Python wrapper

find_package(Boost COMPONENTS python QUIET)

