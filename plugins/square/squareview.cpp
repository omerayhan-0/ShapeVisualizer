#include "squareview.h"
#include <QPainter>

SquareView::SquareView(QWidget* parent): QWidget(parent) {
    centerPoint.x = 100;
    centerPoint.y = 100;
}

void SquareView::updateCenter(const Point &center){
    centerPoint = center;
    update();
}

void SquareView::paintEvent(QPaintEvent* event){ //burada Qt ye şunu çiz diyoruz
    QPainter painter(this);
    painter.drawRect(centerPoint.x - 10, centerPoint.y - 10, 20, 20);
}
