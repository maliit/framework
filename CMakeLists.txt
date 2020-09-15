cmake_minimum_required(VERSION 3.3)

project(maliit-framework VERSION 0.99.2)

# Build options

option(enable-docs "Build documentation" ON)
option(enable-tests "Build tests" ON)

option(enable-glib "Build GLib support" ON)

option(enable-xcb "Compile with xcb support" ON)
option(enable-wayland "Compile with support for wayland" ON)
option(enable-wayland-gtk "Compile with support for wayland gtk+" OFF)
option(enable-qt5-inputcontext "Compile with Qt5 input context" ON)

option(enable-hwkeyboard "Enable support for the hardware keyboard" ON)
option(enable-dbus-activation "Enable dbus activation support for maliit-server" OFF)

# Install paths
include(GNUInstallDirs)

if(NOT DEFINED QT5_PLUGINS_INSTALL_DIR)
    set(QT5_PLUGINS_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/qt5/plugins" CACHE PATH
            "Installation directory for Qt 5 plugins [LIB_INSTALL_DIR/qt5/plugins]")
endif()

if(NOT DEFINED QT5_MKSPECS_INSTALL_DIR)
    set(QT5_MKSPECS_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/qt5/mkspecs" CACHE PATH
            "Installation directory for Qt 5 mkspecs files [LIB_INSTALL_DIR/qt5/mkspecs]")
endif()

if(NOT DEFINED GTK3_PLUGINS_INSTALL_DIR)
    set(GTK3_PLUGINS_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/gtk-3.0/3.0.0" CACHE PATH
            "Installation directory for Gtk+ 3 plugins [LIB_INSTALL_DIR/gtk-3.0/3.0.0]")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(PkgConfig REQUIRED)

find_package(Qt5Core)
find_package(Qt5DBus)
find_package(Qt5Gui REQUIRED PRIVATE)
find_package(Qt5Quick)
find_package(Qt5Widgets)

if(enable-wayland)
    find_package(WaylandProtocols REQUIRED PRIVATE)
    find_package(QtWaylandScanner REQUIRED)
    find_package(Wayland REQUIRED)
    find_package(Qt5WaylandClient 5.14 REQUIRED PRIVATE)
    find_package(Qt5XkbCommonSupport REQUIRED PRIVATE)

    find_package(Gtk3)
endif()

include_directories(src common)

add_library(maliit-common STATIC
            common/maliit/namespace.h
            common/maliit/namespaceinternal.h
            common/maliit/settingdata.cpp
            common/maliit/settingdata.h)
target_link_libraries(maliit-common Qt5::Core)
target_include_directories(maliit-common PUBLIC common)

set(CONNECTION_SOURCES
    connection/connectionfactory.cpp
    connection/connectionfactory.h
    connection/dbuscustomarguments.cpp
    connection/dbuscustomarguments.h
    connection/dbusinputcontextconnection.cpp
    connection/dbusinputcontextconnection.h
    connection/dbusserverconnection.cpp
    connection/dbusserverconnection.h
    connection/inputcontextdbusaddress.cpp
    connection/inputcontextdbusaddress.h
    connection/mimserverconnection.cpp
    connection/mimserverconnection.h
    connection/minputcontextconnection.cpp
    connection/minputcontextconnection.h
    connection/serverdbusaddress.cpp
    connection/serverdbusaddress.h)

if(enable-wayland)
    list(APPEND CONNECTION_SOURCES
         connection/waylandinputmethodconnection.cpp
         connection/waylandinputmethodconnection.h)

    ecm_add_qtwayland_client_protocol(CONNECTION_SOURCES PROTOCOL ${WAYLANDPROTOCOLS_PATH}/unstable/input-method/input-method-unstable-v1.xml BASENAME input-method-unstable-v1)
    ecm_add_qtwayland_client_protocol(CONNECTION_SOURCES PROTOCOL ${WAYLANDPROTOCOLS_PATH}/unstable/text-input/text-input-unstable-v1.xml BASENAME text-input-unstable-v1)

    add_definitions(-DHAVE_WAYLAND)
