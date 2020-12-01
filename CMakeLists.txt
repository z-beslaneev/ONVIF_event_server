cmake_minimum_required(VERSION 3.7)

project(Sphinx)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_SOURCE_DIR}/external/openssl-1.0.2l-vs2017")

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

include(GSOAPConfig)

set(OPENSSL_USE_STATIC_LIBS TRUE)

if (UNIX)
    find_package(OpenSSL REQUIRED)
    find_package(Threads REQUIRED)
    # dl required for static linking with openssl
    set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} -ldl)
else()
    if (NOT OPENSSL_ROOT_DIR)
        set(OPENSSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/external/openssl-1.0.2l-vs2017/)
    endif()
    message(STATUS "OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR}")

    include(OpenSSLConfig)
    
    # Set OPENSSL_MSVC_STATIC_RT set TRUE to choose the MT version of the lib (???)
    set(OPENSSL_MSVC_STATIC_RT TRUE)
endif()

gsoap_generate(
    USE_DOM
    TYPEMAP WS/typeMap.dat
    GENERATE_HEADER onvif.h
    SOURCE_HEADER WS/onvifFull.h
    OUTPUT_DIR gSoap
    WSDL2H_FLAGS -Nname
    SOAPCPP2_FLAGS -IWS; -j
    OPTNAME GSOAP_GENERATE_ONVIF
    OPTDESC "Check to generate source files from ONVIF xsd and WSDLs"
    SOURCES
        WS/device.wsdl
        WS/event.wsdl
        WS/ws-discovery.wsdl
        WS/onvif.xsd
	WS/sensors.xsd
    )

set(Onvif_SRCS
	src/AuthorisationHolder.cpp
	src/AuthorisationHolder.h
	src/Device.cpp
	src/Device.h
	src/Event.cpp
	src/Event.h	
	src/EventReader.cpp
	src/EventReader.h
	src/GPIOController.cpp
	src/GPIOController.h
	src/LicenseChecker.h
	src/LicenseChecker.cpp
	src/md5Util.h
	src/OnvifServer.h
	src/OnvifServer.cpp
	src/PullPointSubscription.cpp
	src/PullPointSubscription.h
	src/RealTimeClock.cpp
	src/RealTimeClock.h
	src/SoapHelpers.cpp
	src/SoapHelpers.h
	src/SubscriptionController.cpp
	src/SubscriptionController.h
	src/SubscriptionManager.cpp
	src/SubscriptionManager.h
	src/main.cpp
)

if (UNIX)
set(Onvif_SRCS
	${Onvif_SRCS}
	src/SerialController.cpp
	src/SerialController.h
)
endif()

set(Onvif_gSoap
	gSoap/soap.nsmap
	)
source_group("gSoap" FILES ${Onvif_gSoap})

set(Onvif_gSoap_SRCS
	gSoap/chrono_duration.cpp
	gSoap/dom.cpp
	gSoap/httpda.c
	gSoap/httpda.h
	gSoap/mecevp.c
	gSoap/md5evp.c
	gSoap/md5evp.h
	gSoap/smdevp.c
	gSoap/soapC_001.cpp
	gSoap/soapC_002.cpp
	gSoap/soapC_003.cpp
	gSoap/soapC_004.cpp
	gSoap/soapC_005.cpp
	gSoap/soapC_006.cpp
	gSoap/soapC_007.cpp
	gSoap/soapC_008.cpp
	gSoap/soapC_009.cpp
	gSoap/soapC_010.cpp
	gSoap/soapC_011.cpp
	gSoap/soapC_012.cpp
	gSoap/soapC_013.cpp
	gSoap/soapCreatePullPointBindingService.cpp
	gSoap/soapDeviceBindingService.cpp
	gSoap/soapEventBindingService.cpp
	gSoap/soapNotificationConsumerBindingService.cpp
	gSoap/soapNotificationProducerBindingService.cpp
	gSoap/soapPausableSubscriptionManagerBindingService.cpp
	gSoap/soapPullPointBindingService.cpp
	gSoap/soapPullPointSubscriptionBindingService.cpp
	gSoap/soapSubscriptionManagerBindingService.cpp
	gSoap/stdsoap2.cpp
	gSoap/struct_timeval.cpp
	gSoap/threads.c
	gSoap/thread_setup.c	
	gSoap/wsaapi.cpp
	gSoap/wsseapi.cpp
	)

