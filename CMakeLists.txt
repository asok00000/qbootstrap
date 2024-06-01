cmake_minimum_required(VERSION 3.14)

project(qbootstrap LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../libs)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)
# include_directories(${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})
set(EXPORT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/qbootstrap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/qbootstrap_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/svgicon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/animatewidgets.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basewidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/anchormanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/maskwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tooltipmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tooltip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/elementstyle.h
)
add_library(qbootstrap SHARED
  qbootstrap.cpp
  ${EXPORT_HEADERS}
  icons.qrc
  svgicon.cpp
  animatewidgets.cpp
  basewidget.cpp
  anchormanager.cpp
  maskwidget.cpp
  tooltipmanager.cpp
  tooltip.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qss/qcssparser_p.h
  qss/qcssparser.cpp
  qss/stylehelper.h qss/stylehelper.cpp
  elementstyle.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
)

target_compile_definitions(${PROJECT_NAME} PRIVATE QBOOSTRAP_LIBRARY)

add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/../includes/qboostrap
    COMMAND ${CMAKE_COMMAND} -E copy ${EXPORT_HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}/../includes/qboostrap
)
