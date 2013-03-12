# - Try to find Blas (including cblas) and a complete Lapack implementation.
# Once done, this will define
#
#  BLAS_FOUND - system has FFTW
#  BLAS_INCLUDE_DIRS - the directories  which contains (c)blas headers.
#  BLAS_LIBRARIES - link these to use blas.
#  
#  The same for Lapack. 
#
# Set BLAS_DIR=where_blas_is_installed if it's not in a "classic" place or if you want a specific version
#
# 
# inspired from http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries
# 
# Note : the following implementations have been succesfully tested for Siconos :
#  - netlib blas/cblas + lapacke 
#  - mkl cblas + lapacke
#  - OpenBlas (GotoBlas "offspring")
#  - Atlas 
#


## Check if FuncName is available in lapack lib (i.e in one of LAPACK_LIBRARIES)
## and if FuncName symbol is present in file Header. 
# If both are true, result is true.
function(check_lapack_has_function genericName FuncName Header result)
#  message(STATUS "func name : ${FuncName}")
#  message(STATUS "header name : ${Header}")
#  message(STATUS "result name : ${result}")
#  message(STATUS "genericName name : ${genericName}")
  
  check_function_exists(${FuncName} TEST_FUNC)
  check_symbol_exists(${FuncName} ${Header} TEST_HEAD)
  
  if(TEST_HEAD AND TEST_FUNC)
    set(${result} 1 CACHE BOOL "${genericName} is available in lapack.")
  endif()
    
endfunction()

# === Blas ===
set(WITH_CBLAS 1)
#if(USE_MKL)
#  set(BLA_VENDOR Intel10_64lp_seq)
#endif()
if(USE_MKL)
  set(BLA_VENDOR Intel10_64lp_seq)
  #set(BLA_F95 1)
  unset(WITH_CBLAS)
elseif(USE_ATLAS)
  set(BLA_VENDOR ATLAS)
elseif(USE_GOTO)
  set(BLA_VENDOR Goto)
elseif(USE_OpenBLAS)
  set(BLA_VENDOR OpenBLAS)
elseif(USE_APPLE_FRAMEWORK)
  set(BLA_VENDOR Apple)
endif()

compile_with(BLAS REQUIRED)

list(GET BLAS_LIBRARIES 0 BLAS_LIB)
get_filename_component(BLAS_DIR ${BLAS_LIB} PATH)
set(BLAS_DIR ${BLAS_DIR})

unset(BLAS_FOUND)
## --- mkl blas ---
if(BLAS_LIBRARIES MATCHES "mkl.*")
  message(STATUS "Check for blas headers in mkl ...")
  find_path(BLAS_INCLUDE_DIRS
    NAMES mkl_cblas.h
    PATHS ${BLAS_DIR}/..
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    )
  
  if(BLAS_INCLUDE_DIRS)
    set(BLAS_FOUND 1)
    set(HAS_MKL_CBLAS 1 CACHE BOOL "Blas comes from Intel MKL.")
    set(HAS_CBLAS 1 CACHE BOOL "A CBlas implementation is available.")
  endif()
## --- Apple framework blas ---
elseif(BLAS_LIBRARIES MATCHES "Accelerate.*")
  message(STATUS "Try to find apple headers for blas ...")
  find_path(CBLAS_INCLUDE_DIR
    NAMES cblas.h
    )
  find_path(ACC_INCLUDE_DIR
    NAMES Accelerate.h
    )
  set(BLAS_INCLUDE_DIRS ${CBLAS_INCLUDE_DIR} ${ACC_INCLUDE_DIR})
  print_var(BLAS_INCLUDE_DIRS)
  if(BLAS_INCLUDE_DIRS)
    set(BLAS_FOUND 1)
    set(HAS_ACCELERATE 1 CACHE BOOL "Blas/Lapack come from Accelerate framework ")
    set(HAS_CBLAS 1 CACHE BOOL "A CBlas implementation is available.")
  endif()

## --- Atlas blas ---
elseif(BLAS_LIBRARIES MATCHES "atlas.*")
  find_path(BLAS_INCLUDE_DIRS
    NAMES cblas.h
    PATHS ${BLAS_DIR}/..
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    )
  if(BLAS_INCLUDE_DIRS)
    set(BLAS_FOUND 1)
    set(HAS_ATLAS_CBLAS 1 CACHE BOOL "Blas  comes from Atlas framework ")
    set(HAS_CBLAS 1 CACHE BOOL "A CBlas implementation is available.")
  endif()

## --- OpenBlas (ex-Goto) ---
elseif(BLAS_LIBRARIES MATCHES "openblas.*")
  find_path(BLAS_INCLUDE_DIRS
    NAMES cblas.h
    PATHS ${BLAS_DIR}/..
    PATH_SUFFIXES include
    NO_DEFAULT_PATH
    )
  if(BLAS_INCLUDE_DIRS)
    set(BLAS_FOUND 1)
    set(HAS_OpenBLAS 1 CACHE BOOL "Blas/Lapack come from OpenBlas ")
    set(HAS_CBLAS 1 CACHE BOOL "A CBlas implementation is available.")
  endif()
endif()

include_directories(${BLAS_INCLUDE_DIRS})

message(STATUS "Blas Libraries : ${BLAS_LIBRARIES}")
message(STATUS "Blas include : ${BLAS_INCLUDE_DIRS}")
message(STATUS "Blas linker flags : ${BLAS_LINKER_FLAGS}")
message(STATUS "Blas found : ${BLAS_FOUND}")
message(STATUS "Blas Vendor : ${BLA_VENDOR}")

# === Lapack ===

