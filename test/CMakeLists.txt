if(NOT ${BUILD_TESTS})
  return()
endif()

#----- build all tests and link them to the core library

file(GLOB executables_noroot RELATIVE ${HECTOR_TEST_DIR} *.cc)
foreach(exec_src ${executables_noroot})
    string(REGEX REPLACE "(test_|.cc)" "" exec_bin ${exec_src})
    add_executable(${exec_bin} ${exec_src})
    target_link_libraries(${exec_bin} Hector2 ${HECTOR_DEPENDENCIES})
    add_test(NAME ${exec_bin} COMMAND ${exec_bin})
#    set_tests_properties(${exec_bin} PROPERTIES PASS_REGULAR_EXPRESSION "Passed")
endforeach()

file(GLOB py_tests RELATIVE ${PROJECT_SOURCE_DIR}/test *.py)
foreach(_files ${py_tests})
  configure_file(${_files} ${_files} COPYONLY)
endforeach()

