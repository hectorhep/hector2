#----- list all test executables

file(GLOB executables_noroot RELATIVE ${PROJECT_SOURCE_DIR}/test *.cpp)
file(GLOB executables_root RELATIVE ${PROJECT_SOURCE_DIR}/test *.cxx)

#----- build all tests and link them to the core library

foreach(exec_src ${executables_noroot})
    string(REGEX REPLACE "(test_|.cpp)" "" exec_bin ${exec_src})
    add_executable(${exec_bin} ${exec_src})
    set_target_properties(${exec_bin} PROPERTIES EXCLUDE_FROM_ALL true)
    target_link_libraries(${exec_bin} Hector2 ${HECTOR_DEPENDENCIES})
    add_test(NAME ${exec_bin} COMMAND ${exec_bin})
#    set_tests_properties(${exec_bin} PROPERTIES PASS_REGULAR_EXPRESSION "Passed")
endforeach()

#----- specify the tests requiring ROOT

find_package(ROOT QUIET)
if(${ROOT_FOUND})
    message(STATUS "ROOT found in ${ROOT_INCLUDE_DIRS}")
    include_directories(${ROOT_INCLUDE_DIRS})
    link_directories(${ROOT_LIBRARY_DIR})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

    foreach(exec_src ${executables_root})
        string(REGEX REPLACE "(test_|.cxx)" "" exec_bin ${exec_src})
        add_executable(${exec_bin} ${exec_src})
        set_target_properties(${exec_bin} PROPERTIES EXCLUDE_FROM_ALL true)
        target_link_libraries(${exec_bin} Hector2 ${HECTOR_DEPENDENCIES} ${ROOT_LIBRARIES})
        add_test(NAME ${exec_bin} COMMAND ${exec_bin})
    endforeach()
else()
    message(STATUS "ROOT not found! skipping these tests!")
endif()

file(GLOB py_tests RELATIVE ${PROJECT_SOURCE_DIR}/test *.py)
foreach(_files ${py_tests})
  configure_file(${_files} ${_files} COPYONLY)
endforeach()