endif()

set_source_files_properties(dbus_interfaces/minputmethodcontext1interface.xml dbus_interfaces/minputmethodserver1interface.xml
                            PROPERTIES INCLUDE maliit/settingdata.h)

qt5_add_dbus_adaptor(CONNECTION_SOURCES dbus_interfaces/minputmethodcontext1interface.xml dbusserverconnection.h DBusServerConnection)
qt5_add_dbus_adaptor(CONNECTION_SOURCES dbus_interfaces/minputmethodserver1interface.xml dbusinputcontextconnection.h DBusInputContextConnection)

qt5_add_dbus_interface(CONNECTION_SOURCES dbus_interfaces/minputmethodcontext1interface.xml minputmethodcontext1interface_interface)
qt5_add_dbus_interface(CONNECTION_SOURCES dbus_interfaces/minputmethodserver1interface.xml minputmethodserver1interface_interface)

add_library(maliit-connection STATIC ${CONNECTION_SOURCES})
target_link_libraries(maliit-connection Qt5::Core Qt5::DBus Qt5::Gui maliit-common)
if(enable-wayland)
    target_link_libraries(maliit-connection Wayland::Client)
endif()
target_include_directories(maliit-connection PUBLIC connection)

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})

set(PLUGINS_SOURCES
    src/maliit/plugins/abstractinputmethod.cpp
    src/maliit/plugins/abstractinputmethod.h
    src/maliit/plugins/abstractinputmethodhost.cpp
    src/maliit/plugins/abstractinputmethodhost.h
    src/maliit/plugins/abstractpluginsetting.h
    src/maliit/plugins/attributeextension.cpp
    src/maliit/plugins/attributeextension.h
    src/maliit/plugins/attributeextension_p.h
    src/maliit/plugins/extensionevent.cpp
    src/maliit/plugins/extensionevent.h
    src/maliit/plugins/extensionevent_p.h
    src/maliit/plugins/inputmethodplugin.h
    src/maliit/plugins/keyoverride.cpp
    src/maliit/plugins/keyoverride.h
    src/maliit/plugins/keyoverride_p.h
    src/maliit/plugins/keyoverridedata.cpp
    src/maliit/plugins/keyoverridedata.h
    src/maliit/plugins/plugindescription.cpp
    src/maliit/plugins/plugindescription.h
    src/maliit/plugins/subviewdescription.cpp
    src/maliit/plugins/subviewdescription.h
    src/maliit/plugins/updateevent.cpp
    src/maliit/plugins/updateevent.h
    src/maliit/plugins/updateevent_p.h
    src/maliit/plugins/updatereceiver.cpp
    src/maliit/plugins/updatereceiver.h
    src/maliit/standaloneinputmethod.cpp
    src/maliit/standaloneinputmethod.h
    src/maliit/standaloneinputmethodhost.cpp
    src/maliit/standaloneinputmethodhost.h
    src/quick/inputmethodquick.cpp
    src/quick/inputmethodquick.h
    src/quick/inputmethodquickplugin.cpp
    src/quick/inputmethodquickplugin.h
    src/quick/keyoverridequick.cpp
    src/quick/keyoverridequick.h
    src/quick/keyoverridequick_p.h
    src/quick/maliitquick.h
    src/abstractplatform.cpp
    src/abstractplatform.h
    src/mattributeextensionid.cpp
    src/mattributeextensionid.h
    src/mattributeextensionmanager.cpp
    src/mattributeextensionmanager.h
    src/mimhwkeyboardtracker.h
    src/mimonscreenplugins.cpp
    src/mimonscreenplugins.h
    src/mimpluginmanager.cpp
    src/mimpluginmanager.h
    src/mimpluginmanager_p.h
    src/mimserver.cpp
    src/mimserver.h
    src/mimserveroptions.cpp
    src/mimserveroptions.h
    src/mimsettings.cpp
    src/mimsettings.h
    src/mimsettingsqsettings.cpp
    src/mimsettingsqsettings.h
    src/mimsubviewoverride.cpp
    src/mimsubviewoverride.h
    src/minputmethodhost.cpp
    src/minputmethodhost.h
    src/msharedattributeextensionmanager.cpp
    src/msharedattributeextensionmanager.h
    src/unknownplatform.cpp
    src/unknownplatform.h
    src/windowdata.cpp
    src/windowdata.h
    src/windowgroup.cpp
    src/windowgroup.h)

