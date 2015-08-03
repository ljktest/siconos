# apply misc flags

INCLUDE(cxxVersion)
INCLUDE(cVersion)

macro(ADD_CXX_OPTIONS OPT)

 CHECK_CXX_ACCEPTS_FLAG("${OPT}" CXX_HAVE_"${OPT}")

 set(_compilers ${ARGN})
 IF(_compilers)
  SET(ADD_OPTION FALSE)
  FOREACH(_compiler ${_compilers})
   IF (${CMAKE_CXX_COMPILER_ID} MATCHES ${_compiler})
    SET(ADD_OPTION TRUE)
   ENDIF()
  ENDFOREACH()
 ELSE(_compilers)
  SET(ADD_OPTION TRUE)
 ENDIF(_compilers)

 IF(ADD_OPTION AND CXX_HAVE_"${OPT}")
  APPEND_CXX_FLAGS("${OPT}")
 ENDIF(ADD_OPTION AND CXX_HAVE_"${OPT}")

endmacro(ADD_CXX_OPTIONS)

macro(ADD_C_OPTIONS OPT)

 STRING(REGEX REPLACE " " "" OPT_SANE "${OPT}")
 CHECK_C_COMPILER_FLAG("${OPT}" C_HAVE_${OPT_SANE})

 set(_compilers ${ARGN})
 IF(_compilers)
  SET(ADD_OPTION FALSE)
  FOREACH(_compiler ${_compilers})
   IF (${CMAKE_C_COMPILER_ID} MATCHES ${_compiler})
    MESSAGE(STATUS "Adding option for compiler ${_compiler}")
    SET(ADD_OPTION TRUE)
   ENDIF()
  ENDFOREACH()
 ELSE(_compilers)
  SET(ADD_OPTION TRUE)
 ENDIF(_compilers)

 IF(ADD_OPTION AND C_HAVE_${OPT_SANE})
  APPEND_C_FLAGS("${OPT}")
 ENDIF(ADD_OPTION AND C_HAVE_${OPT_SANE})

endmacro(ADD_C_OPTIONS)


IF(CMAKE_C_COMPILER)
 INCLUDE(CheckCCompilerFlag)
 detect_c_version(C_VERSION)

 IF(C_VERSION STRLESS "201112L")
  SET(C_STD_VERSION "c99")
 ELSE(C_VERSION STRLESS "201112L")
  # default C standart is c11 or newer
  SET(C_STD_VERSION "c11")
 ENDIF(C_VERSION STRLESS "201112L")

 IF(NOT MSVC)
  ADD_C_OPTIONS("-std=${C_STD_VERSION}")
  ADD_C_OPTIONS("-x${C_STD_VERSION}")
 ENDIF(NOT MSVC)

 # ADD_C_OPTIONS("-static -static-libgcc" "GNU;Clang")
 # way too verbose with MSVC
 IF(NOT MSVC)
  ADD_C_OPTIONS("-Wall")
 ENDIF(NOT MSVC)
 ADD_C_OPTIONS("-Werror=overloaded-virtual")
 ADD_C_OPTIONS("-Wextra -Wno-unused-parameter")
 ADD_C_OPTIONS("-Werror=implicit-function-declaration")
 ADD_C_OPTIONS("-Werror=conversion -Wno-sign-conversion -Wno-error=sign-conversion -Wno-error=shorten-64-to-32")
 ADD_C_OPTIONS("-Werror=switch-bool")
 ADD_C_OPTIONS("-Werror=logical-not-parentheses")
 ADD_C_OPTIONS("-Werror=sizeof-array-argument")
 ADD_C_OPTIONS("-Werror=bool-compare")
 ADD_C_OPTIONS("-Werror=array-bounds")

 # C specific
 ADD_C_OPTIONS("-Werror=missing-prototypes")

 # Compiler Specific
 ADD_C_OPTIONS("-diag-disable 654" "Intel")
 IF(NOT ICCOK)
  ADD_C_OPTIONS("-D__aligned__=ignored" "Intel")
 ENDIF(NOT ICCOK)

 ADD_C_OPTIONS("-Wno-string-plus-int" "Clang")
 ADD_C_OPTIONS("-Werror=unreachable-code" "Clang")

 # too many errors right now ...
 #IF(C_HAVE_MISS)
 #  APPEND_C_FLAGS("-Wmissing-prototypes")
 #ENDIF(C_HAVE_MISS)

ENDIF(CMAKE_C_COMPILER)

IF(CMAKE_CXX_COMPILER)
 detect_cxx_version(CXX_VERSION)
 INCLUDE(TestCXXAcceptsFlag)
 # ADD_CXX_OPTIONS("-static -static-libgcc -static-libstdc++" "GNU;Clang")
 # way too verbose with MSVC
 IF(NOT MSVC)
  ADD_CXX_OPTIONS("-Wall")
 ENDIF(NOT MSVC)
 ADD_CXX_OPTIONS("-Werror=overloaded-virtual")
 ADD_CXX_OPTIONS("-Wextra -Wno-unused-parameter")
 ADD_CXX_OPTIONS("-Werror=implicit-function-declaration")
 ADD_CXX_OPTIONS("-Werror=conversion -Wno-sign-conversion -Wno-error=sign-conversion -Wno-error=shorten-64-to-32" C_HAVE_CONV)
 ADD_CXX_OPTIONS("-Werror=missing-declarations")
 ADD_CXX_OPTIONS("-Werror=switch-bool")
 ADD_CXX_OPTIONS("-Werror=logical-not-parentheses")
 ADD_CXX_OPTIONS("-Werror=sizeof-array-argument")
 ADD_CXX_OPTIONS("-Werror=bool-compare")
 ADD_CXX_OPTIONS("-Werror=array-bounds")

 ADD_CXX_OPTIONS("-Wodr")

 IF(NOT CXX_VERSION STRLESS "201102L" AND DEV_MODE)
  ADD_CXX_OPTIONS("-Wsuggest-final-types")
  ADD_CXX_OPTIONS("-Wsuggest-final-methods")
  ADD_CXX_OPTIONS("-Wzero-as-null-pointer-constant")
 ENDIF()

 # Compiler Specific
 ADD_CXX_OPTIONS("-diag-disable 654" "Intel")
 IF(NOT ICCOK)
  ADD_CXX_OPTIONS("-D__aligned__=ignored" "Intel")
 ENDIF(NOT ICCOK)

 ADD_CXX_OPTIONS("-Wno-string-plus-int" "Clang")
 ADD_CXX_OPTIONS("-Werror=unreachable-code" "Clang")

ENDIF(CMAKE_CXX_COMPILER)
