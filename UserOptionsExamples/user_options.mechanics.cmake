# ================================================================
# All the default values for siconos cmake parameters
#
# Usage:
# cmake path-to-sources
#  --> to keep default value
# 
# cmake path-to-sources -DWITH_PYTHON_BINDINGS=ON
#  --> to enable (ON), or disable (OFF) the concerned option.
#
# For details about all these options check siconos install guide.
# ================================================================

# --------- User-defined options ---------
# Use cmake -DOPTION_NAME=some-value ... to modify default value.
option(WITH_DOCUMENTATION "Build Documentation. Default = OFF" OFF)
option(WITH_PYTHON_BINDINGS "Build python bindings using swig. Default = ON" ON)
option(WITH_DOXYGEN_WARNINGS "Explore doxygen warnings." ON)
option(WITH_DOXY2SWIG "Build swig docstrings from doxygen xml output. Default = ON." ON)
option(WITH_SYSTEM_INFO "Verbose mode to get some system/arch details. Default = off." OFF)
option(WITH_TESTING "Enable 'make test' target" ON)
option(WITH_GIT "Consider sources are under GIT" OFF)
option(WITH_SERIALIZATION "Compilation of serialization functions. Default = OFF" OFF)
option(WITH_GENERATION "Generation of serialization functions with gccxml. Default = OFF" OFF)
option(WITH_CXX "Enable CXX compiler for Numerics. Default=ON." ON)
option(WITH_UNSTABLE "Enable this to include all 'unstable' sources. Default=OFF" OFF)
option(BUILD_SHARED_LIBS "Building of shared libraries" ON)
option(DEV_MODE "Compilation flags setup for developpers. Default: ON" OFF)
option(WITH_BULLET "compilation with Bullet Bindings. Default = OFF" OFF)
option(WITH_OCC "compilation with OpenCascade Bindings. Default = OFF" ON)
option(WITH_MUMPS "Compilation with MUMPS solver. Default = OFF" OFF)
option(WITH_FCLIB "link with fclib when this mode is enable. Default = off." OFF)
option(WITH_FREECAD "Use FreeCAD" OFF)
option(WITH_MECHANISMS "Generation of bindings for Saladyn Mechanisms toolbox" ON)
option(WITH_XML "Enable xml files i/o. Default = ON" ON)

# List of components to build and installed
# List of siconos component to be installed
# complete list = Numerics Kernel Control Mechanics IO
set(COMPONENTS_DIRS Numerics Kernel Control Mechanics IO CACHE INTERNAL "List of siconos components to build and install")