if(enable-xcb)
    list(APPEND PLUGINS_SOURCES
         src/xcbplatform.cpp
         src/xcbplatform.h)
    find_package(XCB REQUIRED xfixes)
    list(APPEND PLUGINS_INCLUDE_DIRS ${XCB_INCLUDE_DIRS})
    list(APPEND PLUGINS_LIBRARIES ${XCB_LIBRARIES})
endif()

if(enable-wayland)
    list(APPEND PLUGINS_SOURCES
         src/waylandplatform.cpp
         src/waylandplatform.h)

endif()

if(enable-hwkeyboard)
    list(APPEND PLUGINS_SOURCES
         src/mimhwkeyboardtracker.cpp
         src/mimhwkeyboardtracker_p.h)
    find_package(UDev REQUIRED)
    list(APPEND PLUGINS_LIBRARIES UDev::UDev)
else()
    list(APPEND PLUGINS_SOURCES src/mimhwkeyboardtracker_stub.cpp)
endif()

add_library(maliit-plugins SHARED ${PLUGINS_SOURCES} ${PLUGINS_HEADER})
target_link_libraries(maliit-plugins PRIVATE maliit-common maliit-connection ${PLUGINS_LIBRARIES})
target_link_libraries(maliit-plugins PUBLIC Qt5::Core Qt5::Gui Qt5::Quick)
target_include_directories(maliit-plugins PRIVATE ${PLUGINS_INCLUDE_DIRS})

set_target_properties(maliit-plugins PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        EXPORT_NAME Maliit::Plugins)

if(enable-glib)
    find_package(GLib2)
    find_package(GIO)

    set(GLIB_SOURCES
        maliit-glib/maliitattributeextension.c
        maliit-glib/maliitattributeextension.h
        maliit-glib/maliitattributeextensionprivate.h
        maliit-glib/maliitattributeextensionregistry.c
        maliit-glib/maliitattributeextensionregistry.h
        maliit-glib/maliitbus.c
        maliit-glib/maliitbus.h
        maliit-glib/maliitbusprivate.h
        maliit-glib/maliitinputmethod.c
        maliit-glib/maliitinputmethod.h
        maliit-glib/maliitpluginsettings.c
        maliit-glib/maliitpluginsettings.h
        maliit-glib/maliitpluginsettingsprivate.h
        maliit-glib/maliitsettingdata.c
        maliit-glib/maliitsettingdata.h
        maliit-glib/maliitsettingsentry.c
        maliit-glib/maliitsettingsentry.h
        maliit-glib/maliitsettingsentryprivate.h
        maliit-glib/maliitsettingsmanager.c
        maliit-glib/maliitsettingsmanager.h)

    set_source_files_properties(maliit-glib/maliitmarshallers.list
            PROPERTIES PREFIX maliit_marshal)
    glib2_add_marshal(GLIB_SOURCES maliit-glib/maliitmarshallers.list)

    set_source_files_properties(dbus_interfaces/minputmethodcontext1interface.xml PROPERTIES
            INTERFACE_PREFIX com.meego
            NAMESPACE Maliit
            OUTPUT_NAME maliit-glib/maliitcontext)
    set_property(SOURCE dbus_interfaces/minputmethodcontext1interface.xml PROPERTY
            ANNOTATE com.meego.inputmethod.inputcontext1 org.gtk.GDBus.C.Name Context)

    gdbus_add_code(GLIB_SOURCES dbus_interfaces/minputmethodcontext1interface.xml)

    set_source_files_properties(dbus_interfaces/minputmethodserver1interface.xml PROPERTIES
            INTERFACE_PREFIX com.meego
            NAMESPACE Maliit
            OUTPUT_NAME maliit-glib/maliitserver)
    set_property(SOURCE dbus_interfaces/minputmethodserver1interface.xml PROPERTY
            ANNOTATE com.meego.inputmethod.uiserver1 org.gtk.GDBus.C.Name Server)
    gdbus_add_code(GLIB_SOURCES dbus_interfaces/minputmethodserver1interface.xml)

    add_library(maliit-glib SHARED ${GLIB_SOURCES} ${GLIB_HEADER})
    target_include_directories(maliit-glib PUBLIC ${GIO_INCLUDE_DIRS})
    target_link_libraries(maliit-glib ${GIO_LIBRARIES})
    set_target_properties(maliit-glib PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}
            VERSION ${PROJECT_VERSION}
            EXPORT_NAME Maliit::GLib)
