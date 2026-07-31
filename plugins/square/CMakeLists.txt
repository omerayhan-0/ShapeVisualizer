#Square plugin - kare şekil çizen eklenti, IShapePlugin Arayüzünü uyguluyor
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_add_library(squareplugin SHARED
    squareplugin.h
    squareplugin.cpp
    squareview.h
    squareview.cpp
)

target_link_libraries(squareplugin
    PRIVATE
        Qt::Core
        Qt::Widgets
    )

#shared klasöründeki ISharedPlugin.h ye erişebilmek için include path ekliyoruz
target_include_directories(squareplugin
    PRIVATE
        ${CMAKE_SOURCE_DIR}/shared
    )