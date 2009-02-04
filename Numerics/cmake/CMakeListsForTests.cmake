# -*- cmake -*-
# This is the test cmake configuration
# built from @CMAKE_SOURCE_DIR@/cmake/CMakeListsForTests.cmake.in 

IF(FORTRAN_COMPILER_LIB_DIRECTORIES)
  LINK_DIRECTORIES(${FORTRAN_COMPILER_LIB_DIRECTORIES})
ENDIF(FORTRAN_COMPILER_LIB_DIRECTORIES)

FOREACH(_EXE ${_EXE_LIST_${_CURRENT_TEST_DIRECTORY}})

  MESSAGE(STATUS "Adding test ${_CURRENT_TEST_DIRECTORY}/${_EXE}")

  # a wrapper around test
  IF(TEST_WRAP)
    ADD_EXECUTABLE(${_EXE}.ldwrap ${_EXE}.ldwrap.c)
  ENDIF(TEST_WRAP)

  ADD_EXECUTABLE(${_EXE} ${${_EXE}_FSOURCES})

  SET(EXECUTABLE_OUTPUT_PATH @CMAKE_CURRENT_BINARY_DIR@/@_CURRENT_TEST_DIRECTORY@)

  FOREACH(_D ${${PROJECT_NAME}_INCLUDE_DIRECTORIES})
    INCLUDE_DIRECTORIES(${_D})
  ENDFOREACH(_D ${${PROJECT_NAME}_INCLUDE_DIRECTORIES}) 
  FOREACH(_D ${${PROJECT_NAME}_LINK_DIRECTORIES})
    LINK_DIRECTORIES(${_D})
  ENDFOREACH(_D ${${PROJECT_NAME}_LINK_DIRECTORIES})

  # project name shared or static lib
  IF(${PROJECT_NAME}_SHARED_LIB)
    ADD_DEPENDENCIES(${_EXE} ${PROJECT_NAME}_shared)
    TARGET_LINK_LIBRARIES(${_EXE} ${${PROJECT_NAME}_SHARED_LIB})
  ELSE(${PROJECT_NAME}_SHARED_LIB)
    IF(${PROJECT_NAME}_STATIC_LIB)
      ADD_DEPENDENCIES(${_EXE} ${PROJECT_NAME}_static)
      TARGET_LINK_LIBRARIES(${_EXE} ${${PROJECT_NAME}_STATIC_LIB})
    ENDIF(${PROJECT_NAME}_STATIC_LIB)
  ENDIF(${PROJECT_NAME}_SHARED_LIB)
  FOREACH(_L ${${PROJECT_NAME}_LINK_LIBRARIES})
    TARGET_LINK_LIBRARIES(${_EXE} ${_L})
  ENDFOREACH(_L ${${PROJECT_NAME}_TARGET_LINK_LIBRARIES})
  IF(FORTRAN_LIBRARIES)
    TARGET_LINK_LIBRARIES(${_EXE} ${FORTRAN_LIBRARIES})
  ENDIF(FORTRAN_LIBRARIES)

  IF(TEST_WRAP)
    ADD_TEST(${_EXE} ${_EXE}.ldwrap)
  ELSE(TEST_WRAP)
    ADD_TEST(${_EXE} ${_EXE})
  ENDIF(TEST_WRAP)

  IF(${_EXE}_PROPERTIES)
    SET_TESTS_PROPERTIES(${_EXE} PROPERTIES ${${_EXE}_PROPERTIES})
  ENDIF(${_EXE}_PROPERTIES)

  # a brutal way to handle ccpunit assertions failures
  IF(CPPUNIT_FOUND)
    SET_TESTS_PROPERTIES(${_EXE} PROPERTIES FAIL_REGULAR_EXPRESSION "FAILURE")
  ENDIF(CPPUNIT_FOUND)

ENDFOREACH(_EXE ${_EXE_LIST_${_CURRENT_TEST_DIRECTORY}})
