include(FeatureSummary)
set_package_properties(Gtk3 PROPERTIES
        URL "https://www.gtk.org/"
        DESCRIPTION "GTK+, or the GIMP Toolkit, is a multi-platform toolkit for creating graphical user interfaces")

find_package(PkgConfig)
pkg_check_modules(PC_Gtk3 gtk+-3.0 QUIET)

set(Gtk3_INCLUDE_DIRS ${PC_Gtk3_INCLUDE_DIRS})
foreach(COMP ${PC_Gtk3_LIBRARIES})
    find_library(Gtk3_${COMP} NAMES ${COMP} HINTS ${PC_Gtk3_LIBRARY_DIRS})
    list(APPEND Gtk3_LIBRARIES ${Gtk3_${COMP}})
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Gtk3 DEFAULT_MSG Gtk3_LIBRARIES Gtk3_INCLUDE_DIRS)

mark_as_advanced(Gtk3_INCLUDE_DIRS Gtk3_LIBRARIES)

if(PC_Gtk3_FOUND AND NOT TARGET Gtk3::Gtk)
    add_library(Gtk3::Gtk INTERFACE IMPORTED)

    set_property(TARGET Gtk3::Gtk PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Gtk3_INCLUDE_DIRS})
    set_property(TARGET Gtk3::Gtk PROPERTY INTERFACE_LINK_LIBRARIES ${Gtk3_LIBRARIES})
endif()