# Warning FP : if Blas has been found in ATLAS (i.e. BLA_VENDOR==ATLAS from now on)
# FindLAPACK won't necessary look into atlas directories to find liblapack. Any available 
# lapack will be ok. 
find_package(LAPACK REQUIRED)

if(HAS_ACCELERATE)
  # if blas commes from accelerate, no need to check lapack headers,
  # they are the same. 
  #set(LAPACK_INCLUDE_DIRS ${BLAS_INCLUDE_DIRS})
  find_path(LAPACK_INCLUDE_DIRS
    NAMES clapack.h
    PATHS ${BLAS_INCLUDE_DIRS}
    NO_DEFAULT_PATH
    )
  set(LAPACK_HEADER ${LAPACK_INCLUDE_DIRS}/clapack.h)
  set(LAPACK_SUFFIX "_")
  set(LAPACK_PREFIX)

else()
  #set(LAPACK_FOUND 1)
  list(GET LAPACK_LIBRARIES 0 LAPACK_LIB)
  get_filename_component(LAPACK_DIR ${LAPACK_LIB} PATH)
  set(LAPACK_DIR ${LAPACK_DIR}/..)

  unset(LAPACK_FOUND)
  if(HAS_MKL_CBLAS)
    message(STATUS "Try to find lapack headers in mkl ...")
    find_path(LAPACK_INCLUDE_DIRS
      NAMES mkl_lapacke.h
      PATHS ${LAPACK_DIR}
      PATH_SUFFIXES include
      NO_DEFAULT_PATH
      )
    
    if(LAPACK_INCLUDE_DIRS)
      set(LAPACK_FOUND 1)
      set(HAS_MKL_LAPACKE 1 CACHE BOOL "LAPACK comes from Intel MKL.")
      set(LAPACK_HEADER ${LAPACK_INCLUDE_DIRS}/mkl_lapacke.h)
      set(LAPACK_SUFFIX)
      set(LAPACK_PREFIX "LAPACKE_")
    endif()
  elseif(HAS_ATLAS_CBLAS)
    message("Try to find lapack headers in atlas ...")
#    find_path(LAPACK_INCLUDE_DIRS
#      NAMES lapacke.h
#      PATHS ${LAPACK_DIR}
#      PATH_SUFFIXES include include/atlas
#      NO_DEFAULT_PATH
#      )
    find_path(LAPACK_INCLUDE_DIRS
      NAMES clapack.h
      PATHS ${LAPACK_DIR}
      PATH_SUFFIXES include include/atlas
      NO_DEFAULT_PATH
      )
    if(LAPACK_INCLUDE_DIRS)
      set(LAPACK_FOUND 1)
      set(HAS_ATLAS_LAPACK 1 CACHE BOOL "LAPACK comes from atlas.")
      set(LAPACK_HEADER ${LAPACK_INCLUDE_DIRS}/clapack.h)
      set(LAPACK_SUFFIX)
      set(LAPACK_PREFIX "clapack_")
    endif()
  else() # netlib, openblas
    find_path(LAPACK_INCLUDE_DIRS
      NAMES lapacke.h
      PATHS ${LAPACK_DIR}
      PATH_SUFFIXES include
      NO_DEFAULT_PATH
      )
    if(LAPACK_INCLUDE_DIRS)
      set(LAPACK_FOUND 1)
      set(HAS_LAPACKE 1 PARENT_SCOPE)
      set(LAPACK_HEADER ${LAPACK_INCLUDE_DIRS}/lapacke.h)
      set(LAPACK_SUFFIX)
      set(LAPACK_PREFIX "LAPACKE_")
    endif()
  endif()
endif()

include_directories(${LAPACK_INCLUDE_DIRS})
remember_link_libraries(${LAPACK_LIBRARIES})

message(STATUS "Lapack Libraries : ${LAPACK_LIBRARIES}")
message(STATUS "Lapack include : ${LAPACK_INCLUDE_DIRS}")
message(STATUS "Lapack linker flags : ${LAPACK_LINKER_FLAGS}")
message(STATUS "Lapack found : ${LAPACK_FOUND}")


# === Check for lapack functions ===
# We check only the functions that are known to be un-implemented
# in some lapack libraries (namely atlas ...)

set(CMAKE_REQUIRED_LIBRARIES ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
set(CMAKE_REQUIRED_INCLUDES ${BLAS_INCLUDE_DIRS} ${LAPACK_INCLUDE_DIRS})
## dgesvd ##
unset(HAS_LAPACK_DGESVD)
set(GENERIC_NAME "DGESVD")
set(FUNC_NAME "${LAPACK_PREFIX}dgesvd${LAPACK_SUFFIX}")
check_lapack_has_function(${GENERIC_NAME} ${FUNC_NAME} ${LAPACK_HEADER} HAS_LAPACK_DGESVD)

## dgels ##
unset(HAS_LAPACK_DGELS)
set(GENERIC_NAME "DGELS")
set(FUNC_NAME "${LAPACK_PREFIX}dgels${LAPACK_SUFFIX}")
check_lapack_has_function(${GENERIC_NAME} ${FUNC_NAME} ${LAPACK_HEADER} HAS_LAPACK_DGELS)

## dtrtrs ##
unset(HAS_LAPACK_DTRTRS)
set(GENERIC_NAME "DTRTRS")
set(FUNC_NAME "${LAPACK_PREFIX}dtrtrs${LAPACK_SUFFIX}")
check_lapack_has_function(${GENERIC_NAME} ${FUNC_NAME} ${LAPACK_HEADER} HAS_LAPACK_DTRTRS)



