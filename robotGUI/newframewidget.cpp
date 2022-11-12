#include "newframewidget.h"
#include "mainwindow.h"
#include <iostream>
#include <QWidget>
#include <QtGui>
#include <QFrame>


NewFrameWidget::NewFrameWidget(QWidget *parent): QWidget(parent)
{
    offset = 10;
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

NewFrameWidget::~NewFrameWidget(){
}

void NewFrameWidget::paintEvent(QPaintEvent* event){
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

