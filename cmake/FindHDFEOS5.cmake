# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindHDFEOS5
--------

Find Hierarchical Data Format (HDFEOS5), a library for reading and writing
self describing array data.


This module invokes the ``HDFEOS5`` wrapper compiler that should be installed
alongside ``HDFEOS5``.  Depending upon the ``HDFEOS5`` Configuration, the wrapper
compiler is called either ``h5cc`` or ``h5pcc``.  If this succeeds, the module
will then call the compiler with the show argument to see what flags
are used when compiling an ``HDFEOS5`` client application.

The module will optionally accept the ``COMPONENTS`` argument.  If no
``COMPONENTS`` are specified, then the find module will default to finding
only the ``HDFEOS5`` C library.  If one or more ``COMPONENTS`` are specified, the
module will attempt to find the language bindings for the specified
components.  The valid components are ``C``, ``CXX``, ``Fortran``, ``HL``.
``HL`` refers to the "high-level" HDFEOS5 functions for C and Fortran.
If the ``COMPONENTS`` argument is not given, the module will
attempt to find only the C bindings.
For example, to use Fortran HDFEOS5 and HDFEOS5-HL functions, do:
``find_package(HDFEOS5 COMPONENTS Fortran HL)``.

This module will read the variable
``HDFEOS5_USE_STATIC_LIBRARIES`` to determine whether or not to prefer a
static link to a dynamic link for ``HDFEOS5`` and all of it's dependencies.
To use this feature, make sure that the ``HDFEOS5_USE_STATIC_LIBRARIES``
variable is set before the call to find_package.

Both the serial and parallel ``HDFEOS5`` wrappers are considered and the first
directory to contain either one will be used.  In the event that both appear
in the same directory the serial version is preferentially selected. This
behavior can be reversed by setting the variable ``HDFEOS5_PREFER_PARALLEL`` to
``TRUE``.

In addition to finding the includes and libraries required to compile
an ``HDFEOS5`` client application, this module also makes an effort to find
tools that come with the ``HDFEOS5`` distribution that may be useful for
regression testing.

Result Variables
^^^^^^^^^^^^^^^^

This module will set the following variables in your project:

``HDFEOS5_FOUND``
  HDFEOS5 was found on the system
``HDFEOS5_VERSION``
  HDFEOS5 library version
``HDFEOS5_INCLUDE_DIRS``
  Location of the HDFEOS5 header files
``HDFEOS5_DEFINITIONS``
  Required compiler definitions for HDFEOS5
``HDFEOS5_LIBRARIES``
  Required libraries for all requested bindings
``HDFEOS5_HL_LIBRARIES``
  Required libraries for the HDFEOS5 high level API for all bindings,
  if the ``HL`` component is enabled

Available components are: ``C`` ``CXX`` ``Fortran`` and ``HL``.
For each enabled language binding, a corresponding ``HDFEOS5_${LANG}_LIBRARIES``
variable, and potentially ``HDFEOS5_${LANG}_DEFINITIONS``, will be defined.
If the ``HL`` component is enabled, then an ``HDFEOS5_${LANG}_HL_LIBRARIES`` will
also be defined.  With all components enabled, the following variables will be defined:

``HDFEOS5_C_DEFINITIONS``
  Required compiler definitions for HDFEOS5 C bindings
``HDFEOS5_CXX_DEFINITIONS``
  Required compiler definitions for HDFEOS5 C++ bindings
``HDFEOS5_Fortran_DEFINITIONS``
  Required compiler definitions for HDFEOS5 Fortran bindings
``HDFEOS5_C_INCLUDE_DIRS``
  Required include directories for HDFEOS5 C bindings
``HDFEOS5_CXX_INCLUDE_DIRS``
  Required include directories for HDFEOS5 C++ bindings
``HDFEOS5_Fortran_INCLUDE_DIRS``
  Required include directories for HDFEOS5 Fortran bindings
``HDFEOS5_C_LIBRARIES``
  Required libraries for the HDFEOS5 C bindings
``HDFEOS5_CXX_LIBRARIES``
  Required libraries for the HDFEOS5 C++ bindings
``HDFEOS5_Fortran_LIBRARIES``
  Required libraries for the HDFEOS5 Fortran bindings
``HDFEOS5_C_HL_LIBRARIES``
  Required libraries for the high level C bindings
``HDFEOS5_CXX_HL_LIBRARIES``
  Required libraries for the high level C++ bindings
``HDFEOS5_Fortran_HL_LIBRARIES``
  Required libraries for the high level Fortran bindings.

``HDFEOS5_IS_PARALLEL``
  HDFEOS5 library has parallel IO support
``HDFEOS5_C_COMPILER_EXECUTABLE``
  path to the HDFEOS5 C wrapper compiler
``HDFEOS5_CXX_COMPILER_EXECUTABLE``
  path to the HDFEOS5 C++ wrapper compiler
``HDFEOS5_Fortran_COMPILER_EXECUTABLE``
  path to the HDFEOS5 Fortran wrapper compiler
``HDFEOS5_C_COMPILER_EXECUTABLE_NO_INTERROGATE``
  path to the primary C compiler which is also the HDFEOS5 wrapper
``HDFEOS5_CXX_COMPILER_EXECUTABLE_NO_INTERROGATE``
  path to the primary C++ compiler which is also the HDFEOS5 wrapper
``HDFEOS5_Fortran_COMPILER_EXECUTABLE_NO_INTERROGATE``
  path to the primary Fortran compiler which is also the HDFEOS5 wrapper
``HDFEOS5_DIFF_EXECUTABLE``
  path to the HDFEOS5 dataset comparison tool

Hints
^^^^^

The following variables can be set to guide the search for HDFEOS5 libraries and includes:

``HDFEOS5_PREFER_PARALLEL``
  set ``true`` to prefer parallel HDFEOS5 (by default, serial is preferred)

``HDFEOS5_FIND_DEBUG``
  Set ``true`` to get extra debugging output.

``HDFEOS5_NO_FIND_PACKAGE_CONFIG_FILE``
  Set ``true`` to skip trying to find ``hdfeos5-config.cmake``.
#]=======================================================================]

# This module is maintained by Will Dicharry <wdicharry@stellarscience.com>.

include(${CMAKE_CURRENT_LIST_DIR}/SelectLibraryConfigurations.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)

# List of the valid HDFEOS5 components
set(HDFEOS5_VALID_LANGUAGE_BINDINGS C CXX Fortran)

# Validate the list of find components.
if(NOT HDFEOS5_FIND_COMPONENTS)
  set(HDFEOS5_LANGUAGE_BINDINGS "C")
