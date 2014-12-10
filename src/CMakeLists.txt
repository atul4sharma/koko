#
# Common Library
#
set(LIB_SRCS
    imagestorage.cpp
    balooimagefetcher.cpp
)

add_library(kokocommon SHARED ${LIB_SRCS})
target_link_libraries(kokocommon
    Qt5::Core
    Qt5::Positioning
    KF5::Baloo
    KVariantStore
)

generate_export_header(kokocommon BASE_NAME KOKO EXPORT_FILE_NAME koko_export.h)

install(TARGETS kokocommon EXPORT KokoLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

#
# Application
#
add_executable(koko
    main.cpp
    reversegeocodelookupjob.cpp
    filesystemtracker.cpp
)

target_link_libraries(koko
    Qt5::Quick
    Qt5::Widgets
    Qt5::Qml
    Qt5::Positioning
    KF5::ConfigCore
    KF5::Baloo
    KF5::FileMetaData
    KVariantStore
    kokocommon
)

install(TARGETS koko ${INSTALL_TARGETS_DEFAULT_ARGS})

#
# QML Plugin
#
set (qml_plugin_SRCS
    qmlplugins.cpp
    tagmodel.cpp
    imagesmodel.cpp
    imagelocationmodel.cpp
    imagetimemodel.cpp
    imagefoldermodel.cpp
    imagelocationcategorizer.cpp
)

add_library (kokoqmlplugin SHARED ${qml_plugin_SRCS})

target_link_libraries (kokoqmlplugin
    Qt5::Qml
    Qt5::Positioning
    KF5::ConfigCore
    KF5::Baloo
    KF5::FileMetaData
    kokocommon
    KVariantStore
)

install (TARGETS kokoqmlplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/koko)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/koko)