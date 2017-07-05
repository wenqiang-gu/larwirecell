# build the libWireCellLarsoft WCT plugin library.
# See README for some details.

file(GLOB wcls_sources "*.cxx")
#message("${wcls_sources}")

# I want to use art_make_library() here but it is apparently broken to
# call as documented.  It fails to find any sources.
#art_make_library(
art_make(
  LIBRARY_NAME WireCellLarsoft
#  SOURCE ${wcls_sources}
  LIB_LIBRARIES ${WIRECELL_LIBS}
)
