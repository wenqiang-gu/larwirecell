file(GLOB wcls_tools_sources "*.cxx")

art_make_library(
  SOURCE ${wcls_tools_sources}
  LIBRARIES ${WIRECELL_LIBS}
  LIBRARY_NAME_VAR wcls_tools_lib
)
simple_plugin(Main "tool" ${wcls_tools_lib}) # ${WIRECELL_LIBS})
