# from default, test solvers with sanitizer
include(CI/config/default.cmake)
set_option(WITH_HDF5 ON)
set_option(WITH_MUMPS ON)
set_option(USE_SANITIZER asan)
