#ifndef CIRCLEVIEW_H
#define CIRCLEVIEW_H
#include <QWidget>
#include "IShapePlugin.h"
class CircleView : public QWidget{
public:
    CircleView(QWidget* parent = nullptr); //widgetin parent i belirtilmemişse, görünmez olarak var olsun
    void updateCenter(const Point& center);

protected:
    void paintEvent(QPaintEvent *event) override; //çizim zamanı gelince tetiklenir. Qt ye özel bir fonksiyon

private:
    Point centerPoint; //point struct'ından nesne oluşturduk


};
#endif