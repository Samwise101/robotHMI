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
    placeGoals = true;
    pointType = 1;
    number2 = 0;
    pointColor = Qt::yellow;
    imageWidth = this->size().width() - offset;
    imageHeight = this->size().height() - offset;
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

MapFrameWidget::~MapFrameWidget(){

}

void MapFrameWidget::paintEvent(QPaintEvent*){
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
        realTheta = 0;
        imageWidth = this->size().width() - offset;
        imageHeight = this->size().height() - offset;
        robotInitialized = true;
    }

    if(updateLaserPicture == 1){
        if(robotOnline){
            if(canTriggerEvents == 0 && copyOfLaserData.numberOfScans > 0){
                canTriggerEvents = 1;
            };

            sectionsX = rectangle.width()/100;
            sectionsY = rectangle.height()/100;

            updateLaserPicture = 0;

             /* pen.setWidth(1);
                pen.setColor(Qt::darkGray);
                painter.setPen(pen);

                for(int i = 1; i <= sectionsX; i++){
                    painter.drawLine(i*100+offset/2, offset/2, i*100+offset/2, rectangle.height()+offset/2);
                    painter.drawLine(offset/2, i*100+offset/2, rectangle.width()+offset/2, i*100+offset/2);
                }
             */
             pen.setWidth(2);
             pen.setColor(Qt::red);
             painter.setPen(pen);

             robotPosition.setX(robotPosition.x()*scale);
             robotPosition.setY(robotPosition.y()*scale);

             painter.drawEllipse(robotPosition.x()-15*scale, robotPosition.y()-15*scale, 30*scale, 30*scale);
             painter.drawLine(robotPosition.x(), robotPosition.y(), robotPosition.x()+15*std::cos(realTheta)*scale, robotPosition.y()-15*std::sin(realTheta)*scale);

             shortestLidarDistance = 10000.0;
             for(int k=0;k<copyOfLaserData.numberOfScans;k++)
             {
                 lidarDist=copyOfLaserData.Data[k].scanDistance;
                 if(lidarDist < shortestLidarDistance && lidarDist > 0.0){
                    shortestLidarDistance = lidarDist;
                    shortestLidarAngle = copyOfLaserData.Data[k].scanAngle*PI/180;
                 }

                 pen.setWidth(3);
                 pen.setColor(Qt::green);
                 painter.setPen(pen);

                 // 1000 mm = 100 bodov
                 lidarDist = lidarDist/10*scale;
                 xp = (robotPosition.x() + lidarDist*sin((360.0-(copyOfLaserData.Data[k].scanAngle)+90)*PI/180+realTheta) + rectangle.topLeft().x());
                 yp = (robotPosition.y() + lidarDist*cos((360.0-(copyOfLaserData.Data[k].scanAngle)+90)*PI/180+realTheta) + rectangle.topLeft().y());


                 if(rectangle.contains(xp,yp)){
                    if(scale < 1.0){
                       painter.drawEllipse(QPoint(xp, yp),1,1);
                    }
                    else{
                       painter.drawEllipse(QPoint(xp, yp),2,2);
                    }
                }
              }

              if(!points.empty()){
                    for(int i = 0; i < points.size(); i++){
                        pen.setColor(points[i].getColor());
                        painter.setPen(pen);
                        painter.setBrush(points[i].getColor());
                        painter.drawEllipse(points[i].x()*scale, points[i].y()*scale, 10*scale, 10*scale);
                    }
                }
            }
        else{
            if(!str.empty() && updateLaserPicture == 1){

                temp1 = str.substr(0, str.find(";"));
                temp2 = str.substr(str.find(";") + 1, str.find("\n"));

                pos = temp1.find(",");
                token = temp1.substr(0, pos);
                temp1.erase(0, pos + 1);
                robotImagePos.setX(std::stoi(token));

                pos = temp1.find(",");
                token = temp1.substr(0, pos);
                temp1.erase(0, pos + 1);
                robotImagePos.setY(std::stoi(token));

                pos = temp1.find(",");
                token = temp1.substr(0, pos);
                temp1.erase(0, pos + 1);
                imageTheta = std::stod(token);

                pen.setWidth(2);
                pen.setColor(Qt::red);
                painter.setPen(pen);

                painter.drawEllipse(robotImagePos.x()-15*scale, robotImagePos.y()-15*scale, 30*scale, 30*scale);
                painter.drawLine(robotImagePos.x(), robotImagePos.y(), robotImagePos.x()+15*std::cos(imageTheta)*scale, robotImagePos.y()-15*std::sin(imageTheta)*scale);

                std::cout << str << std::endl;

                while(pos != std::string::npos){
                      pos = temp1.find(",");
                      std::cout << pos << std::endl;
                      token = temp1.substr(0, pos);
                      temp1.erase(0, pos + 1);
                      xp = std::stoi(token);

                      pos = temp1.find(",");
                      token = temp1.substr(0, pos);
                      temp1.erase(0, pos + 1);
                      yp = std::stoi(token);

                      if(scale < 1.0){
                         painter.drawEllipse(QPoint(xp, yp),1,1);
                      }
                      else{
                         painter.drawEllipse(QPoint(xp, yp),2,2);
                      }
                }

                pen.setWidth(3);
                pen.setColor(Qt::green);
                painter.setPen(pen);

                pos = 0;

                while(pos != std::string::npos){

                      pos = temp2.find(",");
                      token = temp2.substr(0, pos);
                      temp2.erase(0, pos + 1);
                      xp = std::stoi(token);

                      pos = temp2.find(",");
                      token = temp2.substr(0, pos);
                      temp2.erase(0, pos + 1);
                      yp = std::stoi(token);

                      if(scale < 1.0){
                         painter.drawEllipse(QPoint(xp, yp),1,1);
                      }
                      else{
                         painter.drawEllipse(QPoint(xp, yp),2,2);
                      }
                }

            }
        }
    }
}

