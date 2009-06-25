#
# Some convenience macros
#

# Basic list manipulation
MACRO(CAR var)
  SET(${var} ${ARGV1})
ENDMACRO(CAR)

MACRO(CDR var junk)
  SET(${var} ${ARGN})
ENDMACRO(CDR)

# LIST(APPEND ...) is not correct on <COMPILER>_FLAGS 
MACRO(APPEND_FLAGS)
  CAR(_V ${ARGV})
  CDR(_F ${ARGV})
  SET(${_V} "${${_V}} ${_F}")
ENDMACRO(APPEND_FLAGS)

# The use of ADD_DEFINITION results in a warning with Fortran compiler
MACRO(APPEND_C_FLAGS)
  APPEND_FLAGS(CMAKE_C_FLAGS ${ARGV})
ENDMACRO(APPEND_C_FLAGS)

MACRO(APPEND_CXX_FLAGS)
  APPEND_FLAGS(CMAKE_CXX_FLAGS ${ARGV})
ENDMACRO(APPEND_CXX_FLAGS)

MACRO(APPEND_Fortran_FLAGS)
  APPEND_FLAGS(CMAKE_Fortran_FLAGS ${ARGV})
ENDMACRO(APPEND_Fortran_FLAGS)

# Do them once and remember the values for other projects (-> tests)
MACRO(REMEMBER_INCLUDE_DIRECTORIES _DIRS)
  FOREACH(_D ${_DIRS})
    IF(NOT ${PROJECT_NAME}_REMEMBER_INC_${_D})
      SET(${PROJECT_NAME}_REMEMBER_INC_${_D} TRUE)
      LIST(APPEND ${PROJECT_NAME}_INCLUDE_DIRECTORIES ${_D})
      INCLUDE_DIRECTORIES(${_DIRS})
    ENDIF(NOT ${PROJECT_NAME}_REMEMBER_INC_${_D})
  ENDFOREACH(_D ${_DIRS})
ENDMACRO(REMEMBER_INCLUDE_DIRECTORIES _DIRS)

MACRO(REMEMBER_LINK_DIRECTORIES _DIRS)
  FOREACH(_D ${_DIRS})
    IF(NOT ${PROJECT_NAME}_REMEMBER_LINK_${_D})
      SET(${PROJECT_NAME}_REMEMBER_LINK_${_D} TRUE)
      LIST(APPEND ${PROJECT_NAME}_LINK_DIRECTORIES ${_D})
      LINK_DIRECTORIES(${_D})
    ENDIF(NOT ${PROJECT_NAME}_REMEMBER_LINK_${_D})
  ENDFOREACH(_D ${_DIRS})
ENDMACRO(REMEMBER_LINK_DIRECTORIES _DIRS)

MACRO(REMEMBER_LINK_LIBRARIES _LIBS)
  FOREACH(_LIB ${_LIBS})
    IF(NOT ${PROJECT_NAME}_REMEMBER_LINK_LIBRARIES_${_LIB})
      SET(${PROJECT_NAME}_REMEMBER_LINK_LIBRARIES_${_LIB} TRUE)
      LIST(APPEND ${PROJECT_NAME}_LINK_LIBRARIES ${_LIB})
    ENDIF(NOT ${PROJECT_NAME}_REMEMBER_LINK_LIBRARIES_${_LIB})
  ENDFOREACH(_LIB ${_LIBS})
ENDMACRO(REMEMBER_LINK_LIBRARIES _LIBS)

