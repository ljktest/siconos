#
# static and shared library setup
#
# input:
#
# <PROJECT_NAME>_SRCS : [optional] Project SRCS on per files basis
# <PROJECT_NAME>_LIBS_NAME : [optional] libraries name. if it is empty the libs name are the same as PROJECT_NAME.
# <PROJECT_NAME>_DIRS : sources directories
# <PROJECT_NAME>_Unstable_SRCS : built only if -DUNSTABLE=ON
# <PROJECT_NAME>_VERSION : version of the library
# <PROJECT_NAME>_HDRS : installation headers  (if none all headers)
# <PROJECT_NAME>_INSTALL_INCLUDE_DIR : where to install headers
# <PROJECT_NAME>_INSTALL_LIB_DIR     : where to install the build libraries
# <PROJECT_NAME>_SOURCE_FILE_EXTENSIONS may contains the wanted sources extensions (default: all extensions)
# <PROJECT_NAME>_LINKER_LANGUAGE : the language used to link the whole librarie

MACRO(LIBRARY_PROJECT_SETUP)

  MESSAGE(STATUS "")
  MESSAGE(STATUS "Setting up ${PROJECT_NAME} library build")

  # do not skip the full RPATH for the build tree
  SET(CMAKE_SKIP_BUILD_RPATH FALSE)

  # when building, don't use the install RPATH already
  # (but later on when installing)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

  # the RPATH to be used when installing
  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

  # don't add the automatically determined parts of the RPATH
  # which point to directories outside the build tree to the install RPATH
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

  #+RPG add a reset of _ALL_EXTS
  SET(_ALL_EXTS)
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
  
  #+++RPG add the possiblity to add files with PROJECT_NAME_SRCS and not automatically
  IF(${PROJECT_NAME}_SRCS) 
    MESSAGE(STATUS "Sources files manually with var ${PROJECT_NAME}_SRCS")
    FOREACH(_FILE ${${PROJECT_NAME}_SRCS})
      FILE(GLOB _GFILE ${_FILE})
      IF(_GFILE)
        LIST(APPEND _ALL_FILES ${_GFILE})
      ELSE(_GFILE)
        MESSAGE(STATUS "WARNING : file NOT FOUND : ${_FILE}")
      ENDIF(_GFILE)
    ENDFOREACH(_FILE ${${PROJECT_NAME}_SRCS})
  ELSE(${PROJECT_NAME}_SRCS)
    

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
  ENDIF(${PROJECT_NAME}_SRCS)
  #
  # headers
  #
  IF(NOT ${PROJECT_NAME}_HDRS)
    FOREACH(_DIR ${_ALL_DIRS})
      FILE(GLOB _HDRS  ${_DIR}/*.h ${_DIR}/*.hpp)
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
  
  SET(${PROJECT_NAME}_SRCS ${_ALL_FILES})
  
  INCLUDE_DIRECTORIES(${_ALL_DIRS})

  #
  # On Mac OS : 
  #  Note Franck : I have commented the following lines, test purpose. It seems to work
  # without these on SnowLeopard and above ...To be confirmed
  #IF(APPLE)
    # 1 : do not use defaults system libraries (i.e not fortran lapack lib but atlas/lapack lib)
    #SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -Wl,-search_paths_first,-single_module")
    #SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -Wl,-search_paths_first")
    #SET(CMAKE_SHARED_LINKER_FLAGS "-read_only_relocs suppress")
  #ENDIF(APPLE)

  IF(_ALL_FILES)
    IF(BUILD_STATIC_LIBS)
      ADD_LIBRARY(${PROJECT_NAME}_static  STATIC ${_ALL_FILES})
    ENDIF(BUILD_STATIC_LIBS)
    
    IF(BUILD_SHARED_LIBS)
      ADD_LIBRARY(${PROJECT_NAME}_shared  SHARED ${_ALL_FILES})
    ENDIF(BUILD_SHARED_LIBS)

    APPEND_Fortran_FLAGS("-w") # gnu specific ...
    
    IF(BUILD_STATIC_LIBS)
      SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES 
        OUTPUT_NAME "${PROJECT_NAME}" 
        VERSION "${${PROJECT_NAME}_VERSION}" 
        CLEAN_DIRECT_OUTPUT 1 # no clobbering
        LINKER_LANGUAGE ${${PROJECT_NAME}_LINKER_LANGUAGE})
    ENDIF(BUILD_STATIC_LIBS)
    
    IF(BUILD_SHARED_LIBS)
      SET_TARGET_PROPERTIES(${PROJECT_NAME}_shared PROPERTIES
        OUTPUT_NAME "${PROJECT_NAME}" 
        VERSION "${${PROJECT_NAME}_VERSION}" 
        CLEAN_DIRECT_OUTPUT 1 
        LINKER_LANGUAGE ${${PROJECT_NAME}_LINKER_LANGUAGE}
        )
      IF(MSVC)
        GET_FILENAME_COMPONENT(${PROJECT_NAME}_SHARED_LIB_WE lib${PROJECT_NAME} NAME_WE)
        SET_TARGET_PROPERTIES(${PROJECT_NAME}_shared PROPERTIES
          ENABLE_EXPORT 1
          LINK_FLAGS /DEF:"${CMAKE_CURRENT_BINARY_DIR}/${${PROJECT_NAME}_SHARED_LIB_WE}.def")
        ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}_shared
          PRE_BUILD
          COMMAND ${CMAKE_NM} ARGS @${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}_shared.dir/objects1.rsp
          | egrep ' "(D|T|B)" ' | cut -f 3 -d ' ' | sed  's,^_,,' | sed  '1iEXPORTS' > ${${PROJECT_NAME}_SHARED_LIB_WE}.def
          ) # gruik gruik
      ENDIF(MSVC)
    ENDIF(BUILD_SHARED_LIBS)
    
    IF(BUILD_STATIC_LIBS)
      TARGET_LINK_LIBRARIES(${PROJECT_NAME}_static ${${PROJECT_NAME}_LINK_LIBRARIES})
      # output in ${PROJECT_NAME}_STATIC|SHARED_LIB the path of the libraries
      GET_TARGET_PROPERTY(${PROJECT_NAME}_STATIC_LIB ${PROJECT_NAME}_static LOCATION)
    ENDIF(BUILD_STATIC_LIBS)
    
    IF(BUILD_SHARED_LIBS)
      MESSAGE(STATUS "Link directories : ${${PROJECT_NAME}_LINK_DIRECTORIES}")
      MESSAGE(STATUS "Link libraries : ${${PROJECT_NAME}_LINK_LIBRARIES}")
      TARGET_LINK_LIBRARIES(${PROJECT_NAME}_shared ${${PROJECT_NAME}_LINK_LIBRARIES})
      GET_TARGET_PROPERTY(${PROJECT_NAME}_SHARED_LIB ${PROJECT_NAME}_shared LOCATION)
      IF(MSVC)
        GET_FILENAME_COMPONENT(${PROJECT_NAME}_SHARED_LIB_PATH ${${PROJECT_NAME}_SHARED_LIB} PATH)
        SET(${PROJECT_NAME}_SHARED_LIB_LINK "${${PROJECT_NAME}_SHARED_LIB_PATH}/${${PROJECT_NAME}_SHARED_LIB_WE}.dll.a") # Hack
      ELSE(MSVC)
        SET(${PROJECT_NAME}_SHARED_LIB_LINK ${${PROJECT_NAME}_SHARED_LIB})
      ENDIF(MSVC)
      IF(${PROJECT_NAME}_LIBS_NAME)
        SET_TARGET_PROPERTIES(${PROJECT_NAME}_shared
          PROPERTIES OUTPUT_NAME  ${${PROJECT_NAME}_LIBS_NAME})
      ENDIF(${PROJECT_NAME}_LIBS_NAME)
    ENDIF(BUILD_SHARED_LIBS)

    IF(BUILD_STATIC_LIBS)
      GET_FILENAME_COMPONENT(${PROJECT_NAME}_STATIC_LIB_NAME ${${PROJECT_NAME}_STATIC_LIB} NAME)
      IF (${PROJECT_NAME}_LIBS_NAME)
        SET_TARGET_PROPERTIES(${PROJECT_NAME}_static
          PROPERTIES OUTPUT_NAME  ${${PROJECT_NAME}_LIBS_NAME})
      ENDIF(${PROJECT_NAME}_LIBS_NAME)
    ENDIF(BUILD_STATIC_LIBS)
    
    IF(BUILD_SHARED_LIBS)
      GET_FILENAME_COMPONENT(${PROJECT_NAME}_SHARED_LIB_NAME ${${PROJECT_NAME}_SHARED_LIB} NAME)
    ENDIF(BUILD_SHARED_LIBS)
    
    # Installation
    IF(${PROJECT_NAME}_INSTALL_LIB_DIR)
      SET(_install_lib ${${PROJECT_NAME}_INSTALL_LIB_DIR})
    ELSE(${PROJECT_NAME}_INSTALL_LIB_DIR)
      ASSERT(CMAKE_INSTALL_LIBDIR)
      SET(_install_lib ${CMAKE_INSTALL_LIBDIR})
      SET(${PROJECT_NAME}_INSTALL_LIB_DIR ${_install_lib})
    ENDIF(${PROJECT_NAME}_INSTALL_LIB_DIR)
    
    IF(${PROJECT_NAME}_INSTALL_INCLUDE_DIR)
      SET(_install_include ${${PROJECT_NAME}_INSTALL_INCLUDE_DIR})
    ELSE(${PROJECT_NAME}_INSTALL_INCLUDE_DIR)
      SET(_install_include include)
      SET(${PROJECT_NAME}_INSTALL_INCLUDE_DIR ${_install_include})
    ENDIF(${PROJECT_NAME}_INSTALL_INCLUDE_DIR)
    
    INSTALL(FILES ${${PROJECT_NAME}_HDRS} DESTINATION ${_install_include})
    
    IF(BUILD_SHARED_LIBS AND BUILD_STATIC_LIBS)
      INSTALL(TARGETS 
        ${PROJECT_NAME}_static ${PROJECT_NAME}_shared 
        RUNTIME DESTINATION ${_install_lib}
        ARCHIVE DESTINATION ${_install_lib}
        LIBRARY DESTINATION ${_install_lib})
    ELSEIF(BUILD_SHARED_LIBS) 
      INSTALL(TARGETS 
        ${PROJECT_NAME}_shared
        RUNTIME DESTINATION ${_install_lib}
        ARCHIVE DESTINATION ${_install_lib}
        LIBRARY DESTINATION ${_install_lib})
    ELSEIF(BUILD_STATIC_LIBS)
      INSTALL(TARGETS 
        ${PROJECT_NAME}_static
        RUNTIME DESTINATION ${_install_lib}
        ARCHIVE DESTINATION ${_install_lib}
        LIBRARY DESTINATION ${_install_lib})
    ELSE(BUILD_STATIC_LIBS)
      MESSAGE("ERROR:: BUILD_SHARED_LIBS and/or BUILD_SHARED_LIBS must be set")
    ENDIF(BUILD_SHARED_LIBS AND BUILD_STATIC_LIBS)

 
    MESSAGE(STATUS "${PROJECT_NAME} library setup done")
    MESSAGE(STATUS "")
    
  ENDIF(_ALL_FILES)
ENDMACRO(LIBRARY_PROJECT_SETUP)