void MapFrameWidget::setNumber2(int newNumber2)
{
    number2 = newNumber2;
}

void MapFrameWidget::setStr(const std::string &newStr)
{
    str = newStr;
}

const std::string &MapFrameWidget::getStr() const
{
    return str;
}

void MapFrameWidget::setRobotOnline(bool newRobotOnline)
{
    robotOnline = newRobotOnline;
}

void MapFrameWidget::createFrameLog(float& timepassed, fstream& file)
{
    QRect rectTest(offset/2, offset/2, imageWidth, imageHeight);

    file  << robotImagePos.x() << "," << robotImagePos.y() << "," << realTheta;

    if(number2%5 == 0){
        robotPositionInTime.push_back(QPoint(robotImagePos.x(),robotImagePos.y()));
    }
    number2++;

    for(int i = 0; i < robotPositionInTime.size(); i++){;
        file << "," << robotPositionInTime[i].x() <<  "," << robotPositionInTime[i].y();
    }

    file << ';';
    number = 0;

    for(int k=0;k<copyOfLaserData.numberOfScans;k++)
    {
        lidarDistImage = copyOfLaserData.Data[k].scanDistance/10;
        xp2 = (robotImagePos.x() + lidarDistImage*sin((360.0-(copyOfLaserData.Data[k].scanAngle)+90)*PI/180+realTheta) + rectTest.topLeft().x());
        yp2 = (robotImagePos.y() + lidarDistImage*cos((360.0-(copyOfLaserData.Data[k].scanAngle)+90)*PI/180+realTheta) + rectTest.topLeft().y());

        if(rectTest.contains(xp2,yp2)){
            if((number < copyOfLaserData.numberOfScans) && number%10 == 0){
                if(number == 0){
                   file << xp2 << "," << yp2;
                }
                else{
                   file << "," << xp2 << "," << yp2;
                }
            }
            number++;
        }
    }
    file << "\n";
}

void MapFrameWidget::setScale(float newScale)
{
    scale = newScale;
}

void MapFrameWidget::mousePressEvent(QMouseEvent *event){
    if(canTriggerEvents && placeGoals){
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


void MapFrameWidget::updateRobotValuesForGUI(double& x, double& y, double& theta)
{
    robotPosition.setX(x);
    robotPosition.setY(y);
    robotImagePos.setX(x);
    robotImagePos.setY(y);
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

double MapFrameWidget::getLidarAngle()
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
    return (int)points.size();
}

int MapFrameWidget::getGoalType()
{
    return points[points.size()-1].getType();
}

void MapFrameWidget::setPlaceGoals(bool newPlaceGoals)
{
    placeGoals = newPlaceGoals;
}

