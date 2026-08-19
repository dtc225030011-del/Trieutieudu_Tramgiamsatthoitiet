cmake_minimum_required(VERSION 3.18)

project(TramThoiTietQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ui")
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    Sql
    Network
    DBus
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MOSQUITTO REQUIRED IMPORTED_TARGET libmosquitto)

qt_add_executable(tram_thoi_tiet_qt
    src/main.cpp
    src/dang_nhap.cpp
    src/quan_ly_tai_khoan.cpp
    src/app_controller.cpp
    src/cua_so_chinh.cpp
    src/mqtt_service.cpp
    src/database_service.cpp
    src/settings_service.cpp
    src/csv_exporter.cpp

    include/cua_so_chinh.h
    include/dang_nhap.h
    include/quan_ly_tai_khoan.h
    include/app_controller.h
    include/mqtt_service.h
    include/database_service.h
    include/sensor_data.h
    include/settings_service.h
    include/csv_exporter.h

    ui/cua_so_chinh.ui
    ui/dang_nhap.ui
    ui/quan_ly_tai_khoan.ui
    resources/resources.qrc
)

target_include_directories(
    tram_thoi_tiet_qt
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    tram_thoi_tiet_qt
    PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    Qt6::DBus
    PkgConfig::MOSQUITTO
)

install(
    TARGETS tram_thoi_tiet_qt
    RUNTIME DESTINATION bin
)