endif()

add_definitions(-DMALIIT_FRAMEWORK_USE_INTERNAL_API
                -DMALIIT_PLUGINS_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/maliit/plugins"
                -DMALIIT_EXTENSIONS_DIR="${CMAKE_INSTALL_FULL_DATADIR}/maliit-framework/extensions"
                -DMALIIT_CONFIG_ROOT="/maliit/"
                -DMALIIT_PLUGINS_DIR="${CMAKE_INSTALL_FULL_LIBDIR}/maliit/plugins"
                -DMALIIT_DEFAULT_HW_PLUGIN="libmaliit-keyboard-plugin.so"
                -DMALIIT_ENABLE_MULTITOUCH=true
                -DMALIIT_DEFAULT_PLUGIN="libmaliit-keyboard-plugin.so"
                -DMALIIT_DEFAULT_SUBVIEW="")

add_executable(maliit-server passthroughserver/main.cpp)
target_link_libraries(maliit-server maliit-plugins maliit-connection)

if(enable-qt5-inputcontext)
    set(INPUT_CONTEXT_SOURCES
            input-context/main.cpp
            input-context/minputcontext.cpp
            input-context/minputcontext.h)

    add_library(maliitplatforminputcontextplugin MODULE ${INPUT_CONTEXT_SOURCES})
    target_link_libraries(maliitplatforminputcontextplugin maliit-connection Qt5::Quick)
endif()

if(enable-wayland)
    set(INPUT_PANEL_SHELL_SOURCES
            src/qt/plugins/shellintegration/inputpanelshellplugin.cpp
            src/qt/plugins/shellintegration/qwaylandinputpanelshellintegration.cpp
            src/qt/plugins/shellintegration/qwaylandinputpanelshellintegration.h src/qt/plugins/shellintegration/qwaylandinputpanelsurface.cpp src/qt/plugins/shellintegration/qwaylandinputpanelsurface.h)

    ecm_add_qtwayland_client_protocol(INPUT_PANEL_SHELL_SOURCES PROTOCOL ${WAYLANDPROTOCOLS_PATH}/unstable/input-method/input-method-unstable-v1.xml BASENAME input-method-unstable-v1)

    add_library(inputpanel-shell MODULE ${INPUT_PANEL_SHELL_SOURCES})
    target_link_libraries(inputpanel-shell Qt5::WaylandClient)
    target_include_directories(inputpanel-shell PRIVATE ${Qt5WaylandClient_PRIVATE_INCLUDE_DIRS} ${Qt5XkbCommonSupport_PRIVATE_INCLUDE_DIRS})

    if(enable-wayland-gtk AND TARGET Gtk3::Gtk)
        add_library(im-wayland MODULE
                src/gtk/plugins/im-module/gtkimcontextwayland.c
                src/gtk/plugins/im-module/gtkimcontextwayland.h
                src/gtk/plugins/im-module/imwayland.c)
        ecm_add_wayland_client_protocol(IM_WAYLAND_SOURCES PROTOCOL ${WAYLANDPROTOCOLS_PATH}/unstable/text-input/text-input-unstable-v1.xml BASENAME text-input-unstable-v1)
        target_sources(im-wayland PRIVATE ${IM_WAYLAND_SOURCES})
        target_link_libraries(im-wayland Gtk3::Gtk)
    endif()
