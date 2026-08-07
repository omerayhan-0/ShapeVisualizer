#include "circleview.h"
#include <QPainter>
CircleView::CircleView(QWidget* parent): QWidget(parent) {
    centerPoint.x = 100;
    centerPoint.y = 100;
}
void CircleView::updateCenter(const Point &center){
    centerPoint = center;
    update();
}
void CircleView::paintEvent(QPaintEvent* event){ //burada Qt ye şunu çiz diyoruz
    QPainter painter(this);
    const qreal scale = 10; // 1 birim = 10 piksel (görsel büyütme)
    QRectF boundingBox(centerPoint.x - scale, centerPoint.y - scale, scale * 2, scale * 2);
    painter.drawEllipse(boundingBox);
}