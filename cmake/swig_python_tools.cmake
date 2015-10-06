# =======================================
# Macros and functions related to swig
# and python
# =======================================

# -----------------------------------
# Build targets to generate python
# docstrings from xml doxygen output.
#
# Warning : xml files must exist
# before any call to this macro!!
# -----------------------------------
macro(doxy2swig_docstrings COMP)
  if(WITH_${COMPONENT}_DOXY2SWIG)
    set(DOCSTRINGS_FILES)
    # for each header of the current component ...
    foreach(_F ${${COMP}_HDRS})
      get_filename_component(_XFWE ${_F} NAME_WE)
      get_filename_component(_EXT ${_F} EXT)
      string(REPLACE "_" "__" _FWE ${_XFWE})
      file(GLOB ${_FWE}_XMLS
	${DOXYGEN_OUTPUT}/xml/*class${_FWE}.xml
	${DOXYGEN_OUTPUT}/xml/*struct${_FWE}.xml
	${DOXYGEN_OUTPUT}/xml/${_FWE}_8h*.xml)
      foreach(_FXML ${${_FWE}_XMLS})
	get_filename_component(_FWE_XML ${_FXML} NAME_WE)
	set(outfile_name ${SICONOS_SWIG_ROOT_DIR}/${_FWE_XML}.i)
	add_custom_command(OUTPUT ${outfile_name} 
          DEPENDS ${DOXYGEN_OUTPUT}/xml/${_FWE_XML}.xml
          COMMAND ${PYTHON_EXECUTABLE}
	  ARGS "${CMAKE_BINARY_DIR}/share/doxy2swig.py"
	  ${DOXYGEN_OUTPUT}/xml/${_FWE_XML}.xml ${outfile_name}
          COMMENT "docstrings generation for ${_FWE} (parsing ${_FWE_XML}.xml)")
	add_custom_target(doc_${_FWE_XML}.i DEPENDS ${outfile_name})
	list(APPEND DOCSTRINGS_FILES ${outfile_name})
      endforeach()
    endforeach()
    
    add_custom_command(OUTPUT ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i
      DEPENDS ${DOCSTRINGS_FILES}
      COMMAND cat
      ARGS ${DOCSTRINGS_FILES} > ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i
      COMMENT "${COMP} docstrings concatenation")
    add_custom_target(${COMP}_docstrings DEPENDS ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i)
  else()
    add_custom_command(OUTPUT ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i
      DEPENDS ${DOCSTRINGS_FILES}
      COMMAND echo "" > ${COMP}-docstrings.i
      )
    
    add_custom_target(${COMP}_docstrings DEPENDS ${SICONOS_SWIG_ROOT_DIR}/${COMP}-docstrings.i)
  endif()
  # Doxygen doc must be built before a call to the targets above.
  #add_dependencies(${COMP}_OUTPUT doc)
endmacro()

# ----------------------------------------------------------------------
# Build a swig module from .i files
#
# Usage :
# add_siconos_swig_submodule(full_name)
#
# full_name must be a path to file.i, path relative to siconos
# python package root.
# For example, to build a swig module 'bullet' in siconos.mechanics.contact_detection,
# call this macro with full_name = mechanics/contact_detection/bullet
# and to build a swig module for kernel in siconos,
# call this macro with full_name = ./kernel
#
# ----------------------------------------------------------------------
macro(add_siconos_swig_sub_module fullname)
  get_filename_component(_name ${fullname} NAME)
  get_filename_component(_path ${fullname} DIRECTORY)
  
  message(" -- Build module ${_name} in directory ${_path} for parent ${COMPONENT}")
  # Add component dependencies to the current submodule deps.
  if(DEFINED SWIG_MODULE_${COMPONENT}_EXTRA_DEPS)
    set(SWIG_MODULE_${_name}_EXTRA_DEPS ${SWIG_MODULE_${COMPONENT}_EXTRA_DEPS})
  endif()
  
  # add as dependencies all the i files
  file(GLOB ${_name}_I_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/*.i)
  foreach(_f ${${_name}_I_FILES})
    list(APPEND SWIG_MODULE_${_name}_EXTRA_DEPS ${_f})
  endforeach()

  # set properties for current '.i' file
  set(swig_file ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/${_name}.i)

  # set output dir
  set(CMAKE_SWIG_OUTDIR "${SICONOS_SWIG_ROOT_DIR}/${_path}")

  # compile flags
  foreach(_dir ${${COMPONENT}_SWIG_INCLUDE_DIRECTORIES})
    set(${COMPONENT}_SWIG_DEFS "-I${_dir};${${COMPONENT}_SWIG_DEFS}")
  endforeach()
      
  set_source_files_properties(${swig_file}
    PROPERTIES SWIG_FLAGS "${${COMPONENT}_SWIG_DEFS}" CPLUSPLUS ON)
  
  # --- build swig module ---
  swig_add_module(${_name} python ${swig_file})

  # WARNING ${swig_generated_file_fullname} is overriden 
  set(${_name}_generated_file_fullname ${swig_generated_file_fullname})
  
  # Set path for the library generated by swig for the current module --> siconos python package path
  set_property(TARGET ${SWIG_MODULE_${_name}_REAL_NAME} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${SICONOS_SWIG_ROOT_DIR}/${_path})
  message(" -- ${_name} generated (swig) file will be ${${_name}_generated_file_fullname}")

  # Check dependencies and then link ...
  add_dependencies(${SWIG_MODULE_${_name}_REAL_NAME} ${COMPONENT})
  swig_link_libraries(${_name} ${PYTHON_LIBRARIES} ${SICONOS_LINK_LIBRARIES} ${COMPONENT})

  # set dep between docstrings and python bindings
  add_dependencies(${SWIG_MODULE_${_name}_REAL_NAME} ${COMPONENT}_docstrings)

  # set dependency of sphinx apidoc to this target
  if(USE_SPHINX)
    add_dependencies(apidoc ${SWIG_MODULE_${_name}_REAL_NAME})
  endif()
  
  # Copy __init__.py file if needed
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/__init__.py.in)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_path}/__init__.py.in
      ${SICONOS_SWIG_ROOT_DIR}/${_path}/__init__.py @ONLY)
  elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_path}/__init__.py)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${_path}/__init__.py
      ${SICONOS_SWIG_ROOT_DIR}/${_path}/__init__.py COPYONLY)
  endif()

  # --- install python files and target ---
  # install path ...
  set(DEST "${SICONOS_PYTHON_INSTALL_DIR}/${SICONOS_PYTHON_PACKAGE}/${_path}")

  # done by python setup.py ...
  #install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${_name}.py DESTINATION ${DEST})
  install(TARGETS ${SWIG_MODULE_${_name}_REAL_NAME} DESTINATION ${DEST})
  
endmacro()

macro(build_plugin plug)
  get_filename_component(plug_name ${plug} NAME_WE)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tests/plugins/)
  add_library(${plug_name} MODULE ${plug})
  set_property(TARGET ${plug_name} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${SICONOS_SWIG_ROOT_DIR}/plugins)
  set_target_properties(${plug_name} PROPERTIES PREFIX "")
  add_dependencies(${COMPONENT} ${plug_name})
endmacro()

macro(swig_module_setup modules_list)
  if(WITH_${COMPONENT}_PYTHON_BINDINGS)
    # we should use lowercase name for python module (pep8 ...)
    message(" -- Prepare python bindings for component ${COMPONENT} ...")
    # build python bindings
    include_directories(${SICONOS_SWIG_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
    foreach(module ${${COMPONENT}_PYTHON_MODULES})
      add_siconos_swig_sub_module(${module})
    endforeach()
  endif()
endmacro()

include(tools4tests)
