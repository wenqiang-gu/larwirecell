# Build the libWireCellLarsoft WCT plugin library.
# See README for some details.

file(GLOB wcls_components_sources "*.cxx")

art_make_library(
  LIBRARY_NAME WireCellLarsoft
  SOURCE ${wcls_components_sources}
  LIBRARIES
        art_Framework_Core art_Framework_Principal art_Framework_Services_Registry
        art_Utilities
        canvas cetlib cetlib_except
        ${Boost_SYSTEM_LIBRARY}
        ${ROOT_BASIC_LIB_LIST}
        jsoncpp
        ${WIRECELL_LIBS}
)
