# - Find BLAS library
# This module finds an installed library that implements the BLAS 
# with its C interface (cblas) and the classical fortran interface.
# 
#
# This module sets the following variables:
#  BLAS_FOUND - set to true if a library implementing the BLAS interface
#    is found
#  BLAS_LIBRARIES - uncached list of libraries (using full path name) to
#    link against to use BLAS
#  BLAS_INCLUDE_DIRS - location of blas headers found by cmake
#  BLAS_LIBRARY_DIR - location of the first blas lib found by cmake.
#  BLAS_HEADERS - name(s) of the header(s) required for cblas.
#
# To find a specific blas set : 
#  WITH_BLAS  if set checks only a specific implementation of blas which may be : mkl, openblas, atlas, accelerate, veclib, generic. 
# Or : 
#  BLAS_DIR if set, cmake uses this path (absolute) to find a blas implementation in BLAS_DIR/include and/or BLAS_DIR/lib 
# 
# If neither WITH_BLAS nor BLAS_DIR is set, all implementation are searched in this order : mkl, openblas, accelerate, veclib, atlas, generic. 
# If available, pkg-config system is used to give hints to cmake search process.
#
if(NOT BLAS_FOUND)
  
  include(CheckFunctionExists)
  include(CheckFortranFunctionExists)
  find_package(PkgConfig)


  # Check the language being used
  # If only C/C++, we check for cblas_... functions.
  # If Fortran and C, we check also the fortran interface.
  get_property( _LANGUAGES_ GLOBAL PROPERTY ENABLED_LANGUAGES )
  if( _LANGUAGES_ MATCHES Fortran )
    set( _CHECK_FORTRAN TRUE )
  elseif( (_LANGUAGES_ MATCHES C) OR (_LANGUAGES_ MATCHES CXX) )
    set( _CHECK_FORTRAN FALSE )
  else()
    if(BLAS_FIND_REQUIRED)
      message(FATAL_ERROR "FindBLAS requires Fortran, C, or C++ to be enabled.")
    else()
      message(STATUS "Looking for BLAS... - NOT found (Unsupported languages)")
      return()
    endif()
  endif()
  
  macro(check_blas_Libraries LIBRARIES _prefix _name _flags _list _thread)
    # This macro checks for the existence of the combination of libraries
    # given by _list.  If the combination is found, this macro checks (using the
    # Check_Fortran_Function_Exists and/or Check_function_exists macros) whether
    # we can link against that library
    # combination using the name of a routine given by _name using the linker
    # flags given by _flags.  If the combination of libraries is found and passes
    # the link test, LIBRARIES is set to the list of complete library paths that
    # have been found.  Otherwise, LIBRARIES is set to FALSE.
    
    # N.B. _prefix is the prefix applied to the names of all cached variables that
    # are generated internally and marked advanced by this macro.
    
    set(_libdir ${ARGN})
    
    set(_libraries_work TRUE)
    set(${LIBRARIES})
    set(_combined_name)
    
    ## If no extra argument was given to the macro, default search path is
    ## filled with environment variables.
    if (NOT _libdir)
      if (WIN32)
	set(_libdir LIB)
      elseif (APPLE)
	set(_libdir ENV DYLD_LIBRARY_PATH)
      else ()
	set(_libdir ENV LD_LIBRARY_PATH)
      endif ()
    endif ()
    foreach(_library ${_list})
      set(_combined_name ${_combined_name}_${_library})
      
      if(_libraries_work)
	if (BLA_STATIC)
	  if (WIN32)
            set(CMAKE_FIND_LIBRARY_SUFFIXES .lib ${CMAKE_FIND_LIBRARY_SUFFIXES})
	  endif ()
	  if (APPLE)
            set(CMAKE_FIND_LIBRARY_SUFFIXES .lib ${CMAKE_FIND_LIBRARY_SUFFIXES})
	  else ()
            set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
	  endif ()
	else ()
	  if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
            # for ubuntu's libblas3gf and liblapack3gf packages
            set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} .so.3gf)
	  endif ()
	endif ()
	
	# HINTS are checked before PATHS, that's why we call
	# find_library twice, to give priority to LD_LIBRARY_PATH or user-defined paths
	# over pkg-config process.
	find_library(${_prefix}_${_library}_LIBRARY
	  NAMES ${_library}
	  PATHS ${_libdir}
	  NO_DEFAULT_PATH
	  )
	pkg_check_modules(PC_LIBRARY QUIET ${_library})
	PRINT_VAR(PC_LIBRARY_LIBDIR)
	PRINT_VAR(PC_LIBRARY_LIBRARY_DIRS)
	find_library(${_prefix}_${_library}_LIBRARY
	  NAMES ${_library}
	  HINTS ${PC_LIBRARY_LIBDIR} ${PC_LIBRARY_LIBRARY_DIRS} 
	  )
	PRINT_VAR(${_prefix}_${_library}_LIBRARY)
	mark_as_advanced(${_prefix}_${_library}_LIBRARY)
	set(${LIBRARIES} ${${LIBRARIES}} ${${_prefix}_${_library}_LIBRARY})
	set(_libraries_work ${${_prefix}_${_library}_LIBRARY})
      endif()
    endforeach()
    if(_libraries_work)
      # Test this combination of libraries.
      set(CMAKE_REQUIRED_LIBRARIES ${_flags} ${${LIBRARIES}} ${_thread})
      #else()
      ## First we check cblas interface
      check_function_exists("cblas_${_name}" ${_prefix}${_combined_name}_WORKS)
      # and then, if required, fortran interface
      if (_CHECK_FORTRAN)
	check_fortran_function_exists("${_name}" ${_prefix}${_combined_name}_WORKS_F)
	if(NOT ${${_prefix}${_combined_name}_WORKS_F})
	  set(${_prefix}${_combined_name}_WORKS FALSE)
	endif()
      endif()
      set(CMAKE_REQUIRED_LIBRARIES)
      mark_as_advanced(${_prefix}${_combined_name}_WORKS)
      set(_libraries_work ${${_prefix}${_combined_name}_WORKS})
    endif()
    if(NOT _libraries_work)
      set(${LIBRARIES} FALSE)
    endif()
  endmacro()
  

  #### Start Blas search process ####
  set(WITH_BLAS "" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]")
  set(BLAS_DIR "" CACHE PATH "Blas implementation location.")
  
  if(BLAS_DIR) 
    set(CMAKE_PREFIX_PATH ${BLAS_DIR})
    string(TOLOWER ${BLAS_DIR} lowerblasdir)
  else()
    set(lowerblasdir)
  endif()
  
  
  if(BLAS_DIR MATCHES "mkl.*")
    set(WITH_BLAS "mkl" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
    set(INTEL_MKL_DIR ${BLAS_DIR})
    set(INTEL_COMPILER_DIR "${BLAS_DIR}/..")
  elseif(lowerblasdir MATCHES "atlas.*") 
    set(WITH_BLAS "atlas" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
  elseif(lowerblasdir MATCHES "openblas.*")
    set(WITH_BLAS "openblas" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
  elseif(lowerblasdir MATCHES "accelerate.framework.*")
    set(WITH_BLAS "accelerate" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
  elseif(lowerblasdir MATCHES "veclib.framework.*")
    set(WITH_BLAS "veclib" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
  #elseif(BLAS_DIR STREQUAL "") # if blas_dir is not given or corresponds to one of the previous tests, 
    # we set a generic vendor.
   # set(WITH_BLAS "generic" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
  endif()
  
  PRINT_VAR(BLA_VENDOR)
  PRINT_VAR(BLAS_DIR)
  PRINT_VAR(WITH_BLAS)
  set(BLAS_LIBRARIES)
  ## Intel MKL ## 
  if((NOT BLAS_LIBRARIES)
      AND ((NOT WITH_BLAS) OR (WITH_BLAS STREQUAL "mkl")))
    message(STATUS "Try to find blas in intel/mkl ...")
    find_package(MKL)
    if(MKL_FOUND)
      set(WITH_BLAS "mkl" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
      #set(BLA_VENDOR "mkl")
      set(BLAS_LIBRARIES ${MKL_LIBRARIES})
      set(BLAS_INCLUDE_DIR ${MKL_INCLUDE_DIR})
      set(BLAS_VERSION ${MKL_VERSION})
      set(BLAS_HEADERS mkl_cblas.h)
    endif(MKL_FOUND)
  endif()

  ## OpenBLAS ##
  if((NOT BLAS_LIBRARIES)
      AND ((NOT WITH_BLAS) OR (WITH_BLAS STREQUAL "openblas")))
    message(STATUS "Try to find blas in openblas ...")
    check_blas_libraries(
      BLAS_LIBRARIES
      BLAS
      sgemm
      ""
      "openblas"
      "")
    if(BLAS_LIBRARIES)
      set(WITH_BLAS "openblas" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
      #set(BLA_VENDOR "openblas")
      set(BLAS_HEADERS cblas.h)
      set(CMAKE_FIND_FRAMEWORK "Last")
    endif(BLAS_LIBRARIES)
  endif()
  
  ## Apple Framework ## 
  if((NOT BLAS_LIBRARIES)
      AND ((NOT WITH_BLAS) OR (WITH_BLAS STREQUAL "accelerate")))
    message(STATUS "Try to find blas in Accelerate framework ...")
    check_blas_libraries(
      BLAS_LIBRARIES
      BLAS
      sgemm
      ""
      "Accelerate"
      "")
    if (BLAS_LIBRARIES)
      set(WITH_BLAS "accelerate" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
      #set(BLAS_VENDOR "accelerate")
      set(BLAS_HEADERS cblas.h Accelerate.h)
    endif (BLAS_LIBRARIES)
  endif()
  if((NOT BLAS_LIBRARIES)
      AND ((NOT WITH_BLAS) OR (WITH_BLAS STREQUAL "veclib")))
    message(STATUS "Try to find blas in VecLib framework ...")
    check_blas_libraries(
      BLAS_LIBRARIES
      BLAS
      sgemm
      ""
      "vecLib"
      "")
    if (BLAS_LIBRARIES)
      set(WITH_BLAS "veclib" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
      set(BLAS_HEADERS cblas.h vecLib.h)
    endif (BLAS_LIBRARIES)
  endif()
  
  ## Atlas ## 
  if((NOT BLAS_LIBRARIES)
      AND ((NOT WITH_BLAS) OR (WITH_BLAS STREQUAL "atlas")))
    message(STATUS "Try to find blas in atlas ...")
    check_blas_libraries(
      BLAS_LIBRARIES
      BLAS
      sgemm
      ""
      "f77blas;atlas"
      "")
    if (BLAS_LIBRARIES)
      set(WITH_BLAS "atlas" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
      set(BLAS_HEADERS cblas.h)
      set(BLAS_INCLUDE_SUFFIXES atlas)
    endif (BLAS_LIBRARIES)
  endif()

  ## Generic BLAS library? ##
  if((NOT BLAS_LIBRARIES)
      AND ((NOT WITH_BLAS) OR (WITH_BLAS STREQUAL "generic")))
    message(STATUS "Try to find a generic blas ...")
    check_blas_libraries(
      BLAS_LIBRARIES
      BLAS
      sgemm
      ""
      "blas"
      "")
    if (BLAS_LIBRARIES)
      set(WITH_BLAS "generic" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
      set(BLAS_HEADERS cblas.h)
    endif (BLAS_LIBRARIES)
  endif()
  ## Generic BLAS library (if cblas not found in blas) ##
  if((NOT BLAS_LIBRARIES)
      AND ((NOT WITH_BLAS) OR (WITH_BLAS STREQUAL "generic")))
    check_blas_libraries(
      BLAS_LIBRARIES
      BLAS
      sgemm
      ""
      "f77blas;cblas"
      "")
    if (BLAS_LIBRARIES)
      set(WITH_BLAS "generic" CACHE STRING "Blas implementation type [mkl/openblas/atlas/accelerate/veclib/generic]" FORCE)
      set(BLAS_HEADERS cblas.h)
    endif (BLAS_LIBRARIES)
  endif()
  if(BLAS_LIBRARIES)
    set(BLAS_FOUND TRUE)
  else(BLAS_LIBRARIES)
    set(BLAS_FOUND FALSE)
  endif(BLAS_LIBRARIES)
  
  ## Now the headers ...
  if(BLAS_FOUND)
    unset(BLAS_FOUND CACHE)
    # Get an hint for the location of blas headers : probably BLAS_LIBRARIES/../include or BLAS_DIR if given
    list(GET BLAS_LIBRARIES 0 BLAS_LIB)
    get_filename_component(_bdir ${BLAS_LIB} PATH)
    set(BLAS_LIBRARY_DIR ${_bdir} CACHE PATH "Blas libraries location." FORCE)
    
    if(NOT BLAS_DIR)
      get_filename_component(_bdir ${BLAS_LIBRARY_DIR} PATH)
      set(BLAS_DIR ${_bdir} CACHE PATH "Blas implementation location." FORCE)
    endif()
    
    PRINT_VAR(BLAS_LIBRARY_DIR)
    PRINT_VAR(BLAS_DIR)
    
    set(BLAS_INC_DIR ${BLAS_DIR}/include)
    set(CMAKE_INCLUDE_PATH ${BLAS_INC_DIR})
    set(CMAKE_PREFIX_PATH ${BLAS_DIR})
    ## Note Franck : it seems that find_path process does not work as expected on Macosx : it does not respect the search sequence described
    # in cmake doc (i.e. CMAKE_PREFIX, then HINTS, PATHS ... ) and always turn to find apple framework cblas if
    # NO_DEFAULT_PATH is not set. 
    if(APPLE) # First check in HINTS, no default, then global search.
      foreach(_file ${BLAS_HEADERS})
	unset(_dir CACHE)
	PRINT_VAR(_file)
	find_path(_dir
	  NAMES ${_file}
	  HINTS ${BLAS_DIR} ${BLAS_INC_DIR}
	  PATH_SUFFIXES ${LAPACK_INCLUDE_SUFFIXES}
	  NO_DEFAULT_PATH
	  )
	find_path(_dir 
	  NAMES ${_file}
	  PATH_SUFFIXES ${LAPACK_INCLUDE_SUFFIXES}
	  )
	PRINT_VAR(_dir)
	list(APPEND BLAS_INCLUDE_DIRS ${_dir})
      endforeach()
      unset(_dir CACHE)
      set(BLAS_INCLUDE_DIRS ${BLAS_INCLUDE_DIRS} CACHE STRING "Blas headers location." FORCE)
    else() # The case which is supposed to always work
      find_path(BLAS_INCLUDE_DIRS 
	NAMES ${BLAS_HEADERS}
	PATH_SUFFIXES ${LAPACK_INCLUDE_SUFFIXES}
	)
    endif()
    PRINT_VAR(BLAS_DIR)
    PRINT_VAR(BLAS_INCLUDE_DIRS)
    PRINT_VAR(BLAS_LIBRARIES)
    if(BLAS_INCLUDE_DIRS)
      set(BLAS_FOUND 1)
    endif()
    
  endif()

  if(BLAS_FOUND) # NumericsConfig.h setup
    set(HAS_CBLAS 1 CACHE BOOL "A CBlas implementation is available.")

    if(WITH_BLAS STREQUAL "mkl")
      set(HAS_MKL_CBLAS 1 CACHE BOOL "Blas comes from Intel MKL.")
   
    elseif(WITH_BLAS STREQUAL "accelerate")
      set(HAS_ACCELERATE 1 CACHE BOOL "Blas/Lapack come from Accelerate framework ")
      
    elseif(WITH_BLAS STREQUAL "atlas")
      set(HAS_ATLAS_CBLAS 1 CACHE BOOL "Blas  comes from Atlas framework ")

    elseif(WITH_BLAS STREQUAL "openblas")
      set(HAS_OpenBLAS 1 CACHE BOOL "Blas/Lapack come from OpenBlas ")
      
    else()
      set(HAS_GenericCBLAS 1 CACHE BOOL "Blas is available from an unknown version.")

    endif()
  endif()

  if(NOT BLAS_FOUND AND BLAS_FIND_REQUIRED)
    message(FATAL_ERROR "Cannot find a library with BLAS API. Please specify library location using BLAS_DIR option or set your environment variables properly.")
  endif (NOT BLAS_FOUND AND BLAS_FIND_REQUIRED)
  if(NOT BLAS_FIND_QUIETLY)
    if(BLAS_FOUND)
      message(STATUS "Found a library with BLAS API (${BLA_VENDOR}).")
    else(BLAS_FOUND)
      message(STATUS "Cannot find a library with BLAS API. Maybe you can try again using BLAS_DIR option or set your environment variables properly.")
    endif(BLAS_FOUND)
  endif(NOT BLAS_FIND_QUIETLY)
  
endif()

