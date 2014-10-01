project(Gallery)
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS Qml Quick Test)
find_package(KF5 REQUIRED COMPONENTS Plasma I18n Declarative Config)

include(FeatureSummary)
include(ECMAddTests)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(GenerateExportHeader)

add_subdirectory(lib)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)