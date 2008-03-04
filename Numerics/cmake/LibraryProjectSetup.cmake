#
# static and shared library setup
#
# input:
#
# <PROJECT_NAME>_DIRS : sources directories
# <PROJECT_NAME>_Unstable_SRCS : built only if -DUNSTABLE=ON
# <PROJECT_NAME>_VERSION
# <PROJECT_NAME>_SOURCE_FILE_EXTENSIONS may contains the wanted sources extensions (default: all extensions)

macro(LIBRARY_PROJECT_SETUP)

message(STATUS "")
message(STATUS "Setting up ${PROJECT_NAME} library build")

set(_ALL_DIRS ${${PROJECT_NAME}_DIRS})

#

#

if(NOT ${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS)
  # all extensions we know (should be done only with the project languages)
  set(_ALL_EXTS)
  foreach(_EXT ${CMAKE_CXX_SOURCE_FILE_EXTENSIONS} ${CMAKE_C_SOURCE_FILE_EXTENSIONS} ${CMAKE_Fortran_SOURCE_FILE_EXTENSIONS} ${CMAKE_Java_SOURCE_FILE_EXTENSIONS} ${CMAKE_RC_SOURCE_FILE_EXTENSIONS})
    list(APPEND _ALL_EXTS *.${_EXT})
  endforeach(_EXT ${CMAKE_CXX_SOURCE_FILE_EXTENSIONS} ${CMAKE_C_SOURCE_FILE_EXTENSIONS} ${CMAKE_Fortran_SOURCE_FILE_EXTENSIONS} ${CMAKE_Java_SOURCE_FILE_EXTENSIONS} ${CMAKE_RC_SOURCE_FILE_EXTENSIONS})
else(NOT ${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS)
  # specified extensions
  foreach(_EXT ${${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS})
    list(APPEND _ALL_EXTS *.${_EXT})
  endforeach(_EXT ${${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS})
endif(NOT ${PROJECT_NAME}_SOURCE_FILE_EXTENSIONS)

set(_ALL_FILES)
foreach(_DIR ${_ALL_DIRS})
  set(_DIR_FILES)
  foreach(_EXT ${_ALL_EXTS})
    file(GLOB _DIR_FILES_EXT ${_DIR}/${_EXT})
    if(_DIR_FILES_EXT)
      list(APPEND _DIR_FILES ${_DIR_FILES_EXT})
    endif(_DIR_FILES_EXT)
  endforeach(_EXT ${_ALL_EXTS})
  if(_DIR_FILES)
    foreach(_F ${_DIR_FILES})
      if(NOT ${_F}_T)
        set(${_F}_T T)
        list(APPEND _ALL_FILES ${_F})
      endif(NOT ${_F}_T)
    endforeach(_F ${_DIR_FILES})
  endif(_DIR_FILES)
endforeach(_DIR ${_ALL_DIRS})

#
# headers
#
if(NOT ${PROJECT_NAME}_HDRS)
  foreach(_DIR ${_ALL_DIRS})
    file(GLOB _HDRS  ${_DIR}/*.h)
    if(_HDRS)
      list(APPEND ${PROJECT_NAME}_HDRS ${_HDRS})
    endif(_HDRS)
  endforeach(_DIR ${_ALL_DIRS})
endif(NOT ${PROJECT_NAME}_HDRS)

#
# Unstable sources
#
if(NOT UNSTABLE)
  if(${PROJECT_NAME}_Unstable_SRCS)
    message(STATUS "Some unstables sources files are going to be excluded")
    message(STATUS "To configure an unstable build, run : `cmake -DUNSTABLE=ON .'")
    foreach(_FILE ${${PROJECT_NAME}_Unstable_SRCS})
      file(GLOB _GFILE ${_FILE})
      if(_GFILE)
        message(STATUS "excluded : ${_GFILE}")
        list(REMOVE_ITEM _ALL_FILES ${_GFILE})
      else(_GFILE)
        message(STATUS "WARNING : Unstable file NOT FOUND : ${_FILE}")
      endif(_GFILE)
    endforeach(_FILE ${${PROJECT_NAME}_Unstable_SRCS})
  endif(${PROJECT_NAME}_Unstable_SRCS)
endif(NOT UNSTABLE)

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

set(${PROJECT_NAME}_SRCS ${_ALL_FILES})

include_directories(${_ALL_DIRS})

add_library(${PROJECT_NAME}_static  STATIC ${_ALL_FILES})
add_library(${PROJECT_NAME}_shared  SHARED ${_ALL_FILES})

set_target_properties(${PROJECT_NAME}_static PROPERTIES 
  OUTPUT_NAME "${PROJECT_NAME}" 
  VERSION "${${PROJECT_NAME}_VERSION}" 
  CLEAN_DIRECT_OUTPUT 1 # no clobbering
  LINKER_LANGUAGE C)

set_target_properties(${PROJECT_NAME}_shared PROPERTIES
  OUTPUT_NAME "${PROJECT_NAME}" 
  VERSION "${${PROJECT_NAME}_VERSION}" 
  CLEAN_DIRECT_OUTPUT 1
  LINKER_LANGUAGE C)

target_link_libraries(${PROJECT_NAME}_static ${${PROJECT_NAME}_LINK_LIBRARIES})
target_link_libraries(${PROJECT_NAME}_shared ${${PROJECT_NAME}_LINK_LIBRARIES})

if(APPLE)
  set(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -Wl,-search_paths_first")
  if(FORTRAN_LIBRARIES)
    link_directories(${FORTRAN_COMPILER_LIB_DIRECTORY})
    target_link_libraries(${PROJECT_NAME}_static ${FORTRAN_LIBRARIES})
    target_link_libraries(${PROJECT_NAME}_shared ${FORTRAN_LIBRARIES})
  endif(FORTRAN_LIBRARIES)
endif(APPLE)

# output in ${PROJECT_NAME}_STATIC|SHARED_LIB the path of the libraries
get_target_property(${PROJECT_NAME}_STATIC_LIB ${PROJECT_NAME}_static LOCATION)
get_target_property(${PROJECT_NAME}_SHARED_LIB ${PROJECT_NAME}_shared LOCATION)


# Installation
if(${PROJECT_NAME}_LIB_DIR)
  set(_install_lib ${${PROJECT_NAME}_LIB_DIR})
else(${PROJECT_NAME}_LIB_DIR)
  set(_install_lib lib)
endif(${PROJECT_NAME}_LIB_DIR)

if(${PROJECT_NAME}_INSTALL_INCLUDE_DIR)
  set(_install_include ${${PROJECT_NAME}_INSTALL_INCLUDE_DIR})
else(${PROJECT_NAME}_INSTALL_INCLUDE_DIR)
  set(_install_include include)
endif(${PROJECT_NAME}_INSTALL_INCLUDE_DIR)

install(TARGETS 
  ${PROJECT_NAME}_static ${PROJECT_NAME}_shared 
  ARCHIVE DESTINATION ${_install_lib}
  LIBRARY DESTINATION ${_install_lib})
install(FILES ${${PROJECT_NAME}_HDRS} DESTINATION ${_install_include})

message(STATUS "${PROJECT_NAME} library setup done")
message(STATUS "")

endmacro(LIBRARY_PROJECT_SETUP)

