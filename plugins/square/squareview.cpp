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
    const qreal scale = 10; // 1 birim = 10 piksel (görsel büyütme)
    painter.drawRect(centerPoint.x - scale, centerPoint.y - scale, scale * 2, scale * 2);
}