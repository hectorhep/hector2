cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(Hector)
set(PROJECT_VERSION 2)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/cmake)

#----- include external dependencies, prepare the environment

include(SetEnvironment)

#----- define all individual modules to be built beforehand

set(HECTOR_MODULES Core IO Elements Beamline Propagator Utils)
set(HECTOR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Hector)

#----- build all the intermediate objects

include_directories(${PROJECT_SOURCE_DIR})
foreach(_module ${HECTOR_MODULES})
  include_directories(${HECTOR_SOURCE_DIR}/${_module})
  add_subdirectory(Hector/${_module})
endforeach()

#----- link everything into the library and executable

# do not forget to retrieve
# http://home.thep.lu.se/~leif/LHEF/LHEF.h

add_library(Hector2 SHARED $<TARGET_OBJECTS:HectorCore>
                           $<TARGET_OBJECTS:HectorIO>
                           $<TARGET_OBJECTS:HectorElements>
                           $<TARGET_OBJECTS:HectorBeamline>
                           $<TARGET_OBJECTS:HectorPropagator>
                           $<TARGET_OBJECTS:HectorUtils>)
set_target_properties(Hector2 PROPERTIES LINKER_LANGUAGE CXX)

install(TARGETS Hector2 DESTINATION lib)
install(DIRECTORY Hector DESTINATION include FILES_MATCHING PATTERN "*.h")

#----- RPM packaging

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME "Hector")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Laurent Forthomme")
set(CPACK_PACKAGE_VENDOR "CERN")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}.${CMAKE_SYSTEM_PROCESSOR}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/hector.spec.in" "${CMAKE_CURRENT_BINARY_DIR}/hector.spec" @ONLY IMMEDIATE)
set(CPACK_RPM_USER_BINARY_SPECFILE "${CMAKE_CURRENT_BINARY_DIR}/hector.spec")
include(CPack)

#----- test executable

add_executable(hector test/test_hector.cpp)
target_link_libraries(hector Hector2 ${HECTOR_DEPENDENCIES})

#----- python wrapper

if(Boost_FOUND)
  find_package(PythonLibs 2.7 REQUIRED)
  include_directories(${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
  PYTHON_ADD_MODULE(pyhector ${HECTOR_SOURCE_DIR}/PythonWrapper.cc)
  target_link_libraries(pyhector Hector2 ${HECTOR_DEPENDENCIES} ${Boost_LIBRARIES})
  #install(TARGETS pyhector DESTINATION ${PYTHON_INSTALL_PATH})
endif()

#----- add the tests

enable_testing()
add_subdirectory(test)

#----- copy the data files

file(GLOB input RELATIVE ${PROJECT_SOURCE_DIR} data/twiss/*)
foreach(_files ${input})
  configure_file(${_files} ${_files} COPYONLY)
endforeach()