else()
  set(HDFEOS5_LANGUAGE_BINDINGS)
  # add the extra specified components, ensuring that they are valid.
  set(HDFEOS5_FIND_HL OFF)
  foreach(_component IN LISTS HDFEOS5_FIND_COMPONENTS)
    list(FIND HDFEOS5_VALID_LANGUAGE_BINDINGS ${_component} _component_location)
    if(NOT _component_location EQUAL -1)
      list(APPEND HDFEOS5_LANGUAGE_BINDINGS ${_component})
    elseif(_component STREQUAL "HL")
      set(HDFEOS5_FIND_HL ON)
    elseif(_component STREQUAL "Fortran_HL") # only for compatibility
      list(APPEND HDFEOS5_LANGUAGE_BINDINGS Fortran)
      set(HDFEOS5_FIND_HL ON)
      set(HDFEOS5_FIND_REQUIRED_Fortran_HL FALSE)
      set(HDFEOS5_FIND_REQUIRED_Fortran TRUE)
      set(HDFEOS5_FIND_REQUIRED_HL TRUE)
    else()
      message(FATAL_ERROR "${_component} is not a valid HDFEOS5 component.")
    endif()
  endforeach()
  unset(_component)
  unset(_component_location)
  if(NOT HDFEOS5_LANGUAGE_BINDINGS)
    get_property(_langs GLOBAL PROPERTY ENABLED_LANGUAGES)
    foreach(_lang IN LISTS _langs)
      if(_lang MATCHES "^(C|CXX|Fortran)$")
        list(APPEND HDFEOS5_LANGUAGE_BINDINGS ${_lang})
      endif()
    endforeach()
  endif()
  list(REMOVE_ITEM HDFEOS5_FIND_COMPONENTS Fortran_HL) # replaced by Fortran and HL
  list(REMOVE_DUPLICATES HDFEOS5_LANGUAGE_BINDINGS)
endif()

# Determine whether to search for serial or parallel executable first
if(HDFEOS5_PREFER_PARALLEL)
  set(HDFEOS5_C_COMPILER_NAMES h5pcc h5cc)
  set(HDFEOS5_CXX_COMPILER_NAMES h5pc++ h5c++)
  set(HDFEOS5_Fortran_COMPILER_NAMES h5pfc h5fc)
else()
  set(HDFEOS5_C_COMPILER_NAMES h5cc h5pcc)
  set(HDFEOS5_CXX_COMPILER_NAMES h5c++ h5pc++)
  set(HDFEOS5_Fortran_COMPILER_NAMES h5fc h5pfc)
endif()

# We may have picked up some duplicates in various lists during the above
# process for the language bindings (both the C and C++ bindings depend on
# libz for example).  Remove the duplicates. It appears that the default
# CMake behavior is to remove duplicates from the end of a list. However,
# for link lines, this is incorrect since unresolved symbols are searched
# for down the link line. Therefore, we reverse the list, remove the
# duplicates, and then reverse it again to get the duplicates removed from
# the beginning.
macro(_HDFEOS5_remove_duplicates_from_beginning _list_name)
  if(${_list_name})
    list(REVERSE ${_list_name})
    list(REMOVE_DUPLICATES ${_list_name})
    list(REVERSE ${_list_name})
  endif()
endmacro()


# Test first if the current compilers automatically wrap HDFEOS5

function(_HDFEOS5_test_regular_compiler_C success version is_parallel)
  set(scratch_directory
    ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hdfeos5)
  if(NOT ${success} OR
     NOT EXISTS ${scratch_directory}/compiler_has_h5_c)
    set(test_file ${scratch_directory}/cmake_hdfeos5_test.c)
    file(WRITE ${test_file}
      "#include <hdfeos5.h>\n"
      "const char* info_ver = \"INFO\" \":\" H5_VERSION;\n"
      "#ifdef H5_HAVE_PARALLEL\n"
      "const char* info_parallel = \"INFO\" \":\" \"PARALLEL\";\n"
      "#endif\n"
      "int main(int argc, char **argv) {\n"
      "  int require = 0;\n"
      "  require += info_ver[argc];\n"
      "#ifdef H5_HAVE_PARALLEL\n"
      "  require += info_parallel[argc];\n"
      "#endif\n"
      "  hid_t fid;\n"
      "  fid = H5Fcreate(\"foo.h5\",H5F_ACC_TRUNC,H5P_DEFAULT,H5P_DEFAULT);\n"
      "  return 0;\n"
      "}")
    try_compile(${success} ${scratch_directory} ${test_file}
      COPY_FILE ${scratch_directory}/compiler_has_h5_c
    )
  endif()
  if(${success})
    file(STRINGS ${scratch_directory}/compiler_has_h5_c INFO_STRINGS
      REGEX "^INFO:"
    )
    string(REGEX MATCH "^INFO:([0-9]+\\.[0-9]+\\.[0-9]+)(-patch([0-9]+))?"
      INFO_VER "${INFO_STRINGS}"
    )
    set(${version} ${CMAKE_MATCH_1})
    if(CMAKE_MATCH_3)
      set(${version} ${HDFEOS5_C_VERSION}.${CMAKE_MATCH_3})
    endif()
    set(${version} ${${version}} PARENT_SCOPE)

    if(INFO_STRINGS MATCHES "INFO:PARALLEL")
      set(${is_parallel} TRUE PARENT_SCOPE)
    else()
      set(${is_parallel} FALSE PARENT_SCOPE)
    endif()
  endif()
endfunction()

function(_HDFEOS5_test_regular_compiler_CXX success version is_parallel)
  set(scratch_directory ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hdfeos5)
  if(NOT ${success} OR
     NOT EXISTS ${scratch_directory}/compiler_has_h5_cxx)
    set(test_file ${scratch_directory}/cmake_hdfeos5_test.cxx)
    file(WRITE ${test_file}
      "#include <H5Cpp.h>\n"
      "#ifndef H5_NO_NAMESPACE\n"
      "using namespace H5;\n"
      "#endif\n"
      "const char* info_ver = \"INFO\" \":\" H5_VERSION;\n"
      "#ifdef H5_HAVE_PARALLEL\n"
      "const char* info_parallel = \"INFO\" \":\" \"PARALLEL\";\n"
      "#endif\n"
      "int main(int argc, char **argv) {\n"
      "  int require = 0;\n"
      "  require += info_ver[argc];\n"
      "#ifdef H5_HAVE_PARALLEL\n"
      "  require += info_parallel[argc];\n"
      "#endif\n"
      "  H5File file(\"foo.h5\", H5F_ACC_TRUNC);\n"
      "  return 0;\n"
      "}")
    try_compile(${success} ${scratch_directory} ${test_file}
      COPY_FILE ${scratch_directory}/compiler_has_h5_cxx
    )
  endif()
  if(${success})
    file(STRINGS ${scratch_directory}/compiler_has_h5_cxx INFO_STRINGS
      REGEX "^INFO:"
    )
    string(REGEX MATCH "^INFO:([0-9]+\\.[0-9]+\\.[0-9]+)(-patch([0-9]+))?"
      INFO_VER "${INFO_STRINGS}"
    )
    set(${version} ${CMAKE_MATCH_1})
    if(CMAKE_MATCH_3)
      set(${version} ${HDFEOS5_CXX_VERSION}.${CMAKE_MATCH_3})
    endif()
    set(${version} ${${version}} PARENT_SCOPE)

    if(INFO_STRINGS MATCHES "INFO:PARALLEL")
      set(${is_parallel} TRUE PARENT_SCOPE)
    else()
      set(${is_parallel} FALSE PARENT_SCOPE)
    endif()
  endif()
