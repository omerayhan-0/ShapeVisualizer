#include "squareplugin.h"


SquarePlugin :: SquarePlugin(){
    view = new SquareView();
}


QString SquarePlugin::pluginName() const{
    return "Square";
}

QWidget* SquarePlugin::getWidget(){
    return view;
}

void SquarePlugin::handleMessage(const Point &center){
    view -> updateCenter(center);
}