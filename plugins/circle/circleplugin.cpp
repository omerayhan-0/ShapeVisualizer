#include "circleplugin.h"

QString CirclePlugin::pluginName() const{
    return "Circle";
}

QWidget* CirclePlugin::getWidget(){
    return &view; // adresini veriyoruz
}

void CirclePlugin::handleMessage(const Point &center){
    view.updateCenter(center);
}