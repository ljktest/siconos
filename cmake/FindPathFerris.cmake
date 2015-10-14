# -----------------------------------------
# Search for path library (headers are in externals)
# --> libpath
#
# User hint : PathFerris_DIR --> try cmake -DPathFerris_DIR=path-to-path-install
#
# By default we are using path 4.7
# -----------------------------------------
include(LibFindMacros)

IF(NOT PathFerris_VERSION)
  SET(PathFerris_VERSION "47")
ENDIF(NOT PathFerris_VERSION)

IF(NOT PathFerris_DIR AND GAMS_FOUND)
  SET(PathFerris_DIR ${GAMS_DIR})
ENDIF(NOT PathFerris_DIR AND GAMS_FOUND)

# ----------------------------------------
# Library == libpath${PathFerris_VERSION}
# First, search in (optionnaly) user-defined PathFerris_DIR
# Try:
# - PathFerris_DIR
# - PathFerris_DIR/lib
# - PathFerris_DIR/lib/CMAKE_LIBRARY_ARCHITECTURE (for example on Debian like system : lib/x86_64-linux-gnu
if(PathFerris_DIR)
find_library(PathFerris_LIBRARY path${PathFerris_VERSION}
  PATHS ${PathFerris_DIR}
  PATH_SUFFIXES lib lib/${CMAKE_LIBRARY_ARCHITECTURE} 
  )
else(PathFerris_DIR)
find_library(PathFerris_LIBRARY path${PathFerris_VERSION}
  )
endif(PathFerris_DIR)
# If not found, try standard path.

# Set PathFerris_LIBRARY_DIRS and PathFerris_LIBRARIES for libfindprocess
# see Using LibFindMacros : https://cmake.org/Wiki/CMake:How_To_Find_Libraries
IF (PathFerris_LIBRARY)
  GET_FILENAME_COMPONENT(PathFerris_LIBRARY_DIRS ${PathFerris_LIBRARY} PATH)
  GET_FILENAME_COMPONENT(PathFerris_LIBRARIES ${PathFerris_LIBRARY} REALPATH)
endif()
IF (NOT PathFerris_FIND_QUIETLY)
  MESSAGE(STATUS "Found PathFerris: ${PathFerris_LIBRARY}")
  MESSAGE(STATUS "PathFerris_LIBRARIES: ${PathFerris_LIBRARIES}")
ENDIF (NOT PathFerris_FIND_QUIETLY)

# Final check :
set(PathFerris_PROCESS_LIBS PathFerris_LIBRARIES)
libfind_process(PathFerris)
