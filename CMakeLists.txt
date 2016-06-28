# larwirecell/CMakeLists.txt
#
# David Adams
# June 2016
#
# CMake instructions for the larwirecell package.
#
# ======================================================================
# Old build instructions:
#  cd .../path/to/build/directory
#  source .../path/to/dune_extensions/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================

# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(larwirecell)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH $ENV{ART_DIR}/Modules
		      $ENV{CETBUILDTOOLS_DIR}/Modules
		      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

set_install_root()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( lardata )
find_ups_product( art )
find_ups_product( cetbuildtools )

#art_make( BASENAME_ONLY MODULE_LIBRARIES HitFinderLBNE )

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# sources
add_subdirectory(larwirecell)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