endfunction()

function(_HDFEOS5_test_regular_compiler_Fortran success is_parallel)
  if(NOT ${success})
    set(scratch_directory
      ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hdfeos5)
    set(test_file ${scratch_directory}/cmake_hdfeos5_test.f90)
    file(WRITE ${test_file}
      "program hdfeos5_hello\n"
      "  use hdfeos5\n"
      "  use h5lt\n"
      "  use h5ds\n"
      "  integer error\n"
      "  call h5open_f(error)\n"
      "  call h5close_f(error)\n"
      "end\n")
    try_compile(${success} ${scratch_directory} ${test_file})
    if(${success})
      execute_process(COMMAND ${CMAKE_Fortran_COMPILER} -showconfig
        OUTPUT_VARIABLE config_output
        ERROR_VARIABLE config_error
        RESULT_VARIABLE config_result
        )
      if(config_output MATCHES "Parallel HDFEOS5: yes")
        set(${is_parallel} TRUE PARENT_SCOPE)
      else()
        set(${is_parallel} FALSE PARENT_SCOPE)
      endif()
    endif()
  endif()
endfunction()

# Invoke the HDFEOS5 wrapper compiler.  The compiler return value is stored to the
# return_value argument, the text output is stored to the output variable.
function( _HDFEOS5_invoke_compiler language output_var return_value_var version_var is_parallel_var)
  set(is_parallel FALSE)
  if(HDFEOS5_USE_STATIC_LIBRARIES)
    set(lib_type_args -noshlib)
  else()
    set(lib_type_args -shlib)
  endif()
  set(scratch_dir ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/hdfeos5)
  if("${language}" STREQUAL "C")
    set(test_file ${scratch_dir}/cmake_hdfeos5_test.c)
  elseif("${language}" STREQUAL "CXX")
    set(test_file ${scratch_dir}/cmake_hdfeos5_test.cxx)
  elseif("${language}" STREQUAL "Fortran")
    set(test_file ${scratch_dir}/cmake_hdfeos5_test.f90)
  endif()
  # Verify that the compiler wrapper can actually compile: sometimes the compiler
  # wrapper exists, but not the compiler.  E.g. Miniconda / Anaconda Python
  execute_process(
    COMMAND ${HDFEOS5_${language}_COMPILER_EXECUTABLE} ${test_file}
    RESULT_VARIABLE return_value
    )
  if(return_value)
    message(STATUS
      "HDFEOS5 ${language} compiler wrapper is unable to compile a minimal HDFEOS5 program.")
  else()
    execute_process(
      COMMAND ${HDFEOS5_${language}_COMPILER_EXECUTABLE} -show ${lib_type_args} ${test_file}
      OUTPUT_VARIABLE output
      ERROR_VARIABLE output
      RESULT_VARIABLE return_value
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    if(return_value)
      message(STATUS
        "Unable to determine HDFEOS5 ${language} flags from HDFEOS5 wrapper.")
    endif()
    execute_process(
      COMMAND ${HDFEOS5_${language}_COMPILER_EXECUTABLE} -showconfig
      OUTPUT_VARIABLE config_output
      ERROR_VARIABLE config_output
      RESULT_VARIABLE return_value
      OUTPUT_STRIP_TRAILING_WHITESPACE
      )
    if(return_value)
      message(STATUS
        "Unable to determine HDFEOS5 ${language} version_var from HDFEOS5 wrapper.")
    endif()
    string(REGEX MATCH "HDFEOS5 Version: ([a-zA-Z0-9\\.\\-]*)" version "${config_output}")
    if(version)
      string(REPLACE "HDFEOS5 Version: " "" version "${version}")
      string(REPLACE "-patch" "." version "${version}")
    endif()
    if(config_output MATCHES "Parallel HDFEOS5: yes")
      set(is_parallel TRUE)
    endif()
  endif()
  foreach(var output return_value version is_parallel)
    set(${${var}_var} ${${var}} PARENT_SCOPE)
  endforeach()
endfunction()

# Parse a compile line for definitions, includes, library paths, and libraries.
function(_HDFEOS5_parse_compile_line compile_line_var include_paths definitions
    library_paths libraries libraries_hl)

  separate_arguments(_compile_args NATIVE_COMMAND "${${compile_line_var}}")

  foreach(_arg IN LISTS _compile_args)
    if("${_arg}" MATCHES "^-I(.*)$")
      # include directory
      list(APPEND include_paths "${CMAKE_MATCH_1}")
    elseif("${_arg}" MATCHES "^-D(.*)$")
      # compile definition
      list(APPEND definitions "-D${CMAKE_MATCH_1}")
    elseif("${_arg}" MATCHES "^-L(.*)$")
      # library search path
      list(APPEND library_paths "${CMAKE_MATCH_1}")
    elseif("${_arg}" MATCHES "^-l(hdfeos5.*hl.*)$")
      # library name (hl)
      list(APPEND libraries_hl "${CMAKE_MATCH_1}")
    elseif("${_arg}" MATCHES "^-l(.*)$")
      # library name
      list(APPEND libraries "${CMAKE_MATCH_1}")
    elseif("${_arg}" MATCHES "^(.:)?[/\\].*\\.(a|so|dylib|sl|lib)$")
      # library file
      if(NOT EXISTS "${_arg}")
        continue()
      endif()
      get_filename_component(_lpath "${_arg}" DIRECTORY)
      get_filename_component(_lname "${_arg}" NAME_WE)
      string(REGEX REPLACE "^lib" "" _lname "${_lname}")
      list(APPEND library_paths "${_lpath}")
      if(_lname MATCHES "hdfeos5.*hl")
        list(APPEND libraries_hl "${_lname}")
      else()
        list(APPEND libraries "${_lname}")
      endif()
    endif()
  endforeach()
  foreach(var include_paths definitions library_paths libraries libraries_hl)
    set(${${var}_var} ${${var}} PARENT_SCOPE)
  endforeach()
endfunction()

# Select a preferred imported configuration from a target
function(_HDFEOS5_select_imported_config target imported_conf)
    # We will first assign the value to a local variable _imported_conf, then assign
    # it to the function argument at the end.
    get_target_property(_imported_conf ${target} MAP_IMPORTED_CONFIG_${CMAKE_BUILD_TYPE})
    if (NOT _imported_conf)
        # Get available imported configurations by examining target properties
        get_target_property(_imported_conf ${target} IMPORTED_CONFIGURATIONS)
        if(HDFEOS5_FIND_DEBUG)
            message(STATUS "Found imported configurations: ${_imported_conf}")
        endif()
        # Find the imported configuration that we prefer.
        # We do this by making list of configurations in order of preference,
        # starting with ${CMAKE_BUILD_TYPE} and ending with the first imported_conf
        set(_preferred_confs ${CMAKE_BUILD_TYPE})
        list(GET _imported_conf 0 _fallback_conf)
        list(APPEND _preferred_confs RELWITHDEBINFO RELEASE DEBUG ${_fallback_conf})
        if(HDFEOS5_FIND_DEBUG)
            message(STATUS "Start search through imported configurations in the following order: ${_preferred_confs}")
        endif()
        # Now find the first of these that is present in imported_conf
        cmake_policy(PUSH)
        cmake_policy(SET CMP0057 NEW) # support IN_LISTS
        foreach (_conf IN LISTS _preferred_confs)
            if (${_conf} IN_LIST _imported_conf)
               set(_imported_conf ${_conf})
               break()
            endif()
        endforeach()
        cmake_policy(POP)
    endif()
    if(HDFEOS5_FIND_DEBUG)
        message(STATUS "Selected imported configuration: ${_imported_conf}")
    endif()
    # assign value to function argument
    set(${imported_conf} ${_imported_conf} PARENT_SCOPE)
endfunction()


if(NOT HDFEOS5_ROOT)
    set(HDFEOS5_ROOT $ENV{HDFEOS5_ROOT})
endif()
if(HDFEOS5_ROOT)
    set(_HDFEOS5_SEARCH_OPTS NO_DEFAULT_PATH)
else()
    set(_HDFEOS5_SEARCH_OPTS)
endif()

# Try to find HDFEOS5 using an installed hdfeos5-config.cmake
if(NOT HDFEOS5_FOUND AND NOT HDFEOS5_NO_FIND_PACKAGE_CONFIG_FILE)
    find_package(HDFEOS5 QUIET NO_MODULE
      HINTS ${HDFEOS5_ROOT}
      ${_HDFEOS5_SEARCH_OPTS}
      )
    if( HDFEOS5_FOUND)
        if(HDFEOS5_FIND_DEBUG)
            message(STATUS "Found HDFEOS5 at ${HDFEOS5_DIR} via NO_MODULE. Now trying to extract locations etc.")
        endif()
        set(HDFEOS5_IS_PARALLEL ${HDFEOS5_ENABLE_PARALLEL})
        set(HDFEOS5_INCLUDE_DIRS ${HDFEOS5_INCLUDE_DIR})
        set(HDFEOS5_LIBRARIES)
        if (NOT TARGET hdfeos5 AND NOT TARGET hdfeos5-static AND NOT TARGET hdfeos5-shared)
            # Some HDFEOS5 versions (e.g. 1.8.18) used hdfeos5::hdfeos5 etc
            set(_target_prefix "hdfeos5::")
        endif()
        set(HDFEOS5_C_TARGET ${_target_prefix}hdfeos5)
        set(HDFEOS5_C_HL_TARGET ${_target_prefix}hdfeos5_hl)
        set(HDFEOS5_CXX_TARGET ${_target_prefix}hdfeos5_cpp)
        set(HDFEOS5_CXX_HL_TARGET ${_target_prefix}hdfeos5_hl_cpp)
        set(HDFEOS5_Fortran_TARGET ${_target_prefix}hdfeos5_fortran)
        set(HDFEOS5_Fortran_HL_TARGET ${_target_prefix}hdfeos5_hl_fortran)
        set(HDFEOS5_DEFINITIONS "")
        if(HDFEOS5_USE_STATIC_LIBRARIES)
            set(_suffix "-static")
        else()
            set(_suffix "-shared")
        endif()
        foreach(_lang ${HDFEOS5_LANGUAGE_BINDINGS})

            #Older versions of hdfeos5 don't have a static/shared suffix so
            #if we detect that occurrence clear the suffix
            if(_suffix AND NOT TARGET ${HDFEOS5_${_lang}_TARGET}${_suffix})
              if(NOT TARGET ${HDFEOS5_${_lang}_TARGET})
                #can't find this component with or without the suffix
                #so bail out, and let the following locate HDFEOS5
                set(HDFEOS5_FOUND FALSE)
                break()
              endif()
              set(_suffix "")
            endif()

            if(HDFEOS5_FIND_DEBUG)
                message(STATUS "Trying to get properties of target ${HDFEOS5_${_lang}_TARGET}${_suffix}")
            endif()
            # Find library for this target. Complicated as on Windows with a DLL, we need to search for the import-lib.
            _HDFEOS5_select_imported_config(${HDFEOS5_${_lang}_TARGET}${_suffix} _hdfeos5_imported_conf)
            get_target_property(_hdfeos5_lang_location ${HDFEOS5_${_lang}_TARGET}${_suffix} IMPORTED_IMPLIB_${_hdfeos5_imported_conf} )
            if (NOT _hdfeos5_lang_location)
                # no import lib, just try LOCATION
                get_target_property(_hdfeos5_lang_location ${HDFEOS5_${_lang}_TARGET}${_suffix} LOCATION_${_hdfeos5_imported_conf})
                if (NOT _hdfeos5_lang_location)
                    get_target_property(_hdfeos5_lang_location ${HDFEOS5_${_lang}_TARGET}${_suffix} LOCATION)
                endif()
            endif()
            if( _hdfeos5_lang_location )
                set(HDFEOS5_${_lang}_LIBRARY ${_hdfeos5_lang_location})
                list(APPEND HDFEOS5_LIBRARIES ${HDFEOS5_${_lang}_TARGET}${_suffix})
                set(HDFEOS5_${_lang}_LIBRARIES ${HDFEOS5_${_lang}_TARGET}${_suffix})
                set(HDFEOS5_${_lang}_FOUND TRUE)
            endif()
            if(HDFEOS5_FIND_HL)
                get_target_property(_lang_hl_location ${HDFEOS5_${_lang}_HL_TARGET}${_suffix} IMPORTED_IMPLIB_${_hdfeos5_imported_conf} )
                if (NOT _hdfeos5_lang_hl_location)
                    get_target_property(_hdfeos5_lang_hl_location ${HDFEOS5_${_lang}_HL_TARGET}${_suffix} LOCATION_${_hdfeos5_imported_conf})
                    if (NOT _hdfeos5_hl_lang_location)
                        get_target_property(_hdfeos5_hl_lang_location ${HDFEOS5_${_lang}_HL_TARGET}${_suffix} LOCATION)
                    endif()
                endif()
                if( _hdfeos5_lang_hl_location )
                    set(HDFEOS5_${_lang}_HL_LIBRARY ${_hdfeos5_lang_hl_location})
                    list(APPEND HDFEOS5_HL_LIBRARIES ${HDFEOS5_${_lang}_HL_TARGET}${_suffix})
                    set(HDFEOS5_${_lang}_HL_LIBRARIES ${HDFEOS5_${_lang}_HL_TARGET}${_suffix})
                    set(HDFEOS5_HL_FOUND TRUE)
                endif()
                unset(_hdfeos5_lang_hl_location)
            endif()
            unset(_hdfeos5_imported_conf)
            unset(_hdfeos5_lang_location)
        endforeach()
    endif()
endif()

if(NOT HDFEOS5_FOUND)
  set(_HDFEOS5_NEED_TO_SEARCH FALSE)
  set(HDFEOS5_COMPILER_NO_INTERROGATE TRUE)
  # Only search for languages we've enabled
  foreach(_lang IN LISTS HDFEOS5_LANGUAGE_BINDINGS)
    # First check to see if our regular compiler is one of wrappers
    if(_lang STREQUAL "C")
      _HDFEOS5_test_regular_compiler_C(
        HDFEOS5_${_lang}_COMPILER_NO_INTERROGATE
        HDFEOS5_${_lang}_VERSION
        HDFEOS5_${_lang}_IS_PARALLEL)
    elseif(_lang STREQUAL "CXX")
      _HDFEOS5_test_regular_compiler_CXX(
        HDFEOS5_${_lang}_COMPILER_NO_INTERROGATE
        HDFEOS5_${_lang}_VERSION
        HDFEOS5_${_lang}_IS_PARALLEL)
    elseif(_lang STREQUAL "Fortran")
      _HDFEOS5_test_regular_compiler_Fortran(
        HDFEOS5_${_lang}_COMPILER_NO_INTERROGATE
        HDFEOS5_${_lang}_IS_PARALLEL)
    else()
      continue()
    endif()
    if(HDFEOS5_${_lang}_COMPILER_NO_INTERROGATE)
      if(HDFEOS5_FIND_DEBUG)
        message(STATUS "HDFEOS5: Using hdfeos5 compiler wrapper for all ${_lang} compiling")
      endif()
      set(HDFEOS5_${_lang}_FOUND TRUE)
      set(HDFEOS5_${_lang}_COMPILER_EXECUTABLE_NO_INTERROGATE
          "${CMAKE_${_lang}_COMPILER}"
          CACHE FILEPATH "HDFEOS5 ${_lang} compiler wrapper")
      set(HDFEOS5_${_lang}_DEFINITIONS)
      set(HDFEOS5_${_lang}_INCLUDE_DIRS)
      set(HDFEOS5_${_lang}_LIBRARIES)
      set(HDFEOS5_${_lang}_HL_LIBRARIES)

      mark_as_advanced(HDFEOS5_${_lang}_COMPILER_EXECUTABLE_NO_INTERROGATE)

      set(HDFEOS5_${_lang}_FOUND TRUE)
      set(HDFEOS5_HL_FOUND TRUE)
    else()
      set(HDFEOS5_COMPILER_NO_INTERROGATE FALSE)
      # If this language isn't using the wrapper, then try to seed the
      # search options with the wrapper
      find_program(HDFEOS5_${_lang}_COMPILER_EXECUTABLE
        NAMES ${HDFEOS5_${_lang}_COMPILER_NAMES} NAMES_PER_DIR
        HINTS ${HDFEOS5_ROOT}
        PATH_SUFFIXES bin Bin
        DOC "HDFEOS5 ${_lang} Wrapper compiler.  Used only to detect HDFEOS5 compile flags."
        ${_HDFEOS5_SEARCH_OPTS}
      )
      mark_as_advanced( HDFEOS5_${_lang}_COMPILER_EXECUTABLE )
      unset(HDFEOS5_${_lang}_COMPILER_NAMES)

      if(HDFEOS5_${_lang}_COMPILER_EXECUTABLE)
        _HDFEOS5_invoke_compiler(${_lang} HDFEOS5_${_lang}_COMPILE_LINE
          HDFEOS5_${_lang}_RETURN_VALUE HDFEOS5_${_lang}_VERSION HDFEOS5_${_lang}_IS_PARALLEL)
        if(HDFEOS5_${_lang}_RETURN_VALUE EQUAL 0)
          if(HDFEOS5_FIND_DEBUG)
            message(STATUS "HDFEOS5: Using hdfeos5 compiler wrapper to determine ${_lang} configuration")
          endif()
          _HDFEOS5_parse_compile_line( HDFEOS5_${_lang}_COMPILE_LINE
            HDFEOS5_${_lang}_INCLUDE_DIRS
            HDFEOS5_${_lang}_DEFINITIONS
            HDFEOS5_${_lang}_LIBRARY_DIRS
            HDFEOS5_${_lang}_LIBRARY_NAMES
            HDFEOS5_${_lang}_HL_LIBRARY_NAMES
          )
          set(HDFEOS5_${_lang}_LIBRARIES)

          foreach(_lib IN LISTS HDFEOS5_${_lang}_LIBRARY_NAMES)
            set(_HDFEOS5_SEARCH_NAMES_LOCAL)
            if("x${_lib}" MATCHES "hdfeos5")
              # hdfeos5 library
              set(_HDFEOS5_SEARCH_OPTS_LOCAL ${_HDFEOS5_SEARCH_OPTS})
              if(HDFEOS5_USE_STATIC_LIBRARIES)
                if(WIN32)
                  set(_HDFEOS5_SEARCH_NAMES_LOCAL lib${_lib})
                else()
                  set(_HDFEOS5_SEARCH_NAMES_LOCAL lib${_lib}.a)
                endif()
              endif()
            else()
              # external library
              set(_HDFEOS5_SEARCH_OPTS_LOCAL)
            endif()
            find_library(HDFEOS5_${_lang}_LIBRARY_${_lib}
              NAMES ${_HDFEOS5_SEARCH_NAMES_LOCAL} ${_lib} NAMES_PER_DIR
              HINTS ${HDFEOS5_${_lang}_LIBRARY_DIRS}
                    ${HDFEOS5_ROOT}
              ${_HDFEOS5_SEARCH_OPTS_LOCAL}
              )
            unset(_HDFEOS5_SEARCH_OPTS_LOCAL)
            unset(_HDFEOS5_SEARCH_NAMES_LOCAL)
            if(HDFEOS5_${_lang}_LIBRARY_${_lib})
              list(APPEND HDFEOS5_${_lang}_LIBRARIES ${HDFEOS5_${_lang}_LIBRARY_${_lib}})
            else()
              list(APPEND HDFEOS5_${_lang}_LIBRARIES ${_lib})
            endif()
          endforeach()
          if(HDFEOS5_FIND_HL)
            set(HDFEOS5_${_lang}_HL_LIBRARIES)
            foreach(_lib IN LISTS HDFEOS5_${_lang}_HL_LIBRARY_NAMES)
              set(_HDFEOS5_SEARCH_NAMES_LOCAL)
              if("x${_lib}" MATCHES "hdfeos5")
                # hdfeos5 library
                set(_HDFEOS5_SEARCH_OPTS_LOCAL ${_HDFEOS5_SEARCH_OPTS})
                if(HDFEOS5_USE_STATIC_LIBRARIES)
                  if(WIN32)
                    set(_HDFEOS5_SEARCH_NAMES_LOCAL lib${_lib})
                  else()
                    set(_HDFEOS5_SEARCH_NAMES_LOCAL lib${_lib}.a)
                  endif()
                endif()
              else()
                # external library
                set(_HDFEOS5_SEARCH_OPTS_LOCAL)
              endif()
              find_library(HDFEOS5_${_lang}_LIBRARY_${_lib}
                NAMES ${_HDFEOS5_SEARCH_NAMES_LOCAL} ${_lib} NAMES_PER_DIR
                HINTS ${HDFEOS5_${_lang}_LIBRARY_DIRS}
                      ${HDFEOS5_ROOT}
                ${_HDFEOS5_SEARCH_OPTS_LOCAL}
                )
              unset(_HDFEOS5_SEARCH_OPTS_LOCAL)
              unset(_HDFEOS5_SEARCH_NAMES_LOCAL)
              if(HDFEOS5_${_lang}_LIBRARY_${_lib})
                list(APPEND HDFEOS5_${_lang}_HL_LIBRARIES ${HDFEOS5_${_lang}_LIBRARY_${_lib}})
              else()
                list(APPEND HDFEOS5_${_lang}_HL_LIBRARIES ${_lib})
              endif()
            endforeach()
            set(HDFEOS5_HL_FOUND TRUE)
          endif()

          set(HDFEOS5_${_lang}_FOUND TRUE)
          _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_${_lang}_DEFINITIONS)
          _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_${_lang}_INCLUDE_DIRS)
          _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_${_lang}_LIBRARIES)
          _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_${_lang}_HL_LIBRARIES)
        else()
          set(_HDFEOS5_NEED_TO_SEARCH TRUE)
        endif()
      else()
        set(_HDFEOS5_NEED_TO_SEARCH TRUE)
      endif()
    endif()
    if(HDFEOS5_${_lang}_VERSION)
      if(NOT HDFEOS5_VERSION)
        set(HDFEOS5_VERSION ${HDFEOS5_${_lang}_VERSION})
      elseif(NOT HDFEOS5_VERSION VERSION_EQUAL HDFEOS5_${_lang}_VERSION)
        message(WARNING "HDFEOS5 Version found for language ${_lang}, ${HDFEOS5_${_lang}_VERSION} is different than previously found version ${HDFEOS5_VERSION}")
      endif()
    endif()
    if(DEFINED HDFEOS5_${_lang}_IS_PARALLEL)
      if(NOT DEFINED HDFEOS5_IS_PARALLEL)
        set(HDFEOS5_IS_PARALLEL ${HDFEOS5_${_lang}_IS_PARALLEL})
      elseif(NOT HDFEOS5_IS_PARALLEL AND HDFEOS5_${_lang}_IS_PARALLEL)
        message(WARNING "HDFEOS5 found for language ${_lang} is parallel but previously found language is not parallel.")
      elseif(HDFEOS5_IS_PARALLEL AND NOT HDFEOS5_${_lang}_IS_PARALLEL)
        message(WARNING "HDFEOS5 found for language ${_lang} is not parallel but previously found language is parallel.")
      endif()
    endif()
  endforeach()
  unset(_lib)
