#
# Siconos documentation with Doxygen 
#

# Style sheet
FILE(GLOB CSS_FILES config/css/*.* config/css/*)
FOREACH(FILE ${CSS_FILES})
  IF(NOT IS_DIRECTORY ${FILE})
    FILE(RELATIVE_PATH RFILE ${CMAKE_SOURCE_DIR} ${FILE})
    CONFIGURE_FILE(${FILE} ${CMAKE_BINARY_DIR}/${RFILE} COPYONLY)
  ENDIF(NOT IS_DIRECTORY ${FILE})
ENDFOREACH(FILE ${CSS_FILES})

# If we are under svn and DOXYGEN_OUTPUT is not set, the output should
# go in GeneratedDocs
MESSAGE ( STATUS ${DOXYGEN_OUTPUT})

IF (NOT DOXYGEN_OUTPUT)
  IF(SVN_REVISION)
    # build in source directory
    SET(DOXYGEN_OUTPUT ${CMAKE_SOURCE_DIR}/../GeneratedDocs)
  ELSE(SVN_REVISION)
    MESSAGE(FATAL_ERROR "DOXYGEN_OUPUT must be set")
  ENDIF(SVN_REVISION)
ENDIF(NOT DOXYGEN_OUTPUT)

FILE(MAKE_DIRECTORY ${DOXYGEN_OUTPUT})
FILE(MAKE_DIRECTORY ${DOXYGEN_OUTPUT}/${PROJECT})
FILE(MAKE_DIRECTORY ${DOXYGEN_OUTPUT}/Tags)


INCLUDE(TargetDoc OPTIONAL)
