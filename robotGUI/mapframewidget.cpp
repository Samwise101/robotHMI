#include "mapframewidget.h"
#include <iostream>
#include <QtGui>
#include <QWidget>

MapFrameWidget::MapFrameWidget(QWidget *parent):QWidget{parent}
{
    offset = 10;
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

MapFrameWidget::~MapFrameWidget(){

}

void MapFrameWidget::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.setBrush(Qt::lightGray);
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(5);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    QRect rectangle(offset/2, offset/2, this->size().width() - offset, this->size().height() - offset);
    painter.drawRoundRect(rectangle, 5.0, 5.0);
}