# link/include is done in the BLAS, LAPACK macros, but not in others.
MACRO(COMPILE_WITH)
  CAR(_NAME ${ARGV})
  CDR(_REST ${ARGV})
  CAR(_REQ ${_REST})
  IF(_REST)
    CDR(_RREST ${_REST})
    CAR(_COMP ${_RREST})
  ENDIF(_REST)
  STRING(TOUPPER ${_NAME} _UNAME)
  LIST(APPEND _NAMES ${_NAME})
  LIST(APPEND _NAMES ${_UNAME})
  SET(_FOUND)
  IF(_REQ STREQUAL REQUIRED)
    FIND_PACKAGE(${_NAME} REQUIRED)
  ELSE(_REQ STREQUAL REQUIRED)
    FIND_PACKAGE(${ARGV})
  ENDIF(_REQ STREQUAL REQUIRED)
  FOREACH(_N ${_NAMES})
    IF(${_N}_FOUND)
      SET(_FOUND TRUE)
      IF(${_N}_INCLUDE_DIRS)
        REMEMBER_INCLUDE_DIRECTORIES("${${_N}_INCLUDE_DIRS}")
      ENDIF(${_N}_INCLUDE_DIRS)
      IF(${_N}_INCLUDE_DIR)
        REMEMBER_INCLUDE_DIRECTORIES("${${_N}_INCLUDE_DIR}")
      ENDIF(${_N}_INCLUDE_DIR)
      IF(${_N}_INCLUDE_PATH)
        REMEMBER_INCLUDE_DIRECTORIES("${${_N}_INCLUDE_PATH}")
      ENDIF(${_N}_INCLUDE_PATH)
      IF(${_N}_LIBRARY_DIRS)
        REMEMBER_LINK_DIRECTORIES("${${_N}_LIBRARY_DIRS}")
      ENDIF(${_N}_LIBRARY_DIRS)
      IF(${_N}_LIBRARIES)
        REMEMBER_LINK_LIBRARIES("${${_N}_LIBRARIES}")
      ENDIF(${_N}_LIBRARIES)
      IF(_COMP STREQUAL COMPLETE)
        IF(COMPLETE_${_N}_LIBRARIES)	
          REMEMBER_LINK_LIBRARIES("${COMPLETE_${_N}_LIBRARIES}")
        ENDIF(COMPLETE_${_N}_LIBRARIES)
      ENDIF(_COMP STREQUAL COMPLETE)
      IF(${_N}_DEFINITIONS) # not Fortran is supposed
        APPEND_C_FLAGS(${${_N}_DEFINITIONS})
        APPEND_CXX_FLAGS(${${_N}_DEFINITIONS})
      ENDIF(${_N}_DEFINITIONS)
    ENDIF(${_N}_FOUND)
  ENDFOREACH(_N ${_NAME} ${_UNAME})

  IF(_REQ STREQUAL REQUIRED)
    IF(_FOUND)
    ELSE(_FOUND)
      MESSAGE(FATAL_ERROR "${_NAME} NOT FOUND")
    ENDIF(_FOUND)
  ENDIF(_REQ STREQUAL REQUIRED)

  # update NumericsConfig.h/KernelConfig.h
  IF(NOT CONFIG_H_${_NAME}_CONFIGURED)
    SET(CONFIG_H_${_NAME}_CONFIGURED 1 CACHE BOOL 
      "${PROJECT_SHORT_NAME}Config.h generation for package ${_NAME}")
    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.cmake 
      ${CMAKE_BINARY_DIR}/${PROJECT_SHORT_NAME}Config.h)
  ENDIF(NOT CONFIG_H_${_NAME}_CONFIGURED)
  SET(_N)
  SET(_NAME) 
  SET(_UNAME)
  SET(_NAMES)
ENDMACRO(COMPILE_WITH)

# Tests
MACRO(BEGIN_TEST _D)
  SET(_CURRENT_TEST_DIRECTORY ${_D})
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${_D})
  
  # find and copy data files : *.mat, *.dat and *.xml, and etc.
  FILE(GLOB_RECURSE _DATA_FILES 
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${_D}
    *.mat 
    *.dat
    *.xml
    *.DAT
    *.INI)
    
  FOREACH(_F ${_DATA_FILES})
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${_D}/${_F} ${CMAKE_CURRENT_BINARY_DIR}/${_D}/${_F} COPYONLY)
  ENDFOREACH(_F ${_DATA_FILES})
  
  # configure test CMakeLists.txt (needed for a chdir before running test)
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/CMakeListsForTests.cmake 
    ${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY}/CMakeLists.txt @ONLY)

  SET(_EXE_LIST_${_CURRENT_TEST_DIRECTORY})
ENDMACRO(BEGIN_TEST _D)

# Declaration of a siconos test
MACRO(NEW_TEST)
  CAR(_EXE ${ARGV})
  CDR(_SOURCES ${ARGV})

  LIST(APPEND _EXE_LIST_${_CURRENT_TEST_DIRECTORY} ${_EXE})
  SET(${_EXE}_FSOURCES)
  FOREACH(_F ${_SOURCES})
    LIST(APPEND ${_EXE}_FSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${_CURRENT_TEST_DIRECTORY}/${_F})
  ENDFOREACH(_F ${_SOURCES})

 
  IF(TEST_MAIN)
    LIST(APPEND ${_EXE}_FSOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_MAIN})
  ENDIF(TEST_MAIN)

  
  # pb env in ctest, see http://www.vtk.org/Bug/view.php?id=6391#bugnotes
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/ldwrap.c.in 
    ${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY}/${_EXE}.ldwrap.c)
  
ENDMACRO(NEW_TEST)

MACRO(END_TEST)
  ADD_SUBDIRECTORY(${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY})
ENDMACRO(END_TEST)


# to prevent the reference of inside sources directories 
MACRO(CHECK_INSTALL_INCLUDE_DIRECTORIES)
  SET(CHECKED_${PROJECT_NAME}_INCLUDE_DIRECTORIES)
  FOREACH(_D ${${PROJECT_NAME}_INCLUDE_DIRECTORIES})
    IF(_D MATCHES "${CMAKE_SOURCE_DIR}")
    ELSE(_D MATCHES "${CMAKE_SOURCE_DIR}")
      LIST(APPEND CHECKED_${PROJECT_NAME}_INCLUDE_DIRECTORIES ${_D})
    ENDIF(_D MATCHES "${CMAKE_SOURCE_DIR}")
  ENDFOREACH(_D ${${PROJECT_NAME}_INCLUDE_DIRECTORIES})
ENDMACRO(CHECK_INSTALL_INCLUDE_DIRECTORIES)

# debug
MACRO(PRINT_VAR V)
  MESSAGE(STATUS "${V} = ${${V}}")
ENDMACRO(PRINT_VAR V)
