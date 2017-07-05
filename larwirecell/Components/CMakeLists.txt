# Build the libWireCellLarsoft WCT plugin library.
# See README for some details.

file(GLOB wcls_components_sources "*.cxx")

art_make_library(
  LIBRARY_NAME WireCellLarsoft
  SOURCE ${wcls_components_sources}
  LIBRARIES ${WIRECELL_LIBS}
)
