#----- check if we are building from CERN's lxplus machines

if($ENV{HOSTNAME} MATCHES "^lxplus[0-9]+.cern.ch")
  message(STATUS "Compiling on LXPLUS")
  set(LXPLUS TRUE)
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

#----- Eigen for physics

if(LXPLUS)
  set(BASE_DIR "/cvmfs/sft.cern.ch/lcg/external")
  find_library(CLHEP_LIB CLHEP REQUIRED HINTS "${BASE_DIR}/clhep/2.2.0.4/x86_64-slc6-gcc48-opt/lib")
  find_path(CLHEP_INCLUDE CLHEP HINTS "${BASE_DIR}/clhep/2.2.0.4/x86_64-slc6-gcc48-opt/include")
else()
  find_package(Eigen3 REQUIRED)
  message(STATUS "Eigen found in ${EIGEN3_INCLUDE_DIR}")
endif()

include_directories(${EIGEN3_INCLUDE_DIR})

#----- Pythia 8 for physics samples generation and/or LHE files parsing

if(LXPLUS)
  set(BASE_DIR "/afs/cern.ch/sw/lcg/external")
  set(PYTHIA8_DIR "${BASE_DIR}/MCGenerators_lcgcmt67c/pythia8/230/x86_64-slc6-gcc48-opt/")
endif()
set(PYTHIA8_DIRS $ENV{PYTHIA8_DIR} ${PYTHIA8_DIR} /usr /opt/pythia8)
find_library(PYTHIA8 pythia8 HINTS ${PYTHIA8_DIRS} PATH_SUFFIXES lib)
find_path(PYTHIA8_INCLUDE NAMES Pythia8/Pythia.h HINTS ${PYTHIA8_DIRS} PATH_SUFFIXES include include/Pythia8 include/pythia8)
find_library(LHAPDF LHAPDF)

#----- HepMC for I/O

find_library(HEPMC_LIB HepMC)
find_path(HEPMC_INCLUDE HepMC)

#----- Boost for Python wrapper

find_package(PythonInterp 3)
if(PYTHONINTERP_FOUND)
  find_package(PythonLibs 3)
  # stupid workaround for different behaviour between Fedora's and CC8's Boost CMake bindings
  find_package(Boost OPTIONAL_COMPONENTS python${PYTHON_VERSION_MAJOR} python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR})
  if(NOT ${Boost_python${PYTHON_VERSION_MAJOR}_FOUND})
    if (NOT ${Boost_python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}_FOUND})
      message(FATAL_ERROR "Boost Python binding not found")
    endif()
  endif()
endif()