endif()

add_executable(maliit-exampleapp-plainqt
        examples/apps/plainqt/mainwindow.cpp
        examples/apps/plainqt/mainwindow.h
        examples/apps/plainqt/plainqt.cpp)
target_link_libraries(maliit-exampleapp-plainqt Qt5::Gui Qt5::Widgets)

add_library(cxxhelloworldplugin MODULE
        examples/plugins/cxx/helloworld/helloworldinputmethod.cpp
        examples/plugins/cxx/helloworld/helloworldinputmethod.h
        examples/plugins/cxx/helloworld/helloworldplugin.cpp
        examples/plugins/cxx/helloworld/helloworldplugin.h)
target_link_libraries(cxxhelloworldplugin maliit-plugins Qt5::Widgets)

add_library(cxxoverrideplugin MODULE
            examples/plugins/cxx/override/overrideinputmethod.cpp
            examples/plugins/cxx/override/overrideinputmethod.h
            examples/plugins/cxx/override/overrideplugin.cpp
            examples/plugins/cxx/override/overrideplugin.h)
target_link_libraries(cxxoverrideplugin maliit-plugins Qt5::Widgets)

file(COPY examples/plugins/qml/helloworld/helloworld.qml
     DESTINATION ${CMAKE_BINARY_DIR}/examples/plugins/qml/helloworld)

# Documentation

if(enable-docs)
    find_package(Doxygen REQUIRED)

    configure_file(doc/Doxyfile.in Doxyfile @ONLY)

    add_custom_target(doc ALL
                      ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()

# Package files

configure_file(common/maliit-framework.pc.in maliit-framework.pc @ONLY)
configure_file(src/maliit-plugins.pc.in maliit-plugins.pc @ONLY)
configure_file(src/maliit-server.pc.in maliit-server.pc @ONLY)

configure_file(common/maliit-framework.prf.in maliit-framework.prf @ONLY)
configure_file(src/maliit-plugins.prf.in maliit-plugins.prf @ONLY)
configure_file(src/maliit-defines.prf.in maliit-defines.prf @ONLY)

include(CMakePackageConfigHelpers)

configure_package_config_file(src/MaliitPluginsConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/MaliitPluginsConfig.cmake
        INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/MaliitPlugins
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_DATADIR)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/MaliitPluginsConfigVersion.cmake
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY AnyNewerVersion)

# Installation

install(TARGETS maliit-plugins
        EXPORT MaliitPluginsTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/maliit-2)

install(TARGETS maliit-server maliit-exampleapp-plainqt
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY common/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/maliit-2
        FILES_MATCHING PATTERN "*.h"
        PATTERN "*internal.h" EXCLUDE)
install(DIRECTORY src/maliit
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/maliit-2
        FILES_MATCHING PATTERN "*.h"
        PATTERN "*_p.h" EXCLUDE)
install(FILES src/mimserver.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/maliit-2/maliit)

install(FILES ${CMAKE_BINARY_DIR}/maliit-framework.pc ${CMAKE_BINARY_DIR}/maliit-plugins.pc ${CMAKE_BINARY_DIR}/maliit-server.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(FILES ${CMAKE_BINARY_DIR}/maliit-framework.prf ${CMAKE_BINARY_DIR}/maliit-plugins.prf ${CMAKE_BINARY_DIR}/maliit-defines.prf
        DESTINATION ${QT5_MKSPECS_INSTALL_DIR}/mkspecs/features)

install(EXPORT MaliitPluginsTargets FILE MaliitPluginsTargets.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MaliitPlugins)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MaliitPluginsConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/MaliitPluginsConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MaliitPlugins)