else()
  set(_HDFEOS5_NEED_TO_SEARCH TRUE)
endif()

if(NOT HDFEOS5_FOUND AND HDFEOS5_COMPILER_NO_INTERROGATE)
  # No arguments necessary, all languages can use the compiler wrappers
  set(HDFEOS5_FOUND TRUE)
  set(HDFEOS5_METHOD "Included by compiler wrappers")
  set(HDFEOS5_REQUIRED_VARS HDFEOS5_METHOD)
elseif(NOT HDFEOS5_FOUND AND NOT _HDFEOS5_NEED_TO_SEARCH)
  # Compiler wrappers aren't being used by the build but were found and used
  # to determine necessary include and library flags
  set(HDFEOS5_INCLUDE_DIRS)
  set(HDFEOS5_LIBRARIES)
  set(HDFEOS5_HL_LIBRARIES)
  foreach(_lang IN LISTS HDFEOS5_LANGUAGE_BINDINGS)
    if(HDFEOS5_${_lang}_FOUND)
      if(NOT HDFEOS5_${_lang}_COMPILER_NO_INTERROGATE)
        list(APPEND HDFEOS5_DEFINITIONS ${HDFEOS5_${_lang}_DEFINITIONS})
        list(APPEND HDFEOS5_INCLUDE_DIRS ${HDFEOS5_${_lang}_INCLUDE_DIRS})
        list(APPEND HDFEOS5_LIBRARIES ${HDFEOS5_${_lang}_LIBRARIES})
        if(HDFEOS5_FIND_HL)
          list(APPEND HDFEOS5_HL_LIBRARIES ${HDFEOS5_${_lang}_HL_LIBRARIES})
        endif()
      endif()
    endif()
  endforeach()
  _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_DEFINITIONS)
  _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_INCLUDE_DIRS)
  _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_LIBRARIES)
  _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_HL_LIBRARIES)
  set(HDFEOS5_FOUND TRUE)
  set(HDFEOS5_REQUIRED_VARS HDFEOS5_LIBRARIES)
  if(HDFEOS5_FIND_HL)
    list(APPEND HDFEOS5_REQUIRED_VARS HDFEOS5_HL_LIBRARIES)
  endif()
