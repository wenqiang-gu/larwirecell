file(GLOB wcls_tools_sources "*.cxx")

set(wcls_tools_need_libs fhiclcpp cetlib cetlib_except ${WIRECELL_LIBS})

art_make_library(
  SOURCE ${wcls_tools_sources}
  LIBRARIES ${wcls_tools_need_libs}
  LIBRARY_NAME_VAR wcls_tools_lib
)
simple_plugin(WCLS "tool" ${wcls_tools_lib}) # ${WIRECELL_LIBS})
