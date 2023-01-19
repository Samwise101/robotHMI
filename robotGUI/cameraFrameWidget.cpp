#include "cameraFrameWidget.h"
#include "mainwindow.h"
#include <iostream>
#include <QWidget>
#include <QtGui>
#include <QFrame>




CameraFrameWidget::CameraFrameWidget(QWidget *parent): QWidget(parent)
{
    offset = 10;
    updateCameraPicture = 0;
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
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
        QImage image = QImage((uchar*)frame[actIndex].data, frame[actIndex].cols, frame[actIndex].rows, frame[actIndex].step, QImage::Format_RGB888  );
        painter.drawImage(rectangle,image.rgbSwapped());
    }
}
