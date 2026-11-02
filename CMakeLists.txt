cmake_minimum_required(VERSION 3.19)
project(aerostatTelemetrySystem1 LANGUAGES CXX)

# Charts kütüphanesi buraya eklendi
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql Charts)

qt_standard_project_setup()

qt_add_executable(aerostatTelemetrySystem1
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    TelemetryData.h
    telemetrysimulator.h telemetrysimulator.cpp
    databasemanager.h databasemanager.cpp
    alarmmanager.h alarmmanager.cpp
    logindialog.h logindialog.cpp logindialog.ui

)

# Qt::Charts bağlaması buraya eklendi
target_link_libraries(aerostatTelemetrySystem1
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql
        Qt::Charts
)

install(TARGETS aerostatTelemetrySystem1
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET aerostatTelemetrySystem1
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})