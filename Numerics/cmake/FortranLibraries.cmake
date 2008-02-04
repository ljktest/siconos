# cache result to avoid re-running
#
if(NOT HAVE_FORTRAN_LIBRARIES)

# search for libraries required to link with fortran
# placed in the variable
#  FORTRAN_LIBRARIES
# the location is in
#  FORTRAN_COMPILER_LIB_DIRECTORY

# Need Fortran support for this
ENABLE_LANGUAGE(Fortran)

# these two can be used by the user to guide the process

GET_FILENAME_COMPONENT(fortran_comp_bin_dir ${CMAKE_Fortran_COMPILER} PATH)
GET_FILENAME_COMPONENT(fortran_comp_dir ${fortran_comp_bin_dir}       PATH)

SET(FORTRAN_COMPILER_DIRECTORY ${fortran_comp_dir} CACHE PATH "location of fortran compiler top directory")
SET(FORTRAN_COMPILER_LIB_DIRECTORY ${fortran_comp_dir}/lib CACHE PATH "location of fortran compiler lib directory")

SET(KNOWN_FORTRAN_LIBRARIES
   ""
   "gfortran"
   "fio\;f90math\;f77math"
   "afio\;af90math\;af77math\;amisc"
   "g2c"
)

# Define the list "options" of all possible schemes that we want to consider
# Get its length and initialize the counter "iopt" to zero
LIST(LENGTH KNOWN_FORTRAN_LIBRARIES imax)
SET(iopt 0)

# Try to link against fortran libraries
WHILE(${iopt} LESS ${imax})
    # Get the current list entry (current scheme)
    LIST(GET KNOWN_FORTRAN_LIBRARIES ${iopt} fc_libraries_t)

    STRING(REGEX REPLACE ";" "\\\;" fc_libraries "${fc_libraries_t}")

    FILE(WRITE ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/testF77libs.c
        "        void main(int argc, char ** argv) {}\n"
    )
    # Create a CMakeLists.txt file which will generate the "flib" library
    FILE(WRITE ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/CMakeLists.txt
        "PROJECT(FortranTest Fortran)\n"
        "SET(CMAKE_Fortran_FLAGS \"${TMP_Fortran_FLAGS}\")\n"
        "ADD_LIBRARY(flib ftest.f)\n"
        )
    # Create a simple Fortran source
    FILE(WRITE ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/ftest.f
        "        PROGRAM fortransub\n"
        "        print *, 'hello world'\n"
        "        print *, ' value = ', atan(-1.0)\n"
        "        STOP\n"
        "        END\n"
    )

    # Use TRY_COMPILE to make the target "flib"
    TRY_COMPILE(
        FTEST_OK
        ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp
        ${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp
        flib
        OUTPUT_VARIABLE MY_OUTPUT)
    #message("${MY_OUTPUT}")
    if(NOT FTEST_OK)
        MESSAGE(FATAL_ERROR "Failed to compile simple fortran library")
    endif(NOT FTEST_OK)

    SET(trial_libraries "${fc_libraries}\;flib")
    SET(trial_lib_paths "${FORTRAN_COMPILER_LIB_DIRECTORY}\;${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp")

    MESSAGE(STATUS "trying libraries ${fc_libraries_t}")

    TRY_COMPILE(FORT_LIBS_WORK ${CMAKE_BINARY_DIR}
         ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/testF77libs.c
         CMAKE_FLAGS
         -DLINK_LIBRARIES:STRING=${trial_libraries}
         -DLINK_DIRECTORIES:STRING=${trial_lib_paths}
         OUTPUT_VARIABLE FORT_LIBS_BUILD_OUT
         )
    #MESSAGE(STATUS ${FORT_LIBS_BUILD_OUT})

    IF(FORT_LIBS_WORK)
        # the goal is to set this to something useful
        SET(FORTRAN_LIBRARIES ${fc_libraries} CACHE STRING "fortran libraries required to link with f90 generated code" FORCE)

        MESSAGE(STATUS "Using FORTRAN_LIBRARIES ${FORTRAN_LIBRARIES}")
        MESSAGE(STATUS "Using FORTRAN_COMPILER_LIB_DIRECTORY ${FORTRAN_COMPILER_LIB_DIRECTORY}")

        SET(HAVE_FORTRAN_LIBRARIES TRUE CACHE INTERNAL "successfully found fortran libraries")
        SET(iopt ${imax})
        LINK_DIRECTORIES(${FORTRAN_COMPILER_LIB_DIRECTORY})
    ELSE(FORT_LIBS_WORK)
        MATH(EXPR iopt ${iopt}+1)
    ENDIF(FORT_LIBS_WORK)
ENDWHILE(${iopt} LESS ${imax})

  IF(NOT HAVE_FORTRAN_LIBRARIES)
    MESSAGE(FATAL_ERROR "Could not find valid fortran link libraries. Try setting FORTRAN_COMPILER_DIRECTORY or FORTRAN_LIBRARIES")
  ENDIF(NOT HAVE_FORTRAN_LIBRARIES)

endif(NOT HAVE_FORTRAN_LIBRARIES)