source_group("gSoap\\Source Files" FILES ${Onvif_gSoap_SRCS})

set(Onvif_gSoap_HDRS
	WS/chrono_duration.h
	gSoap/mecevp.h
	gSoap/smdevp.h
	gSoap/soapH.h
	gSoap/soapStub.h
	gSoap/soapCreatePullPointBindingService.h
	gSoap/soapDeviceBindingService.h
	gSoap/soapEventBindingService.h
	gSoap/soapNotificationConsumerBindingService.h
	gSoap/soapNotificationProducerBindingService.h
	gSoap/soapPausableSubscriptionManagerBindingService.h
	gSoap/soapPullPointBindingService.h
	gSoap/soapPullPointSubscriptionBindingService.h
	gSoap/soapSubscriptionManagerBindingService.h
	gSoap/stdsoap2.h
	gSoap/threads.h
	gSoap/thread_setup.h
	gSoap/wsaapi.h
	gSoap/wsseapi.h
	)
source_group("gSoap\\Header Files" FILES ${Onvif_gSoap_HDRS})

set_source_files_properties(${Onvif_gSoap_CPP_SRCS} PROPERTIES LANGUAGE CXX)

include_directories(${OPENSSL_INCLUDE_DIR} sysinfo/ src/)

add_subdirectory(sysinfo sysinfo/build)

add_subdirectory(keygen keygen/build)

include(./primitives/CMakeLists.txt)

set(ONVIF_gSOAP_DEFINITIONS
	-DWITH_NONAMESPACES -DWITH_OPENSSL -DWITH_DOM -DWITH_DEFAULT_VIRTUAL -DSOAP_DEBUG)

add_definitions(${ONVIF_gSOAP_DEFINITIONS})

add_executable(SphinxDetectors 
	${Onvif_SRCS}
	${Onvif_gSoap} 
	${Onvif_gSoap_SRCS}
	${Onvif_gSoap_HDRS}
	${Primitives_SRCS}
	)
	
target_link_libraries(SphinxDetectors
	${OPENSSL_LIBRARIES} 
	${CMAKE_THREAD_LIBS_INIT}
	sysinfo
	)
	
if (MSVC)
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /bigobj")
	
    set_target_properties(SphinxDetectors PROPERTIES
        COMPILE_FLAGS "/wd4005 /wd4018 /wd4101 /wd4267 /wd4244 /wd4800 /wd4996 /wd4477"
        )
endif()

# Задать некоторые характеристики пакета:
set(CPACK_PACKAGE_NAME "SphinxDetectors")
set(CPACK_PACKAGE_VENDOR "Sphinx")
set(CPACK_PACKAGE_CONTACT "info@sphinx-md.ru")
set(CPACK_PACKAGE_DESCRIPTION "Onvif based event server.")
set(CPACK_PACKAGE_VERSION "1.1.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")


install(TARGETS SphinxDetectors DESTINATION /usr/bin)
install(FILES ${CMAKE_SOURCE_DIR}/scripts/SphinxDetectors_d DESTINATION /etc/init.d)
install(FILES ${CMAKE_SOURCE_DIR}/resource/settings.json DESTINATION /var/lib/${CPACK_PACKAGE_NAME})
install(FILES ${CMAKE_SOURCE_DIR}/scripts/synctime DESTINATION /var/lib/${CPACK_PACKAGE_NAME})
install(FILES ${CMAKE_SOURCE_DIR}/scripts/led_of DESTINATION /var/lib/${CPACK_PACKAGE_NAME})

# Задать используемый генератор пакетов:
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_SOURCE_DIR}/scripts/postinst;${CMAKE_SOURCE_DIR}/scripts/prerm;")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "http://www.sphinx-md.ru/")

# Включить стандартный модуль "CPack":
include(CPack)

