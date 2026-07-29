#include "circleview.h"
#include <QPainter>

CircleView::CircleView(QWidget *parent): QWidget(parent){ //QWidgetin constructoru parametre istediği için kalıtımıyla beraber yazdık

}

void CircleView::updateCenter(const Point &center){
    centerPoint = center;
    update();
}

void CircleView::paintEvent(QPaintEvent* event){ //burada Qt ye şunu çiz diyoruz
    QPainter painter(this);
    painter.drawEllipse(QPointF(centerPoint.x, centerPoint.y), 1, 1);
}