install(FILES INSTALL.local LICENSE.LGPL NEWS README
        DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/maliit-framework)

if(enable-glib)
    configure_package_config_file(maliit-glib/MaliitGLibConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/MaliitGLibConfig.cmake
            INSTALL_DESTINATION ${LIB_INSTALL_DIR}/cmake/MaliitGLib
            PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_DATADIR)
    write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/MaliitGLibConfigVersion.cmake
            VERSION ${PACKAGE_VERSION}
            COMPATIBILITY AnyNewerVersion)

    configure_file(maliit-glib/maliit-glib.pc.in maliit-glib.pc @ONLY)

    install(TARGETS maliit-glib EXPORT MaliitGLibTargets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/maliit-2)

    install(DIRECTORY maliit-glib DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/maliit-2
            FILES_MATCHING PATTERN "*.h"
            PATTERN "*private.h" EXCLUDE)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/maliit-glib DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/maliit-2
            FILES_MATCHING PATTERN "*.h")

    install(EXPORT MaliitGLibTargets FILE MaliitGLibTargets.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MaliitGLib)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/MaliitGLibConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/MaliitGLibConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MaliitGLib)

    install(FILES ${CMAKE_BINARY_DIR}/maliit-glib.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

if(enable-qt5-inputcontext)
    install(TARGETS maliitplatforminputcontextplugin LIBRARY DESTINATION ${QT5_PLUGINS_INSTALL_DIR}/platforminputcontexts)
endif()

if(enable-wayland)
    install(TARGETS inputpanel-shell
            LIBRARY DESTINATION ${QT5_PLUGINS_INSTALL_DIR}/wayland-shell-integration)
    if(enable-wayland-gtk AND TARGET Gtk3::Gtk)
        install(TARGETS im-wayland
                LIBRARY DESTINATION ${GTK3_PLUGINS_INSTALL_DIR}/immodules)
    endif()
endif()

if(enable-dbus-activation)
    configure_file(connection/org.maliit.server.service.in org.maliit.server.service @ONLY)

    install(FILES ${CMAKE_BINARY_DIR}/org.maliit.server.service DESTINATION ${CMAKE_INSTALL_DATADIR}/dbus-1/services)
endif()

if(enable-docs)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html/
            DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/maliit-framework-doc)
endif()

# Tests

