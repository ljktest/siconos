#
# static and shared library setup
#
# input:
#
# <PROJECT_NAME>_DIRS : sources directories
# <PROJECT_NAME>_Unstable_SRCS : built only if -DUNSTABLE=ON
# <PROJECT_NAME>_VERSION
# <PROJECT_NAME>_SOURCE_FILE_EXTENSIONS may contains the wanted sources extensions (default: all extensions)

MACRO(LIBRARY_PROJECT_SETUP)

  MESSAGE(STATUS "")
  MESSAGE(STATUS "Setting up ${PROJECT_NAME} library build")

  SET(_ALL_DIRS ${${PROJECT_NAME}_DIRS})
  IF(NOT ${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS)
    # all extensions we know (should be done only with the project languages)
    SET(_ALL_EXTS)
    FOREACH(_EXT ${CMAKE_CXX_SOURCE_FILE_EXTENSIONS} ${CMAKE_C_SOURCE_FILE_EXTENSIONS} ${CMAKE_Fortran_SOURCE_FILE_EXTENSIONS} ${CMAKE_Java_SOURCE_FILE_EXTENSIONS} ${CMAKE_RC_SOURCE_FILE_EXTENSIONS})
      LIST(APPEND _ALL_EXTS *.${_EXT})
    ENDFOREACH(_EXT ${CMAKE_CXX_SOURCE_FILE_EXTENSIONS} ${CMAKE_C_SOURCE_FILE_EXTENSIONS} ${CMAKE_Fortran_SOURCE_FILE_EXTENSIONS} ${CMAKE_Java_SOURCE_FILE_EXTENSIONS} ${CMAKE_RC_SOURCE_FILE_EXTENSIONS})
  ELSE(NOT ${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS)
    # specified extensions
    FOREACH(_EXT ${${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS})
      LIST(APPEND _ALL_EXTS *.${_EXT})
    ENDFOREACH(_EXT ${${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS})
  ENDIF(NOT ${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS)
  
  SET(_ALL_FILES)
  FOREACH(_DIR ${_ALL_DIRS})
    SET(_DIR_FILES)
    FOREACH(_EXT ${_ALL_EXTS})
      FILE(GLOB _DIR_FILES_EXT ${_DIR}/${_EXT})
      IF(_DIR_FILES_EXT)
        LIST(APPEND _DIR_FILES ${_DIR_FILES_EXT})
      ENDIF(_DIR_FILES_EXT)
    ENDFOREACH(_EXT ${_ALL_EXTS})
    IF(_DIR_FILES)
      FOREACH(_F ${_DIR_FILES})
        IF(NOT ${_F}_T)
          SET(${_F}_T T)
          LIST(APPEND _ALL_FILES ${_F})
        ENDIF(NOT ${_F}_T)
      ENDFOREACH(_F ${_DIR_FILES})
    ENDIF(_DIR_FILES)
  ENDFOREACH(_DIR ${_ALL_DIRS})
  
  #
  # headers
  #
  IF(NOT ${PROJECT_NAME}_HDRS)
    FOREACH(_DIR ${_ALL_DIRS})
      FILE(GLOB _HDRS  ${_DIR}/*.h)
      IF(_HDRS)
        LIST(APPEND ${PROJECT_NAME}_HDRS ${_HDRS})
      ENDIF(_HDRS)
    ENDFOREACH(_DIR ${_ALL_DIRS})
  ENDIF(NOT ${PROJECT_NAME}_HDRS)
  
  #
  # Unstable sources
  #
  IF(NOT UNSTABLE)
    IF(${PROJECT_NAME}_Unstable_SRCS)
      MESSAGE(STATUS "Some unstables sources files are going to be excluded")
      MESSAGE(STATUS "To configure an unstable build, run : `cmake -DUNSTABLE=ON .'")
      FOREACH(_FILE ${${PROJECT_NAME}_Unstable_SRCS})
        FILE(GLOB _GFILE ${_FILE})
        IF(_GFILE)
          MESSAGE(STATUS "excluded : ${_GFILE}")
          LIST(REMOVE_ITEM _ALL_FILES ${_GFILE})
        ELSE(_GFILE)
          MESSAGE(STATUS "WARNING : Unstable file NOT FOUND : ${_FILE}")
        ENDIF(_GFILE)
      ENDFOREACH(_FILE ${${PROJECT_NAME}_Unstable_SRCS})
    ENDIF(${PROJECT_NAME}_Unstable_SRCS)
  ENDIF(NOT UNSTABLE)
  
  #
  # Attempt to specify file properties
  #
  
  #if(${PROJECT_NAME}_SOURCES_PROPERTIES)
  #  foreach(_PROPERTY ${${PROJECT_NAME}_SOURCES_PROPERTIES})
  #    foreach(_FILE ${_ALL_FILES})
  #      if(${${PROJECT_NAME}_${_PROPERTY}_REGEX})
  #        if(${_FILE} MATCHES ${${PROJECT_NAME}_${_PROPERTY}}_REGEX})
  #          if(${${PROJECT_NAME}_${_PROPERTY})
  #            set_source_files_properties(${_FILE} ${${PROJECT_NAME}_${_PROPERTY}})
  #          endif(${${PROJECT_NAME}_${_PROPERTY})
  #        endif(${_FILE} MATCHES ${${PROJECT_NAME}_${_PROPERTY}}_REGEX})
  #      endif(${${PROJECT_NAME}_${_PROPERTY}_REGEX})
  #    endforeach(_FILE ${_ALL_FILES})
  #  endforeach(_PROPERTY ${${PROJECT_NAME}_SOURCES_PROPERTIES})
  #endif(${PROJECT_NAME}_SOURCES_PROPERTIES)
  
  SET(${PROJECT_NAME}_SRCS ${_ALL_FILES})
  
  INCLUDE_DIRECTORIES(${_ALL_DIRS})

  ADD_LIBRARY(${PROJECT_NAME}_static  STATIC ${_ALL_FILES})
  ADD_LIBRARY(${PROJECT_NAME}_shared  SHARED ${_ALL_FILES})

  SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES 
    OUTPUT_NAME "${PROJECT_NAME}" 
    VERSION "${${PROJECT_NAME}_VERSION}" 
    CLEAN_DIRECT_OUTPUT 1 # no clobbering
    LINKER_LANGUAGE C)
  
  SET_TARGET_PROPERTIES(${PROJECT_NAME}_shared PROPERTIES
    OUTPUT_NAME "${PROJECT_NAME}" 
    VERSION "${${PROJECT_NAME}_VERSION}" 
    CLEAN_DIRECT_OUTPUT 1
    LINKER_LANGUAGE C)
  
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}_static ${${PROJECT_NAME}_LINK_LIBRARIES})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME}_shared ${${PROJECT_NAME}_LINK_LIBRARIES})
  
  IF(APPLE)
    SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -Wl,-search_paths_first")
    IF(FORTRAN_LIBRARIES)
      LINK_DIRECTORIES(${FORTRAN_COMPILER_LIB_DIRECTORY})
      TARGET_LINK_LIBRARIES(${PROJECT_NAME}_static ${FORTRAN_LIBRARIES})
      TARGET_LINK_LIBRARIES(${PROJECT_NAME}_shared ${FORTRAN_LIBRARIES})
    ENDIF(FORTRAN_LIBRARIES)
  ENDIF(APPLE)

  # output in ${PROJECT_NAME}_STATIC|SHARED_LIB the path of the libraries
  GET_TARGET_PROPERTY(${PROJECT_NAME}_STATIC_LIB ${PROJECT_NAME}_static LOCATION)
  GET_TARGET_PROPERTY(${PROJECT_NAME}_SHARED_LIB ${PROJECT_NAME}_shared LOCATION)


  # Installation
  IF(${PROJECT_NAME}_INSTALL_LIB_DIR)
    SET(_install_lib ${${PROJECT_NAME}_INSTALL_LIB_DIR})
  ELSE(${PROJECT_NAME}_INSTALL_LIB_DIR)
    SET(_install_lib lib)
  ENDIF(${PROJECT_NAME}_INSTALL_LIB_DIR)

  IF(${PROJECT_NAME}_INSTALL_INCLUDE_DIR)
    SET(_install_include ${${PROJECT_NAME}_INSTALL_INCLUDE_DIR})
  ELSE(${PROJECT_NAME}_INSTALL_INCLUDE_DIR)
    SET(_install_include include)
  ENDIF(${PROJECT_NAME}_INSTALL_INCLUDE_DIR)

  INSTALL(TARGETS 
    ${PROJECT_NAME}_static ${PROJECT_NAME}_shared 
    ARCHIVE DESTINATION ${_install_lib}
    LIBRARY DESTINATION ${_install_lib})
  INSTALL(FILES ${${PROJECT_NAME}_HDRS} DESTINATION ${_install_include})

  EXPORT_LIBRARY_DEPENDENCIES(DEPEND.cmake)

  MESSAGE(STATUS "${PROJECT_NAME} library setup done")
  MESSAGE(STATUS "")

ENDMACRO(LIBRARY_PROJECT_SETUP)

