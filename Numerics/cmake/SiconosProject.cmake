#
# Common setup
#

# before everything
CMAKE_MINIMUM_REQUIRED(VERSION 2.4.4)

# An encourage to out of source builds 
INCLUDE(OutOfSourcesBuild)

MACRO(SICONOS_PROJECT 
    _PROJECT_NAME 
    MAJOR_VERSION MINOR_VERSION PATCH_VERSION
    _PACKAGE_INFO)
  
  STRING(TOLOWER _PROJECT_NAME _LPROJECT_NAME)
  
  SET(PROJECT_SHORT_NAME ${_PROJECT_NAME})
  
  SET(PROJECT_PACKAGE_NAME "siconos-${_LPROJECT_NAME}")
  
  # PACKAGE PROJECT SETUP
  PROJECT(${PROJECT_PACKAGE_NAME})

  SET(VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")  
  
  # try to get the SVN revision number
  INCLUDE(SVNRevisionNumber)

  # Some macros needed to check compilers environment
  INCLUDE(CheckSymbolExists)
  INCLUDE(CheckFunctionExists)


  # Compilers environment
  IF(CMAKE_C_COMPILER)
    INCLUDE(CheckCCompilerFlag)
    CHECK_C_COMPILER_FLAG("-std=c99" C_HAVE_C99)
  ENDIF(CMAKE_C_COMPILER)

  IF(CMAKE_CXX_COMPILER)
    INCLUDE(TestCXXAcceptsFlag)
    CHECK_CXX_ACCEPTS_FLAG(-ffriend-injection CXX_HAVE_FRIEND_INJECTION)
  ENDIF(CMAKE_CXX_COMPILER)

  IF(CMAKE_Fortran_COMPILER)
    INCLUDE(fortran)
    INCLUDE(FortranLibraries)
    INCLUDE(LibraryProjectSetup)
  ENDIF(CMAKE_Fortran_COMPILER)



  # Tests+Dashboard configuration
  ENABLE_TESTING()
  INCLUDE(DartConfig)
  INCLUDE(Dart)
  
  # The library build stuff
  INCLUDE(LibraryProjectSetup)
  
  # Doxygen documentation
  INCLUDE(SiconosDoc)

  # config.h and include
  CONFIGURE_FILE(config.h.cmake config.h)
  INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

  # Top level install
  SET(CMAKE_INCLUDE_CURRENT_DIR ON)
  INSTALL(FILES AUTHORS ChangeLog COPYING INSTALL README 
    DESTINATION share/doc/siconos-${VERSION}/${_PROJECT_NAME})

  # man files
  IF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/man)
    FILE(GLOB MAN_FILES man/*.?)
    IF(MAN_FILES)
      FOREACH(_FILE ${MAN_FILES})
        GET_FILENAME_COMPONENT(_EXT ${_FILE} EXT)
        INSTALL(FILES ${_FILE} DESTINATION man/man${_EXT}/${_FILE})
      ENDFOREACH(_FILE ${MAN_FILES})
    ENDIF(MAN_FILES)
  ENDIF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/man)

  # xml
  IF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/config/xmlschema)
    FILE(GLOB _SFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} config/xmlschema/*.xsd)
    FOREACH(_F ${_SFILES})
      CONFIGURE_FILE(${_F} ${CMAKE_CURRENT_BINARY_DIR}/${_F} COPYONLY)
      GET_FILENAME_COMPONENT(_BF ${_F} NAME)
      INSTALL(FILES ${_BF} DESTINATION share/${PROJECT_PACKAGE_NAME})
    ENDFOREACH(_F ${_SFILES})
  ENDIF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/config/xmlschema)

  # Sources
  IF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
    SUBDIRS(src)
  ENDIF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/src)

  # Packaging
  INCLUDE(CPack)
  INCLUDE(InstallRequiredSystemLibraries)
  
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${_PACKAGE_INFO})
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
  SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
  SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

ENDMACRO(SICONOS_PROJECT)

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

# This is done in the BLAS, LAPACK macros but it is not correct
MACRO(COMPILE_WITH)
  CAR(_NAME ${ARGV})
  STRING(TOUPPER ${_NAME} _UNAME)
  LIST(APPEND _NAMES ${_NAME})
  LIST(APPEND _NAMES ${_UNAME})
  FIND_PACKAGE(${ARGV})
  FOREACH(_N ${_NAMES})
    IF(${_N}_FOUND)
      IF(${_N}_INCLUDE_DIRS)
        REMEMBER_INCLUDE_DIRECTORIES("${${_N}_INCLUDE_DIRS}")
      ENDIF(${_N}_INCLUDE_DIRS)
      IF(${_N}_INCLUDE_DIR)
        REMEMBER_INCLUDE_DIRECTORIES("${${_N}_INCLUDE_DIR}")
      ENDIF(${_N}_INCLUDE_DIR)
      IF(${NAME}_INCLUDE_PATH)
        REMEMBER_INCLUDE_DIRECTORIES("${${_N}_INCLUDE_PATH}")
      ENDIF(${NAME}_INCLUDE_PATH)
      IF(${_N}_LIBRARY_DIRS)
        REMEMBER_LINK_DIRECTORIES("${${_N}_LIBRARY_DIRS}")
      ENDIF(${_N}_LIBRARY_DIRS)
      IF(${_N}_LIBRARIES)
        REMEMBER_LINK_LIBRARIES("${${_N}_LIBRARIES}")
      ENDIF(${_N}_LIBRARIES)
      IF(${_N}_DEFINITIONS) # not Fortran is supposed
        APPEND_C_FLAGS(${${_N}_DEFINITIONS})
        APPEND_CXX_FLAGS(${${_N}_DEFINITIONS})
      ENDIF(${_N}_DEFINITIONS)
    ENDIF(${_N}_FOUND)
  ENDFOREACH(_N ${_NAME} ${_UNAME})
  # update config.h
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)
  SET(_N)
  SET(_NAME) 
  SET(_UNAME)
  SET(_NAMES)
ENDMACRO(COMPILE_WITH)

# Tests
MACRO(BEGIN_TEST _D)
  SET(_CURRENT_TEST_DIRECTORY ${_D})
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${_D})
  
  # find and copy data files : *.mat, *.dat and *.xml
  FILE(GLOB_RECURSE _DATA_FILES 
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${_D}
    ${CMAKE_CURRENT_SOURCE_DIR}/${_D}/*.mat 
    ${CMAKE_CURRENT_SOURCE_DIR}/${_D}/*.dat
    ${CMAKE_CURRENT_SOURCE_DIR}/${_D}/*.xml)
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
  
ENDMACRO(NEW_TEST)

MACRO(END_TEST)
  ADD_SUBDIRECTORY(${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY})
ENDMACRO(END_TEST)
