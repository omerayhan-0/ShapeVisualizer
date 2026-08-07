cmake_minimum_required(VERSION 3.19)
project(ShapeVisualizer LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(ShapeVisualizer
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    shared/IShapePlugin.h
    pluginmanager.h
    pluginmanager.cpp
    udpworker.cpp
    udpworker.h
)

target_link_libraries(ShapeVisualizer
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

install(TARGETS ShapeVisualizer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ShapeVisualizer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# plugins/ altindaki her klasoru otomatik bul ve derlemeye dahil et
file(GLOB plugin_dirs LIST_DIRECTORIES true ${CMAKE_SOURCE_DIR}/plugins/*)
foreach(dir ${plugin_dirs})
    if(IS_DIRECTORY ${dir})
        add_subdirectory(${dir})
    endif()
endforeach()