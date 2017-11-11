#----- CLHEP for physics

find_package(CLHEP REQUIRED)
set(HECTOR_DEPENDENCIES CLHEP)

#----- Pythia 8 for physics samples generation

find_package(Pythia8 QUIET)

if(PYTHIA8_FOUND)
  list(APPEND HECTOR_DEPENDENCIES ${PYTHIA8_LIBRARY})
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
