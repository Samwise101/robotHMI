#include "mapframewidget.h"
#include <iostream>
#include <QtGui>
#include <QWidget>

MapFrameWidget::MapFrameWidget(QWidget *parent):QWidget{parent}
{
    offset = 10;
    updateLaserPicture = 0;
    canTriggerEvents = false;
    distance = 0;
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

MapFrameWidget::~MapFrameWidget(){

}

void MapFrameWidget::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.setBrush(Qt::black);
    QPen pen;
    pen.setStyle(Qt::SolidLine);

    QRect rectangle(offset/2, offset/2, this->size().width() - offset, this->size().height() - offset);
    painter.drawRect(rectangle);
    middle.setX(rectangle.width()/2);
    middle.setY(rectangle.height()/2);

    if(canTriggerEvents == 0 && copyOfLaserData.numberOfScans > 0){
        canTriggerEvents = 1;
    };

    if(updateLaserPicture == 1){

        updateLaserPicture = 0;

        pen.setWidth(2);
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.drawEllipse(middle.x(), middle.y(), 30, 30);
        painter.drawLine(middle.x()+15, middle.y(), middle.x()+15, middle.y()+15);

        pen.setWidth(3);
        pen.setColor(Qt::green);
        painter.setPen(pen);

        for(int k=0;k<copyOfLaserData.numberOfScans;k++)
        {
            int dist=copyOfLaserData.Data[k].scanDistance/20;
            int xp=(rectangle.width()/2 + dist*2*sin((360.0-copyOfLaserData.Data[k].scanAngle +180)*3.14159/180.0))+rectangle.topLeft().x();
            int yp=(rectangle.height()/2 + dist*2*cos((360.0-copyOfLaserData.Data[k].scanAngle +180)*3.14159/180.0))+rectangle.topLeft().y();
            if(rectangle.contains(xp,yp))
                painter.drawEllipse(QPoint(xp, yp),2,2);
                //std::cout << "middle = [" << middle.x() << "," << middle.y()
                         // << "]; LIDAR_point = [" << xp << "," << yp << "]" << std::endl;
        }

        if(!points.empty()){
            pen.setColor(Qt::yellow);
            painter.setPen(pen);
            painter.setBrush(Qt::yellow);

            for(int i = 0; i < points.size(); i++){
                disX = points[i].x() - middle.x();
                int xt = oldDisX - disX;
                oldDisX = disX;
                disY = points[i].y() - middle.y();
                int yt = oldDisY - disY;
                oldDisY = disY;
                painter.drawEllipse(points[i].x(), points[i].y(), 10, 10);
                //points[i].setY(points[i].y() - yt);
            }
        }
    }
}

void MapFrameWidget::mousePressEvent(QMouseEvent *event){
    if(canTriggerEvents){
        std::cout << "Event triggered: x=" << event->x() << "; y=" << event->y() << std::endl;
        if(points.size() < 10){
           points.push_back(QPoint(event->x(), event->y()));
        }
    }
}

void MapFrameWidget::setCanTriggerEvent(bool state){
    canTriggerEvents = state;
}

void MapFrameWidget::setDistance(int s){
    distance = s;
}

std::vector<QPoint> MapFrameWidget::getPoints(){
    return points;
}
