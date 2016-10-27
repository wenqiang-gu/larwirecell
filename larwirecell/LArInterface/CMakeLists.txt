# larwirecell/LArInterface/CMakeLists.txt
#
# David Adams
# June 2016
#
# CMake instructions for larsoft service interface classes.

cet_enable_asserts()

message("WIRECELL_LIBS variable is ${WIRECELL_LIBS}")

art_make( BASENAME_ONLY
          LIBRARY_NAME  larwirecell_larinterface
          LIB_LIBRARIES
                        ${ART_FRAMEWORK_CORE}
                        art_Utilities canvas_Utilities
                        ${MF_MESSAGELOGGER}
                        ${MF_UTILITIES}
                        ${CETLIB}
                        ${Boost_SYSTEM_LIBRARY}
                        ${ROOT_BASIC_LIB_LIST}
          MODULE_LIBRARIES
          lardata_Utilities
	  lardataobj_RawData
          SignalShapingServiceMicroBooNE_service
          ${ART_FRAMEWORK_CORE}
          ${ART_FRAMEWORK_PRINCIPAL}
          ${ART_FRAMEWORK_SERVICES_REGISTRY}
          ${ART_FRAMEWORK_SERVICES_OPTIONAL}
          ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
          art_Persistency_Common canvas_Persistency_Common
          art_Persistency_Provenance canvas_Persistency_Provenance
          art_Utilities canvas_Utilities
          ${MF_MESSAGELOGGER}
          ${MF_UTILITIES}
          ${FHICLCPP}
          ${CETLIB}
          ${ROOT_GEOM}
          ${ROOT_XMLIO}
          ${ROOT_GDML}
          ${ROOT_BASIC_LIB_LIST}
	  #LArWireCellNoiseFilterService_service
	  ${WIRECELL_LIBS}
)

install_headers()
install_fhicl()
install_source()
