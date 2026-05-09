cmake_minimum_required(VERSION 3.16)

project(krunner-bigclock VERSION 0.1.0 LANGUAGES CXX)

set(QT_MIN_VERSION "6.5.0")
set(KF_MIN_VERSION "6.0.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Config I18n Runner)

add_library(krunner_bigclock MODULE)

target_sources(krunner_bigclock PRIVATE
    src/bigclockrunner.cpp
    src/bigclockrunner.h
)

target_link_libraries(krunner_bigclock PRIVATE
    KF6::I18n
    KF6::Runner
)

target_compile_definitions(krunner_bigclock PRIVATE
    BIGCLOCK_INSTALL_BINDIR="${KDE_INSTALL_FULL_BINDIR}"
)

set_target_properties(krunner_bigclock PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_executable(krunner-bigclock-window)

target_sources(krunner-bigclock-window PRIVATE
    src/bigclockapp.cpp
    src/bigclockwidget.cpp
    src/bigclockwidget.h
)

target_link_libraries(krunner-bigclock-window PRIVATE
    Qt6::Widgets
)

set_target_properties(krunner-bigclock-window PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

install(TARGETS krunner_bigclock DESTINATION ${KDE_INSTALL_PLUGINDIR}/kf6/krunner)
install(TARGETS krunner-bigclock-window DESTINATION ${KDE_INSTALL_BINDIR})
install(FILES org.kde.krunner-bigclock.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
