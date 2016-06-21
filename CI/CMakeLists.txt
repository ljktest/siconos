# Configurations used for continuous integration
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(Tools)

if(CI_CONFIG)
  string(REPLACE "," ";" CI_CONFIG_LIST ${CI_CONFIG})
  FOREACH(_CI ${CI_CONFIG_LIST})
    include(config/${_CI}.cmake)
  ENDFOREACH(_CI CI_CONFIG_LIST)
endif()

# Check/set uninitialized cache variables (--> for docker config)
get_uninitialized_vars(CMAKE_ARGS_FROM_CACHE)


# ---------- Docker or not docker ... ------------------------
if(WITH_DOCKER)
  find_program(HAVE_DOCKER docker)
  if(HAVE_DOCKER)
    set(DOCKER_MKSENV_COMMAND ${CMAKE_SOURCE_DIR}/machinery/mksenv.py)
    set(DOCKER_MKSENV_INPUT ${CMAKE_SOURCE_DIR}/config/siconos.yml)
    set(DOCKER_REPOSITORY ci_driver)
    set(DOCKER_CMAKE_FLAGS ${CMAKE_ARGS_FROM_CACHE};-DWITH_DOCKER=0;-V)
    set(DOCKER_CTEST_DRIVER ${CMAKE_SOURCE_DIR}/machinery/CTestDriver.cmake)
    set(DOCKER_CTEST_MODE ${MODE})
    set(DOCKER_BUILD_CONFIGURATION ${BUILD_CONFIGURATION})
    include(Docker)
    
    add_docker_targets(
      DOCKER_IMAGE ${DOCKER_IMAGE}
      DOCKER_TEMPLATE ${DOCKER_TEMPLATE}
      DOCKER_IMAGE_DIR ${DOCKER_IMAGE_DIR}
      DOCKER_REPOSITORY ${DOCKER_REPOSITORY}
      DOCKER_CMAKE_FLAGS ${DOCKER_CMAKE_FLAGS}
      DOCKER_CTEST_DRIVER ${DOCKER_CTEST_DRIVER}
      DOCKER_PROJECT_SOURCE_DIR ${DOCKER_PROJECT_SOURCE_DIR})
      
    project(ci_driver NONE)
  else()
    message(FATAL_ERROR "Cannot find docker.")
  endif()
endif()