#include "mapframewidget.h"
#include <iostream>
#include <QtGui>
#include <QWidget>
#include <math.h>
#include <cmath>

MapFrameWidget::MapFrameWidget(QWidget *parent):QWidget{parent}
{
    offset = 10;
    updateLaserPicture = 0;
    canTriggerEvents = false;
    pointType = 1;
    pointColor = Qt::yellow;
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

    //std::cout << "Rect [x,y]=[" << (float)rectangle.width()/100.0 << "," << (float)rectangle.height()/100.0 << "][m]" << std::endl;

    if(!robotInitialized){
        robotPosition.setX(rectangle.width()/2);
        robotPosition.setY(rectangle.height()/2);
        robotInitialized = true;
    }

    if(canTriggerEvents == 0 && copyOfLaserData.numberOfScans > 0){
        canTriggerEvents = 1;
    };

    if(updateLaserPicture == 1){

        updateLaserPicture = 0;

        pen.setWidth(2);
        pen.setColor(Qt::red);
        painter.setPen(pen);

        // kolesa = vzdialenost 230mm + 10mm = 24
        painter.drawEllipse(robotPosition.x()-15, robotPosition.y()-15, 30, 30);
        painter.drawLine(robotPosition.x(), robotPosition.y(), robotPosition.x()+15*std::cos(realTheta), robotPosition.y()-15*std::sin(realTheta));

        shortestLidarDistance = 10000.0;
        for(int k=0;k<copyOfLaserData.numberOfScans;k++)
        {
            lidarDist=copyOfLaserData.Data[k].scanDistance;
            //std::cout << "Lidar distance=" << lidarDist << std::endl;
            if(lidarDist < shortestLidarDistance && lidarDist > 0.0){
                shortestLidarDistance = lidarDist;
                shortestLidarAngle = copyOfLaserData.Data[k].scanAngle*PI/180;
            }

            pen.setWidth(3);
            pen.setColor(Qt::green);
            painter.setPen(pen);

            // 1000 mm = 100 bodov
            lidarDist = lidarDist/10;
            xp = (robotPosition.x() + lidarDist*sin((360.0-(copyOfLaserData.Data[k].scanAngle)+90)*PI/180+realTheta));
            yp = (robotPosition.y() + lidarDist*cos((360.0-(copyOfLaserData.Data[k].scanAngle)+90)*PI/180+realTheta));

            if(rectangle.contains(xp,yp)){
                painter.drawEllipse(QPoint(xp, yp),2,2);
            }
        }

        if(!points.empty()){
            for(int i = 0; i < points.size(); i++){
                pen.setColor(points[i].getColor());
                painter.setPen(pen);
                painter.setBrush(points[i].getColor());
                painter.drawEllipse(points[i].x(), points[i].y(), 10, 10);
            }
        }
    }
}

void MapFrameWidget::mousePressEvent(QMouseEvent *event){
    if(canTriggerEvents){
        std::cout << "Event triggered: x=" << event->x() << "; y=" << event->y() << std::endl;
        if(points.size() < 10){
           points.insert(points.begin(), RobotGoal(event->x(), event->y(), this->pointType, this->pointColor));
        }
    }
}

bool MapFrameWidget::removeLastPoint()
{
    if(!points.empty()){
        points.pop_back();
        return true;
    }
    return false;
}

void MapFrameWidget::removeAllPoints()
{
    if(!points.empty()){
        points.erase(points.begin(),points.end());
    }
}


void MapFrameWidget::updateRobotValuesForGUI(float& x, float& y, float& theta)
{
    robotPosition.setX(x);
    robotPosition.setY(y);
    //robotRealX = x;
    //robotRealY = y;
    realTheta = theta;
}

void MapFrameWidget::setCanTriggerEvent(bool state){
    canTriggerEvents = state;
}


bool MapFrameWidget::isGoalVectorEmpty()
{
    if(points.empty()){
        return true;
    }
    return false;
}


double MapFrameWidget::getDistanceToFirstPoint()
{
    if(!points.empty()){
        return std::sqrt((points[0].x() - robotPosition.x())^2 + (points[0].y() - robotPosition.y())^2);
    }
    return -1.0;
}

double MapFrameWidget::getShortestDistanceLidar()
{
    return shortestLidarDistance;
}

double MapFrameWidget::getShortestDistanceLidarAngle()
{
    return shortestLidarAngle;
}

void MapFrameWidget::setPointType(int newPointType)
{
    pointType = newPointType;
}

void MapFrameWidget::setPointColor(const QColor &newPointColor)
{
    pointColor = newPointColor;
}

void MapFrameWidget::setOffset(double newOffset)
{
    offset = newOffset;
}

int MapFrameWidget::getGoalYPosition()
{
    return points[points.size()-1].y();
}

int MapFrameWidget::getGoalXPosition()
{
    return points[points.size()-1].x();
}

int MapFrameWidget::getGoalVectorSize()
{
    return points.size();
}

int MapFrameWidget::getGoalType()
{
    return points[points.size()-1].getType();
}