endif()

find_program( HDFEOS5_DIFF_EXECUTABLE
    NAMES h5diff
    HINTS ${HDFEOS5_ROOT}
    PATH_SUFFIXES bin Bin
    ${_HDFEOS5_SEARCH_OPTS}
    DOC "HDFEOS5 file differencing tool." )
mark_as_advanced( HDFEOS5_DIFF_EXECUTABLE )

if( NOT HDFEOS5_FOUND )
    # seed the initial lists of libraries to find with items we know we need
    set(HDFEOS5_C_LIBRARY_NAMES          hdfeos5)
    set(HDFEOS5_C_HL_LIBRARY_NAMES       hdfeos5_hl ${HDFEOS5_C_LIBRARY_NAMES} )

    set(HDFEOS5_CXX_LIBRARY_NAMES        hdfeos5_cpp    ${HDFEOS5_C_LIBRARY_NAMES})
    set(HDFEOS5_CXX_HL_LIBRARY_NAMES     hdfeos5_hl_cpp ${HDFEOS5_C_HL_LIBRARY_NAMES} ${HDFEOS5_CXX_LIBRARY_NAMES})

    set(HDFEOS5_Fortran_LIBRARY_NAMES    hdfeos5_fortran   ${HDFEOS5_C_LIBRARY_NAMES})
    set(HDFEOS5_Fortran_HL_LIBRARY_NAMES hdfeos5hl_fortran ${HDFEOS5_C_HL_LIBRARY_NAMES} ${HDFEOS5_Fortran_LIBRARY_NAMES})

    # suffixes as seen on Linux, MSYS2, ...
    set(_lib_suffixes hdfeos5)
    if(NOT HDFEOS5_PREFER_PARALLEL)
      list(APPEND _lib_suffixes hdfeos5/serial)
    endif()
    if(HDFEOS5_USE_STATIC_LIBRARIES)
      set(_inc_suffixes include/static)
    else()
      set(_inc_suffixes include/shared)
    endif()

    foreach(_lang IN LISTS HDFEOS5_LANGUAGE_BINDINGS)
        # find the HDFEOS5 include directories
        if("${_lang}" STREQUAL "Fortran")
            set(HDFEOS5_INCLUDE_FILENAME hdfeos5.mod HDFEOS5.mod)
        elseif("${_lang}" STREQUAL "CXX")
            set(HDFEOS5_INCLUDE_FILENAME H5Cpp.h)
        else()
            set(HDFEOS5_INCLUDE_FILENAME hdfeos5.h)
        endif()

        find_path(HDFEOS5_${_lang}_INCLUDE_DIR ${HDFEOS5_INCLUDE_FILENAME}
            HINTS ${HDFEOS5_ROOT}
            PATHS $ENV{HOME}/.local/include
            PATH_SUFFIXES include Include ${_inc_suffixes} ${_lib_suffixes}
            ${_HDFEOS5_SEARCH_OPTS}
        )
        mark_as_advanced(HDFEOS5_${_lang}_INCLUDE_DIR)
        # set the _DIRS variable as this is what the user will normally use
        set(HDFEOS5_${_lang}_INCLUDE_DIRS ${HDFEOS5_${_lang}_INCLUDE_DIR})
        list(APPEND HDFEOS5_INCLUDE_DIRS ${HDFEOS5_${_lang}_INCLUDE_DIR})

        # find the HDFEOS5 libraries
        foreach(LIB IN LISTS HDFEOS5_${_lang}_LIBRARY_NAMES)
            if(HDFEOS5_USE_STATIC_LIBRARIES)
                # According to bug 1643 on the CMake bug tracker, this is the
                # preferred method for searching for a static library.
                # See https://gitlab.kitware.com/cmake/cmake/-/issues/1643.  We search
                # first for the full static library name, but fall back to a
                # generic search on the name if the static search fails.
                set( THIS_LIBRARY_SEARCH_DEBUG
                    lib${LIB}d.a lib${LIB}_debug.a lib${LIB}d lib${LIB}_D lib${LIB}_debug
                    lib${LIB}d-static.a lib${LIB}_debug-static.a ${LIB}d-static ${LIB}_D-static ${LIB}_debug-static )
                set( THIS_LIBRARY_SEARCH_RELEASE lib${LIB}.a lib${LIB} lib${LIB}-static.a ${LIB}-static)
            else()
                set( THIS_LIBRARY_SEARCH_DEBUG ${LIB}d ${LIB}_D ${LIB}_debug ${LIB}d-shared ${LIB}_D-shared ${LIB}_debug-shared)
                set( THIS_LIBRARY_SEARCH_RELEASE ${LIB} ${LIB}-shared)
                if(WIN32)
                  list(APPEND HDFEOS5_DEFINITIONS "-DH5_BUILT_AS_DYNAMIC_LIB")
                endif()
            endif()
            find_library(HDFEOS5_${LIB}_LIBRARY_DEBUG
                NAMES ${THIS_LIBRARY_SEARCH_DEBUG}
                HINTS ${HDFEOS5_ROOT} PATH_SUFFIXES lib Lib ${_lib_suffixes}
                ${_HDFEOS5_SEARCH_OPTS}
            )
            find_library(HDFEOS5_${LIB}_LIBRARY_RELEASE
                NAMES ${THIS_LIBRARY_SEARCH_RELEASE}
                HINTS ${HDFEOS5_ROOT} PATH_SUFFIXES lib Lib ${_lib_suffixes}
                ${_HDFEOS5_SEARCH_OPTS}
            )

            select_library_configurations( HDFEOS5_${LIB} )
            list(APPEND HDFEOS5_${_lang}_LIBRARIES ${HDFEOS5_${LIB}_LIBRARY})
        endforeach()
        if(HDFEOS5_${_lang}_LIBRARIES)
            set(HDFEOS5_${_lang}_FOUND TRUE)
        endif()

        # Append the libraries for this language binding to the list of all
        # required libraries.
        list(APPEND HDFEOS5_LIBRARIES ${HDFEOS5_${_lang}_LIBRARIES})

        if(HDFEOS5_FIND_HL)
            foreach(LIB IN LISTS HDFEOS5_${_lang}_HL_LIBRARY_NAMES)
                if(HDFEOS5_USE_STATIC_LIBRARIES)
                    # According to bug 1643 on the CMake bug tracker, this is the
                    # preferred method for searching for a static library.
                    # See https://gitlab.kitware.com/cmake/cmake/-/issues/1643.  We search
                    # first for the full static library name, but fall back to a
                    # generic search on the name if the static search fails.
                    set( THIS_LIBRARY_SEARCH_DEBUG
                        lib${LIB}d.a lib${LIB}_debug.a lib${LIB}d lib${LIB}_D lib${LIB}_debug
                        lib${LIB}d-static.a lib${LIB}_debug-static.a lib${LIB}d-static lib${LIB}_D-static lib${LIB}_debug-static )
                    set( THIS_LIBRARY_SEARCH_RELEASE lib${LIB}.a lib${LIB} lib${LIB}-static.a lib${LIB}-static)
                else()
                    set( THIS_LIBRARY_SEARCH_DEBUG ${LIB}d ${LIB}_D ${LIB}_debug ${LIB}d-shared ${LIB}_D-shared ${LIB}_debug-shared)
                    set( THIS_LIBRARY_SEARCH_RELEASE ${LIB} ${LIB}-shared)
                endif()
                find_library(HDFEOS5_${LIB}_LIBRARY_DEBUG
                    NAMES ${THIS_LIBRARY_SEARCH_DEBUG}
                    HINTS ${HDFEOS5_ROOT} PATH_SUFFIXES lib Lib ${_lib_suffixes}
                    ${_HDFEOS5_SEARCH_OPTS}
                )
                find_library(HDFEOS5_${LIB}_LIBRARY_RELEASE
                    NAMES ${THIS_LIBRARY_SEARCH_RELEASE}
                    HINTS ${HDFEOS5_ROOT} PATH_SUFFIXES lib Lib ${_lib_suffixes}
                    ${_HDFEOS5_SEARCH_OPTS}
                )

                select_library_configurations( HDFEOS5_${LIB} )
                list(APPEND HDFEOS5_${_lang}_HL_LIBRARIES ${HDFEOS5_${LIB}_LIBRARY})
            endforeach()

            # Append the libraries for this language binding to the list of all
            # required libraries.
            list(APPEND HDFEOS5_HL_LIBRARIES ${HDFEOS5_${_lang}_HL_LIBRARIES})
        endif()
    endforeach()
    if(HDFEOS5_FIND_HL AND HDFEOS5_HL_LIBRARIES)
        set(HDFEOS5_HL_FOUND TRUE)
    endif()

    _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_DEFINITIONS)
    _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_INCLUDE_DIRS)
    _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_LIBRARIES)
    _HDFEOS5_remove_duplicates_from_beginning(HDFEOS5_HL_LIBRARIES)

    # If the HDFEOS5 include directory was found, open H5pubconf.h to determine if
    # HDFEOS5 was compiled with parallel IO support
    set( HDFEOS5_IS_PARALLEL FALSE )
    set( HDFEOS5_VERSION "" )
    foreach( _dir IN LISTS HDFEOS5_INCLUDE_DIRS )
      foreach(_hdr "${_dir}/H5pubconf.h" "${_dir}/H5pubconf-64.h" "${_dir}/H5pubconf-32.h")
        if( EXISTS "${_hdr}" )
            file( STRINGS "${_hdr}"
                HDFEOS5_HAVE_PARALLEL_DEFINE
                REGEX "HAVE_PARALLEL 1" )
            if( HDFEOS5_HAVE_PARALLEL_DEFINE )
                set( HDFEOS5_IS_PARALLEL TRUE )
            endif()
            unset(HDFEOS5_HAVE_PARALLEL_DEFINE)

            file( STRINGS "${_hdr}"
                HDFEOS5_VERSION_DEFINE
                REGEX "^[ \t]*#[ \t]*define[ \t]+H5_VERSION[ \t]+" )
            if( "${HDFEOS5_VERSION_DEFINE}" MATCHES
                "H5_VERSION[ \t]+\"([0-9]+\\.[0-9]+\\.[0-9]+)(-patch([0-9]+))?\"" )
                set( HDFEOS5_VERSION "${CMAKE_MATCH_1}" )
                if( CMAKE_MATCH_3 )
                  set( HDFEOS5_VERSION ${HDFEOS5_VERSION}.${CMAKE_MATCH_3})
                endif()
            endif()
            unset(HDFEOS5_VERSION_DEFINE)
        endif()
      endforeach()
    endforeach()
    unset(_hdr)
    unset(_dir)
    set( HDFEOS5_IS_PARALLEL ${HDFEOS5_IS_PARALLEL} CACHE BOOL
        "HDFEOS5 library compiled with parallel IO support" )
    mark_as_advanced( HDFEOS5_IS_PARALLEL )

    set(HDFEOS5_REQUIRED_VARS HDFEOS5_LIBRARIES HDFEOS5_INCLUDE_DIRS)
    if(HDFEOS5_FIND_HL)
        list(APPEND HDFEOS5_REQUIRED_VARS HDFEOS5_HL_LIBRARIES)
    endif()
