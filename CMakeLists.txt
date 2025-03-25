cmake_minimum_required(VERSION 3.25...3.31)

project(qwiiload VERSION 0.9.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Widgets Core Network)

file(GLOB_RECURSE headers src/*.hpp)
file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE guis src/*.ui)

add_executable(qwiiload
    ${guis}
    ${sources}
    ${headers}
    resource.qrc
)

target_link_libraries(qwiiload PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)

include(GNUInstallDirs)

install(TARGETS qwiiload RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES share/qwiiload.svg DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
install(FILES share/qwiiload.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install(FILES share/qwiiload-send.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)

