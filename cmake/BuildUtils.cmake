set(lib_name Hector2)

macro(hector_build mod_name)
    set(options)
    set(one_val)
    set(multi_vals CFLAGS
                   DEFINITIONS
                   DEPENDS
                   EXT_LIBS
                   EXT_HEADERS
                   EXT_BINS
                   INSTALL_COMPONENT
                   OPTIONS
                   PROPERTY
                   SOURCES
                   TESTS
                   UTILS)
    cmake_parse_arguments(ARG "${options}" "${one_val}" "${multi_vals}" ${ARGN})
    message(STATUS "Building ${mod_name} add-on")
    if(ARG_SOURCES)
      set(sources)
      foreach(_s ${ARG_SOURCES})
        file(GLOB s "${_s}")
        list(APPEND sources ${s})
      endforeach()
      add_library(${mod_name} SHARED ${sources})
      if(NOT("${mod_name}" STREQUAL "${lib_name}"))
        target_link_libraries(${mod_name} ${lib_name})
      endif()
      set(tests)
      if(ARG_TESTS AND BUILD_TESTS)
        file(GLOB tt "${ARG_TESTS}")
        foreach(_t ${tt})
          get_filename_component(bin ${_t} NAME_WE)
          add_executable(${bin} "${_t}")
          add_test(NAME ${bin} COMMAND ${bin})
          target_link_libraries(${bin} ${lib_name} ${mod_name})
          list(APPEND tests ${bin})
        endforeach()
        message(STATUS "... test(s): ${tests}")
      endif()
      set(utils)
      if(ARG_UTILS)
        file(GLOB uu "${ARG_UTILS}")
        foreach(_u ${uu})
          get_filename_component(bin ${_u} NAME_WE)
          add_executable(${bin} "${_u}")
          set_target_properties(${bin} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
          add_test(NAME ${bin} COMMAND ${bin})
          target_link_libraries(${bin} ${lib_name} ${mod_name})
          list(APPEND utils ${bin})
        endforeach()
        message(STATUS "... utility tool(s): ${utils}")
      endif()
    if(ARG_EXT_LIBS)
      message(STATUS "... external libraries: ${ARG_EXT_LIBS}")
      target_link_libraries(${mod_name} ${ARG_EXT_LIBS})
      foreach(_t ${tests})
        target_link_libraries(${_t} ${ARG_EXT_LIBS})
      endforeach()
      foreach(_t ${utils})
        target_link_libraries(${_t} ${ARG_EXT_LIBS})
      endforeach()
    endif()
    if(ARG_EXT_HEADERS)
      message(STATUS "... external headers: ${ARG_EXT_HEADERS}")
      target_include_directories(${mod_name} PUBLIC ${ARG_EXT_HEADERS})
      foreach(_t ${tests})
        target_include_directories(${_t} PRIVATE ${ARG_EXT_HEADERS})
      endforeach()
      foreach(_t ${utils})
        target_include_directories(${_t} PRIVATE ${ARG_EXT_HEADERS})
      endforeach()
    endif()
    if(ARG_DEPENDS)
      message(STATUS "... external dependencies: ${ARG_DEPENDS}")
      foreach(_dep ${ARG_DEPENDS})
        file(GLOB dep ${_dep})
        if(NOT dep)
          message(FATAL_ERROR "Failed to locate ${_dep}!")
        endif()
      endforeach()
    endif()
    if(ARG_PROPERTY)
      message(STATUS "... additional properties: ${ARG_PROPERTY}")
      set_property(TARGET ${mod_name} PROPERTY ${ARG_PROPERTY})
      foreach(_t ${tests})
        set_target_properties(${_t} PROPERTIES ${ARG_PROPERTY})
      endforeach()
      foreach(_t ${utils})
        set_target_properties(${_t} PROPERTIES ${ARG_PROPERTY})
      endforeach()
    endif()
    if(ARG_DEFINITIONS)
      message(STATUS "... compile definitions: ${ARG_DEFINITIONS}")
      target_compile_definitions(${mod_name} PRIVATE ${ARG_DEFINITIONS})
      foreach(_t ${tests})
        target_compile_definitions(${_t} PRIVATE ${ARG_DEFINITIONS})
      endforeach()
      foreach(_t ${utils})
        target_compile_definitions(${_t} PRIVATE ${ARG_DEFINITIONS})
      endforeach()
    endif()
    if(ARG_OPTIONS)
      message(STATUS "... compile options: ${ARG_OPTIONS}")
      target_compile_options(${mod_name} PRIVATE ${ARG_OPTIONS})
      foreach(_t ${tests})
        target_compile_options(${_t} PRIVATE ${ARG_OPTIONS})
      endforeach()
      foreach(_t ${utils})
        target_compile_options(${_t} PRIVATE ${ARG_OPTIONS})
      endforeach()
    endif()
    if(ARG_CFLAGS)
      message(STATUS "... compile flags: ${ARG_CFLAGS}")
      set_target_properties(${mod_name} PROPERTIES COMPILE_FLAGS ${ARG_CFLAGS})
      foreach(_t ${tests})
        set_target_properties(${_t} PROPERTIES COMPILE_FLAGS ${ARG_CFLAGS})
      endforeach()
      foreach(_t ${utils})
        set_target_properties(${_t} PROPERTIES COMPILE_FLAGS ${ARG_CFLAGS})
      endforeach()
    endif()
  else() # interfacing library (only includes)
      add_library(${mod_name} INTERFACE)
  endif()
  target_include_directories(${mod_name} INTERFACE ${PROJECT_SOURCE_DIR})
  if(ARG_INSTALL_COMPONENT)
    #----- installation rules
    install(TARGETS ${mod_name}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            COMPONENT ${ARG_INSTALL_COMPONENT})
    install(DIRECTORY ./
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${mod_name}
            COMPONENT ${ARG_INSTALL_COMPONENT}
            FILES_MATCHING PATTERN "*.h")
    foreach(_t ${utils})
      install(TARGETS ${_t}
              DESTINATION ${CMAKE_INSTALL_BINDIR}/${mod_name}
              COMPONENT ${ARG_INSTALL_COMPONENT})
    endforeach()
  endif()
endmacro()
