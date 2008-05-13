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
    _PACKAGE_SUMMARY
    _PACKAGE_DESCRIPTION)
  
  STRING(TOLOWER ${_PROJECT_NAME} _LPROJECT_NAME)
  
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
  ENDIF(CMAKE_Fortran_COMPILER)

  # Tests+Dashboard configuration
  ENABLE_TESTING()
  SET(BUILDNAME "${_PROJECT_NAME}")
  INCLUDE(Pipol)
  INCLUDE(DartConfig)
  INCLUDE(Dart)
  
  # The library build stuff
  INCLUDE(LibraryProjectSetup)
  
  # Doxygen documentation
  INCLUDE(SiconosDoc)

  # config.h and include
  IF(NOT CONFIG_H_GLOBAL_CONFIGURED)
    SET(CONFIG_H_GLOBAL_CONFIGURED 1 CACHE BOOL "config.h global generation." )
    CONFIGURE_FILE(config.h.cmake config.h)
  ENDIF(NOT CONFIG_H_GLOBAL_CONFIGURED)
  INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

  # Top level install
  SET(CMAKE_INCLUDE_CURRENT_DIR ON)
  INSTALL(FILES AUTHORS ChangeLog COPYING INSTALL README 
    DESTINATION share/doc/siconos-${VERSION}/${_PROJECT_NAME})

  # man files
  IF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/man)
    CONFIGURE_FILE(man/siconos.1.in man/siconos.1)
    INSTALL(FILES ${CMAKE_BINARY_DIR}/man/siconos.1 DESTINATION man/man1)
  ENDIF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/man)

  # scripts
  FILE(GLOB SCRIPT_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */*.script.in)
  IF(SCRIPT_FILES)
    FOREACH(_FILE ${SCRIPT_FILES})
      GET_FILENAME_COMPONENT(_BFILE ${_FILE} NAME_WE)
      GET_FILENAME_COMPONENT(_PFILE ${_FILE} PATH)
      FILE(MAKE_DIRECTORY ${_PFILE})
      CONFIGURE_FILE(${_FILE} ${_PFILE}/${_BFILE} @ONLY)
      INSTALL(FILES ${CMAKE_BINARY_DIR}/${_PFILE}/${_BFILE} DESTINATION bin RENAME ${_BFILE}
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ 
                    OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
    ENDFOREACH(_FILE ${SCRIPT_FILES})
  ENDIF(SCRIPT_FILES)

  # cmakelists
  FILE(GLOB CMAKELISTS_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */CMakeLists.txt.in)
  IF(CMAKELISTS_FILES)
    FOREACH(_FILE ${CMAKELISTS_FILES})
      GET_FILENAME_COMPONENT(_BFILE ${_FILE} NAME_WE)
      GET_FILENAME_COMPONENT(_PFILE ${_FILE} PATH)
      FILE(MAKE_DIRECTORY ${_PFILE})
      CONFIGURE_FILE(${_FILE} ${_PFILE}/${_BFILE}.txt @ONLY)
      INSTALL(FILES ${CMAKE_BINARY_DIR}/${_PFILE}/${_BFILE}.txt DESTINATION share/${PROJECT_PACKAGE_NAME} RENAME ${_BFILE}.txt)
    ENDFOREACH(_FILE ${CMAKELISTS_FILES})
  ENDIF(CMAKELISTS_FILES)


  # xml
  IF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/config/xmlschema)
    FILE(GLOB _SFILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} config/xmlschema/*.xsd)
    FOREACH(_F ${_SFILES})
      CONFIGURE_FILE(${_F} ${CMAKE_CURRENT_BINARY_DIR}/${_F} COPYONLY)
      INSTALL(FILES ${_F} DESTINATION share/${PROJECT_PACKAGE_NAME})
    ENDFOREACH(_F ${_SFILES})
  ENDIF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/config/xmlschema)

  # needed cmake macros
  IF(EXISTS ${CMAKE_SOURCE_DIR}/cmake/FindQGLViewer.cmake)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/cmake/FindQGLViewer.cmake DESTINATION share/${PROJECT_PACKAGE_NAME}/cmake)
  ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/cmake/FindQGLViewer.cmake) 
  
  # Sources
  IF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
    SUBDIRS(src)
  ENDIF(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/src)

  # To save build settings
  INCLUDE(CMakeExportBuildSettings)

  # Packaging
  INCLUDE(InstallRequiredSystemLibraries)
  
  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${_PACKAGE_SUMMARY})
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
  SET(CPACK_PACKAGE_NAME "${PROJECT_PACKAGE_NAME}")
  SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
  SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

  INCLUDE(UseRPMTools OPTIONAL)
  IF(RPMTools_FOUND)
    RPMTools_ADD_RPM_TARGETS(${PROJECT_PACKAGE_NAME})
  ENDIF(RPMTools_FOUND)

  INCLUDE (DpkgDeb OPTIONAL)
  IF ( DPKG_FOUND )
    SET ( PACKAGE_NAME ${PROJECT_PACKAGE_NAME} )
    SET ( PACKAGE_VERSION "${VERSION}" )
    SET ( PACKAGE_RELEASE 0 )
    SET ( PACKAGE_MAINTAINER_NAME "siconos development team" )
    SET ( PACKAGE_MAINTAINER_EMAIL "siconos-devel-fr@lists.gforge.inria.fr" )
    SET ( PACKAGE_DESCRIPTION  "${_PACKAGE_DESCRIPTION}")
    SET ( PACKAGE_DESCRIPTION_SUMMARY ${_PACKAGE_SUMMARY} )
    SET ( PACKAGE_DEPENDS g++;gfortran;atlas3-base;atlas3-base-dev;atlas3-headers;doxygen)

    ADD_DEBIAN_PACKAGE ( ${PROJECT_PACKAGE_NAME} )

  ENDIF ( DPKG_FOUND )

  INCLUDE(CPack)

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

# link/include is done in the BLAS, LAPACK macros, but not in others.
MACRO(COMPILE_WITH)
  CAR(_NAME ${ARGV})
  CDR(_REQ ${ARGV})
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

  # update config.h
  IF(NOT CONFIG_H_${_NAME}_CONFIGURED)
    SET(CONFIG_H_${_NAME}_CONFIGURED 1 CACHE BOOL "config.h generation for package ${_NAME}")
    CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)
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
  
  # pb env in ctest, see http://www.vtk.org/Bug/view.php?id=6391#bugnotes
  CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/ldwrap.c.in 
    ${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY}/${_EXE}.ldwrap.c)
  
ENDMACRO(NEW_TEST)

MACRO(END_TEST)
  ADD_SUBDIRECTORY(${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY} ${CMAKE_CURRENT_BINARY_DIR}/${_CURRENT_TEST_DIRECTORY})
ENDMACRO(END_TEST)