if(enable-tests)
    enable_testing()

    find_package(Qt5Test)

    set(TEST_PLUGINS_DIR ${CMAKE_BINARY_DIR}/tests/plugins)

    add_library(dummyimplugin SHARED
            tests/dummyimplugin/dummyimplugin.cpp
            tests/dummyimplugin/dummyimplugin.h
            tests/dummyimplugin/dummyinputmethod.cpp
            tests/dummyimplugin/dummyinputmethod.h)
    target_link_libraries(dummyimplugin maliit-plugins)
    target_include_directories(dummyimplugin INTERFACE tests/dummyimplugin)

    add_library(dummyimplugin2 SHARED
            tests/dummyimplugin2/dummyimplugin2.cpp
            tests/dummyimplugin2/dummyimplugin2.h)
    target_link_libraries(dummyimplugin2 maliit-plugins)
    target_include_directories(dummyimplugin2 INTERFACE tests/dummyimplugin2)

    add_library(dummyimplugin3 SHARED
            tests/dummyimplugin3/dummyimplugin3.cpp
            tests/dummyimplugin3/dummyimplugin3.h
            tests/dummyimplugin3/dummyinputmethod3.cpp
            tests/dummyimplugin3/dummyinputmethod3.h)
    target_link_libraries(dummyimplugin3 maliit-plugins)
    target_include_directories(dummyimplugin3 INTERFACE tests/dummyimplugin3)

    add_library(dummyplugin SHARED
            tests/dummyplugin/dummyplugin.cpp
            tests/dummyplugin/dummyplugin.h)
    target_link_libraries(dummyplugin maliit-plugins)
    target_include_directories(dummyplugin INTERFACE tests/dummyplugin)

    set(DUMMY_PLUGINS dummyimplugin dummyimplugin2 dummyimplugin3 dummyplugin)

    set_property(TARGET ${DUMMY_PLUGINS} PROPERTY LIBRARY_OUTPUT_DIRECTORY ${TEST_PLUGINS_DIR})

    add_library(test-utils STATIC
            tests/utils/core-utils.cpp
            tests/utils/core-utils.h
            tests/utils/gui-utils.cpp
            tests/utils/gui-utils.h)
    target_link_libraries(test-utils PUBLIC Qt5::Core Qt5::Gui Qt5::Test maliit-connection)
    target_include_directories(test-utils INTERFACE tests/utils)
    target_compile_definitions(test-utils PUBLIC
            -DMALIIT_TEST_PLUGINS_DIR="${CMAKE_INSTALL_PREFIX}/lib/maliit-framework-tests/plugins"
            -DMALIIT_TEST_DATA_PATH="${CMAKE_INSTALL_PREFIX}/lib/maliit-framework-tests"
            -DIN_TREE_TEST_PLUGIN_DIR="${CMAKE_BINARY_DIR}/examples/plugins")

    add_library(test-stubs STATIC
            tests/stubs/mkeyboardstatetracker_stub.h
            tests/stubs/fakeproperty.cpp
            tests/stubs/fakeproperty.h)
    target_link_libraries(test-stubs PUBLIC Qt5::Core)
    target_include_directories(test-stubs INTERFACE tests/stubs)

    function(create_test name)
        add_executable(${name}
                tests/${name}/${name}.cpp
                tests/${name}/${name}.h)

        set(_libs ${ARGV})
        list(REMOVE_AT _libs 0)
        target_link_libraries(${name} test-utils maliit-plugins ${_libs})

        add_test(${name} ${name})

        set(test_targets ${test_targets} ${name} PARENT_SCOPE)
    endfunction()

    create_test(sanitychecks)
    create_test(ut_mattributeextensionmanager)
    create_test(ut_mimonscreenplugins)
    create_test(ut_mimpluginmanager ${DUMMY_PLUGINS})
    create_test(ut_mimpluginmanagerconfig)
    create_test(ut_mimserveroptions)
    create_test(ut_mimsettings)
    create_test(ut_minputmethodquickplugin)
    create_test(ut_mkeyoverride)
    create_test(ft_exampleplugin)
    create_test(ft_mimpluginmanager test-stubs ${DUMMY_PLUGINS})

    set_property(TEST ${test_targets} PROPERTY ENVIRONMENT
            TESTING_IN_SANDBOX=1
            TESTPLUGIN_PATH=${CMAKE_BINARY_DIR}/tests/plugins
            TESTDATA_PATH=${CMAKE_SOURCE_DIR}/tests)

    if(enable-glib)
        add_executable(ut_maliit_glib_settings
                tests/ut_maliit_glib_settings/ut_maliit_glib_settings.c
                tests/ut_maliit_glib_settings/mockmaliitserver.c
                tests/ut_maliit_glib_settings/mockmaliitserver.h)
        target_link_libraries(ut_maliit_glib_settings GLib2::GLib GLib2::GIO maliit-glib)

        add_test(ut_maliit_glib_settings ut_maliit_glib_settings)
    endif()
endif()

add_custom_target(install_${PROJECT_NAME}
        ${CMAKE_MAKE_PROGRAM} install
        DEPENDS maliit-server
        COMMENT "Installing ${PROJECT_NAME}")
