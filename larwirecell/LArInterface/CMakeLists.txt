# larwirecell/LArInterface/CMakeLists.txt
#
# David Adams
# June 2016
#
# CMake instructions for larsoft service interface classes.

install_headers()
install_source()

art_make(
	LIBRARY_NAME Test_module
	EXCLUDE AdcTypes.h
	MODULE_LIBRARIES
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_IO_PRODUCTMIX}
			${ART_FRAMEWORK_IO_ROOT}
			${ART_FRAMEWORK_MODULES}
                        ${ART_FRAMEWORK_PRINCIPAL}
                        ${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${FHICLCPP}
                        ${CETLIB}
			wirecell
)