endif()

# For backwards compatibility we set HDFEOS5_INCLUDE_DIR to the value of
# HDFEOS5_INCLUDE_DIRS
if( HDFEOS5_INCLUDE_DIRS )
  set( HDFEOS5_INCLUDE_DIR "${HDFEOS5_INCLUDE_DIRS}" )
endif()

# If HDFEOS5_REQUIRED_VARS is empty at this point, then it's likely that
# something external is trying to explicitly pass already found
# locations
if(NOT HDFEOS5_REQUIRED_VARS)
    set(HDFEOS5_REQUIRED_VARS HDFEOS5_LIBRARIES HDFEOS5_INCLUDE_DIRS)
endif()

find_package_handle_standard_args(HDFEOS5
    REQUIRED_VARS ${HDFEOS5_REQUIRED_VARS}
    VERSION_VAR   HDFEOS5_VERSION
    HANDLE_COMPONENTS
)

unset(_HDFEOS5_SEARCH_OPTS)

if( HDFEOS5_FOUND AND NOT HDFEOS5_DIR)
  # hide HDFEOS5_DIR for the non-advanced user to avoid confusion with
  # HDFEOS5_DIR-NOT_FOUND while HDFEOS5 was found.
  mark_as_advanced(HDFEOS5_DIR)
endif()

