#ifndef SQUAREVIEW_H
#define SQUAREVIEW_H

#include <QWidget>
#include "IShapePlugin.h"

class SquareView : public QWidget{
public:
    SquareView(QWidget* parent = nullptr); //widgetin parent i belirtilmemişse, görünmez olarak var olsun
    void updateCenter(const Point& center);

protected:
    void paintEvent(QPaintEvent *event) override; //çizim zamanı gelince tetiklenir. Qt ye özel bir fonksiyon

private:
    Point centerPoint; //point struct'ından nesne oluşturduk

};
#endif