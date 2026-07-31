#include "circleplugin.h"


CirclePlugin :: CirclePlugin(){
    view = new CircleView();
}


QString CirclePlugin::pluginName() const{
    return "Circle";
}

QWidget* CirclePlugin::getWidget(){
    return view;
}

void CirclePlugin::handleMessage(const Point &center){
    view -> updateCenter(center);
}