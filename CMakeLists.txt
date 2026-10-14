cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME cutefish-taskmanager)
project(${PROJECT_NAME})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT Core Gui Quick QuickControls2)
find_package(Qt6 REQUIRED ${QT})

set(SRCS
    src/main.cpp
    src/processmodel.cpp
    src/systemmonitor.cpp
    src/filesystemmodel.cpp
    src/historychartitem.cpp
)

set(RESOURCES
    taskmanager.qrc
)

add_executable(${PROJECT_NAME} ${SRCS} ${RESOURCES})
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::QuickControls2
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /usr/bin)

install(FILES
    cutefish-taskmanager.desktop
    DESTINATION /usr/share/applications/
    COMPONENT Runtime
)

install(FILES
    icons/cutefish-taskmanager.svg
    DESTINATION /usr/share/icons/hicolor/scalable/apps/
    COMPONENT Runtime
)
