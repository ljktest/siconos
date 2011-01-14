SET(EXAMPLE @EXAMPLE@)


SET(CMAKE_SICONOS_COMPILE_OBJECT "${CMAKE_INSTALL_PREFIX}/bin/siconos --noexec <SOURCE> --build_dir <OBJECT>/siconos")
SET(CMAKE_SICONOS_LINK_EXECUTABLE "")

ENABLE_TESTING()

MACRO(ADD_EXAMPLE _N _EX)
  MESSAGE("Adding example ${_N} (${_EX})")
  ADD_TEST(${_N} siconos --nocomp ${_EX}.cpp)
  SET_TESTS_PROPERTIES(${_N} PROPERTIES FAIL_REGULAR_EXPRESSION "FAILURE;Exception;failed;ERROR")
ENDMACRO(ADD_EXAMPLE _N _EX)

IF(NOT EXAMPLE MATCHES Plugin)
  SET(TEST_ME TRUE)
  FOREACH(_NT ${NO_TEST_FILES})
    IF(EXAMPLE MATCHES "${_NT}")
      SET(TEST_ME FALSE)
    ENDIF(EXAMPLE MATCHES "${_NT}")
  ENDFOREACH(_NT ${NO_TEST_FILES})
  IF(TEST_ME)
    GET_FILENAME_COMPONENT(EXAMPLE_PATH ${EXAMPLE} PATH)
    GET_FILENAME_COMPONENT(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
    FILE(GLOB EXAMPLES_XML ${EXAMPLE_PATH}/*.xml)
    FOREACH(_F ${EXAMPLES_XML})
      GET_FILENAME_COMPONENT(EXAMPLE_XML ${_F} NAME)
      MESSAGE(STATUS "Found xml file : ${_F}")
      MESSAGE(STATUS "Configuring file : ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE_XML}")
      CONFIGURE_FILE(${_F} ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE_XML}  @COPYONLY)
    ENDFOREACH(_F ${EXAMPLES_XML})
    SET_SOURCE_FILES_PROPERTIES(${EXAMPLE} PROPERTIES LANGUAGE SICONOS)
    ADD_EXECUTABLE(${EXAMPLE_NAME} ${EXAMPLE})
    ADD_EXAMPLE(${EXAMPLE_NAME} ${EXAMPLE_NAME})
  ENDIF(TEST_ME)
ENDIF(NOT EXAMPLE MATCHES Plugin)
  
