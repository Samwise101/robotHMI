#include "cameraFrameWidget.h"
#include "mainwindow.h"
#include <iostream>
#include <QWidget>
#include <QtGui>
#include <QFrame>




CameraFrameWidget::CameraFrameWidget(QWidget *parent): QWidget(parent)
{
    actIndex=-1;
    offset = 10;
    updateCameraPicture = 0;
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    image2 = QImage(":/resource/Alarmy/warning_red.png");
}

CameraFrameWidget::~CameraFrameWidget(){
}

void CameraFrameWidget::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.setBrush(Qt::black);
    QRect rectangle(offset/2, offset/2, this->size().width() - offset, this->size().height() - offset);
    painter.drawRect(rectangle);


    if(updateCameraPicture == 1){
        std::cout<<actIndex<<std::endl;
        image = QImage((uchar*)frame[actIndex].data, frame[actIndex].cols, frame[actIndex].rows, frame[actIndex].step, QImage::Format_RGB888  );
        painter.drawImage(rectangle,image.rgbSwapped());
        //painter.drawText(QPoint(300,300), "Hello");
        painter.drawImage(QPoint(10,10), image2.scaled(100,100, Qt::KeepAspectRatio));
    }
}
