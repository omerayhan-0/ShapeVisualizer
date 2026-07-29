#Circle plugin - dairesel şekil çizen eklenti, IShapePlugin Arayüzünü uyguluyor
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_add_library(circleplugin SHARED
    circleplugin.h
    circleplugin.cpp
    circleview.h
    circleview.cpp
)

target_link_libraries(circleplugin
    PRIVATE
        Qt::Core
        Qt::Widgets
    )

#shared klasöründeki ISharedPlugin.h ye erişebilmek için include path ekliyoruz
target_include_directories(circleplugin
    PRIVATE
        ${CMAKE_SOURCE_DIR}/shared
    )