if (HDFEOS5_FIND_DEBUG)
  message(STATUS "HDFEOS5_DIR: ${HDFEOS5_DIR}")
  message(STATUS "HDFEOS5_DEFINITIONS: ${HDFEOS5_DEFINITIONS}")
  message(STATUS "HDFEOS5_INCLUDE_DIRS: ${HDFEOS5_INCLUDE_DIRS}")
  message(STATUS "HDFEOS5_LIBRARIES: ${HDFEOS5_LIBRARIES}")
  message(STATUS "HDFEOS5_HL_LIBRARIES: ${HDFEOS5_HL_LIBRARIES}")
  foreach(_lang IN LISTS HDFEOS5_LANGUAGE_BINDINGS)
    message(STATUS "HDFEOS5_${_lang}_DEFINITIONS: ${HDFEOS5_${_lang}_DEFINITIONS}")
    message(STATUS "HDFEOS5_${_lang}_INCLUDE_DIR: ${HDFEOS5_${_lang}_INCLUDE_DIR}")
    message(STATUS "HDFEOS5_${_lang}_INCLUDE_DIRS: ${HDFEOS5_${_lang}_INCLUDE_DIRS}")
    message(STATUS "HDFEOS5_${_lang}_LIBRARY: ${HDFEOS5_${_lang}_LIBRARY}")
    message(STATUS "HDFEOS5_${_lang}_LIBRARIES: ${HDFEOS5_${_lang}_LIBRARIES}")
    message(STATUS "HDFEOS5_${_lang}_HL_LIBRARY: ${HDFEOS5_${_lang}_HL_LIBRARY}")
    message(STATUS "HDFEOS5_${_lang}_HL_LIBRARIES: ${HDFEOS5_${_lang}_HL_LIBRARIES}")
  endforeach()
endif()
unset(_lang)
unset(_HDFEOS5_NEED_TO_SEARCH)
