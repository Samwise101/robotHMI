#include "mapframewidget.h"
#include <iostream>
#include <QtGui>
#include <QWidget>
#include <math.h>
#include <cmath>
#include <QFont>

MapFrameWidget::MapFrameWidget(QWidget *parent):QWidget{parent}
{
    offset = 10;
    updateLaserPicture = 0;
    canTriggerEvents = false;
    robotOnline = false;
    placeGoals = true;

    posMouseTrack = true;
    robotControlOn = true;

    showReplayWarning = false;
    showDisconnectWarning = false;
    showRobotStopped = false;
    showMap = true;

    robotInitialized = false;
    isSimulation = false;

    robotXPos = 0;
    robotYPos = 0;

    replayIndex = 0;

    pointType = 1;
    number2 = 0;
    pointColor = QColor(20,255,20);
    imageWidth = this->size().width() - offset;
    imageHeight = this->size().height() - offset;
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setMouseTracking(true);
    this->setAttribute(Qt::WA_Hover);

}

MapFrameWidget::~MapFrameWidget(){

}

void MapFrameWidget::paintEvent(QPaintEvent*){
    QPainter painter(this);
    painter.setBrush(Qt::black);
    QPen pen;
    pen.setStyle(Qt::SolidLine);

    screen = this->window()->windowHandle()->screen();

    this->setFixedWidth(screen->geometry().width()*scaleFactorWidth/1920);
    this->setFixedHeight(screen->geometry().height()*scaleFactorHeight/1080);

    QRect rectangle(offset/2, offset/2, this->width() - offset, this->height() - offset);

    scale = ((this->width()-15)/574.0f)- ((this->width()-15)/574.0f)/20;

    rectMiddleX = rectangle.width()/2;
    rectMiddleY = rectangle.height()/2;

    painter.drawRect(rectangle);

    pen.setWidth(3);
    pen.setColor(QColor(30,144,255));
    painter.setPen(pen);

    if(updateLaserPicture == 1){

        if(showMap){
            paintMap(&painter);
        }

        if(robotOnline && robotInitialized){
            if(canTriggerEvents == 0 && copyOfLaserData.numberOfScans > 0){
                canTriggerEvents = 1;
            }

            updateLaserPicture = 0;

            if(showRobotStopped){
                painter.setFont(QFont("Segoe UI",8*scale));
                pen.setColor(QColor(255,165,0,255));
                painter.setPen(pen);
                painter.drawText(rectMiddleX-100*scale, 80*scale, "Robot predišiel zrážke a núdzovo zastal!");
            }

            else if(showDisconnectWarning){
                painter.setFont(QFont("Segoe UI",8*scale));
                pen.setColor(QColor(255,165,0,255));
                painter.setPen(pen);
                painter.drawText(rectMiddleX-150*scale, 80*scale, "Pre odpojenie robota stlačte najprv tlačidlo STOP!");
            }

            else if(showReplayWarning){
                painter.setFont(QFont("Segoe UI",8*scale));
                pen.setColor(QColor(255,165,0,255));
                painter.setPen(pen);
                painter.drawText(rectMiddleX-190*scale, 80*scale, "Pre zapnutie prehrávania najprv prepnite robot do režimu STOP!");
            }

            pen.setWidth(3);
            pen.setColor(QColor(30,144,255));
            painter.setPen(pen);

            if(!trajectories.empty()){
                for(int i = 0; i < trajectories.size(); i++)
                {
                    painter.drawEllipse(trajectories[i].x()*scale, trajectories[i].y()*scale, 2*scale, 2*scale);
                }
            }

            pen.setWidth(2);
            pen.setColor(QColor(255,223,0));
            painter.setPen(pen);

            painter.drawEllipse(robotPosition.x()-20*scale, robotPosition.y()-20*scale, 40*scale, 40*scale);
            painter.drawLine(robotPosition.x(), robotPosition.y(), robotPosition.x()+20*std::cos(realTheta)*scale, robotPosition.y()-20*std::sin(realTheta)*scale);

            shortestLidarDistance = 10000.0;
            for(int k=0;k<copyOfLaserData.numberOfScans;k++)
            {
                lidarDist=copyOfLaserData.Data[k].scanDistance;
                if(lidarDist < shortestLidarDistance && lidarDist > 0.0){
                   shortestLidarDistance = lidarDist;
                   shortestLidarAngle = copyOfLaserData.Data[k].scanAngle*PI/180;
                }

                pen.setWidth(3);
                pen.setColor(QColor(255,223,0));
                painter.setPen(pen);

                 // 1000 mm = 100 bodov
                lidarDist = lidarDist/10*scale;
                xp = (robotPosition.x() + lidarDist*sin((360.0-(copyOfLaserData.Data[k].scanAngle)+90)*PI/180+realTheta) + rectangle.topLeft().x());
                yp = (robotPosition.y() + lidarDist*cos((360.0-(copyOfLaserData.Data[k].scanAngle)+90)*PI/180+realTheta) + rectangle.topLeft().y());


                if(rectangle.contains(xp,yp) && lidarDist*10/scale <= 3000){
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
                        painter.drawEllipse(points[i].x()*scale - 5*scale, points[i].y()*scale - 5*scale, 10*scale, 10*scale);
                    }
                }

              pen.setWidth(3*scale);
              pen.setColor(QColor(255,223,0));
              painter.setPen(pen);

              if(posMouseTrack){
                paintMouseCoord(rectangle, &painter);
              }
        }
        else if(!robotReplayPos.empty()){

            if(scale >=1){
                robotImagePos.setX(robotReplayPos[replayIndex][0]);
                robotImagePos.setY(robotReplayPos[replayIndex][1]);
            }
            else{
                robotImagePos.setX(robotReplayPos[replayIndex][0]*0.535*scale);
                robotImagePos.setY(robotReplayPos[replayIndex][1]*0.535*scale);
            }
            imageTheta = robotAngle[replayIndex];

            if(replayIndex == 0){
                startLocation.setX(robotImagePos.x() - 28*scale);
                startLocation.setY(robotImagePos.y() - 28*scale);
            }

            pen.setWidth(2);
            pen.setColor(QColor(255,223,0));
            painter.setPen(pen);

            painter.drawEllipse(robotImagePos.x()-20*scale, robotImagePos.y()-20*scale, 40*scale, 40*scale);
            painter.drawLine(robotImagePos.x(), robotImagePos.y(), robotImagePos.x()+20*std::cos(imageTheta)*scale, robotImagePos.y()-20*std::sin(imageTheta)*scale);


            for(int i = 0; i < robotTrajectory[replayIndex].size(); i++){
                painter.drawLine(robotTrajectory[replayIndex][i].x1()*scale,robotTrajectory[replayIndex][i].y1()*scale,robotTrajectory[replayIndex][i].x2()*scale,robotTrajectory[replayIndex][i].y2()*scale);
            }

            painter.setBrush(QColor(255,223,0));

            for(int i = 0; i < lidarReplayPos[replayIndex].size(); i++){
                if(scale < 1.0){
                   painter.drawEllipse(lidarReplayPos[replayIndex][i].x()*scale, lidarReplayPos[replayIndex][i].y()*scale,2,2);
                }
                else{
                   painter.drawEllipse(lidarReplayPos[replayIndex][i].x()*scale, lidarReplayPos[replayIndex][i].y()*scale,6,6);
                }
            }

            if(replayIndex < missionReplayPoints.size()){
            for(int i = 0; i < missionReplayPoints[replayIndex].size(); i+=3){
                xp = missionReplayPoints[replayIndex][i];
                yp = missionReplayPoints[replayIndex][i+1];
                goalColor = missionReplayPoints[replayIndex][i+2];

                if(goalColor == 1){
                   pen.setColor(QColor(20,255,20));
                   painter.setBrush(QColor(20,255,20));
                   painter.setPen(pen);
                }
                else if(goalColor == 2){
                   pen.setColor(Qt::darkMagenta);
                   painter.setBrush(Qt::darkMagenta);
                   painter.setPen(pen);
                }
                else if(goalColor == 3){
                   pen.setColor(Qt::cyan);
                   painter.setBrush(Qt::cyan);
                   painter.setPen(pen);
                }
                else if(goalColor == 4){
                   pen.setColor(QColor(192,192,192));
                   painter.setBrush(QColor(192,192,192));
                   painter.setPen(pen);
                }

                if(scale < 1.0){
                   painter.drawEllipse(QPoint(xp*scale, yp*scale),2,2);
                }
                else{
                   painter.drawEllipse(QPoint(xp*scale, yp*scale),6,6);
                }
            }

            replayIndex++;

            if(replayIndex >= robotReplayPos.size() - 1){
                replayFinished = true;
            }
        }
        }
    }
}

void MapFrameWidget::setScaleFactorHeight(int newScaleFactorHeight)
{
    scaleFactorHeight = newScaleFactorHeight;
}

void MapFrameWidget::setScaleFactorWidth(int newScaleFactorWidth)
{
    scaleFactorWidth = newScaleFactorWidth;
}

std::vector<QPoint>* MapFrameWidget::getTrajectories()
{
    return &trajectories;
}

const std::vector<QPoint> &MapFrameWidget::getTrajcPoints() const
{
    return trajcPoints;
}

void MapFrameWidget::setRobotControlOn(bool newRobotControlOn)
{
    robotControlOn = newRobotControlOn;
}

void MapFrameWidget::insertToGoals(int x, int y, int type, QColor color)
{
    if(points.size() < 10 && robotOnline && robotControlOn){
        points.insert(points.begin(), RobotGoal(x, y, type, color));
    }
}

bool MapFrameWidget::getShowRobotStopped() const
{
    return showRobotStopped;
}

void MapFrameWidget::setShowRobotStopped(bool newShowRobotStopped)
{
    showRobotStopped = newShowRobotStopped;
}


void MapFrameWidget::createFrameLog(fstream& file)
{
    QRect rectTest(offset/2, offset/2, imageWidth, imageHeight);

    file  << robotImagePos.x() << "," << robotImagePos.y() << "," << realTheta;

    if(number2%4 == 0){
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

        if(rectTest.contains(xp2,yp2) && lidarDistImage*10 <= 3000){
            if((number < copyOfLaserData.numberOfScans) && number%7 == 0){
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
    file << ";";
    if(points.empty()){
       file << "1";
    }
    else{
       for(int i = 0; i < points.size(); i++){
            if(i == 0)
                file << points[i].x() << "," << points[i].y() << "," << points[i].getType();
            else
                file << "," << points[i].x() << "," << points[i].y() << "," << points[i].getType();
       }
    }
    file << "\n";
}


void MapFrameWidget::parseMapFile()
{
    if(!str.empty()){

        pos = str.find(";");

        if(pos != std::string::npos){
            temp1 = str.substr(0, pos);

            str.erase(0, pos + 1);

            pos = temp1.find(",");
            token = temp1.substr(0, pos);
            temp1.erase(0, pos + 1);
            robotPos.push_back(std::stoi(token)*scale);

            pos = temp1.find(",");
            token = temp1.substr(0, pos);
            temp1.erase(0, pos + 1);
            robotPos.push_back(std::stoi(token)*scale);

            pos = temp1.find(",");
            token = temp1.substr(0, pos);
            temp1.erase(0, pos + 1);
            robotAngle.push_back(std::stod(token));


            robotReplayPos.push_back(robotPos);
            robotPos.clear();

            pos = temp1.find(",");
            token = temp1.substr(0, pos);
            temp1.erase(0, pos + 1);
            xp = std::stoi(token);

            pos = temp1.find(",");
            token = temp1.substr(0, pos);
            temp1.erase(0, pos + 1);
            yp = std::stoi(token);

            pos = temp1.find(",");


            while(pos != std::string::npos){
                  pos = temp1.find(",");
                  token = temp1.substr(0, pos);
                  temp1.erase(0, pos + 1);
                  xp2 = std::stoi(token);

                  pos = temp1.find(",");
                  token = temp1.substr(0, pos);
                  temp1.erase(0, pos + 1);
                  yp2 = std::stoi(token);

                  replayTrajectory.push_back(QLine(xp,yp,xp2,yp2));

                  xp = xp2;
                  yp = yp2;
            }
            robotTrajectory.push_back(replayTrajectory);
            replayTrajectory.clear();
        }

        pos = str.find(";");

        if(pos != std::string::npos){
            temp2 = str.substr(0, pos);
            str.erase(0, pos + 1);

            pos = temp2.find(",");

            while(pos != std::string::npos){
                  pos = temp2.find(",");
                  token = temp2.substr(0, pos);
                  temp2.erase(0, pos + 1);
                  xp = std::stoi(token);

                  pos = temp2.find(",");
                  token = temp2.substr(0, pos);
                  temp2.erase(0, pos + 1);
                  yp = std::stoi(token);

                  lidarReplayPoints.push_back(QPoint(xp,yp));
            }

            lidarReplayPos.push_back(lidarReplayPoints);
            lidarReplayPoints.clear();
        }

        if(!str.empty()){
            temp3 = str;

            if(temp3.size() >= 3){
                pos = temp3.find(",");

                while(pos != std::string::npos){
                    pos = temp3.find(",");
                    token = temp3.substr(0, pos);
                    temp3.erase(0, pos + 1);
                    xp = std::stoi(token);

                    pos = temp3.find(",");
                    token = temp3.substr(0, pos);
                    temp3.erase(0, pos + 1);
                    yp = std::stoi(token);

                    pos = temp3.find(",");;
                    token = temp3.substr(0, pos);
                    temp3.erase(0, pos + 1);
                    goalColor = std::stoi(token);

                    missionPoints.push_back(xp);
                    missionPoints.push_back(yp);
                    missionPoints.push_back(goalColor);
                }

                missionReplayPoints.push_back(missionPoints);
                missionPoints.clear();
            }
        }
    }
}

void MapFrameWidget::mousePressEvent(QMouseEvent *event){
    if(canTriggerEvents && placeGoals){
        if(points.size() < 10 && robotOnline && robotControlOn){
           points.insert(points.begin(), RobotGoal(event->x()/scale, event->y()/scale, this->pointType, this->pointColor));
        }
    }
}

bool MapFrameWidget::event(QEvent * event)
{
   if(event->type() == QEvent::HoverLeave){
      hoverLeave(static_cast<QHoverEvent*>(event));
      return true;
   }

   return QWidget::event(event);
}


void MapFrameWidget::hoverLeave(QHoverEvent  *event)
{
    mouseXPos = -1;
    mouseYPos = -1;
}

void MapFrameWidget::mouseMoveEvent(QMouseEvent *event){

    if (event->type() == QEvent::MouseMove)
    {
        mouseXPos = event->x();
        mouseYPos = event->y();
        mouseToMapX = mouseXPos - 15*scale;
        mouseToMapY = mouseYPos + 15*scale;
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

void MapFrameWidget::initializeRobot()
{
    if(!robotInitialized){

        if(isSimulation){
            // simulacia
            robotXPos = rectMiddleX - scale*239;
            robotYPos = rectMiddleY + 184*scale;
            realTheta = 0;
        }
        else{
            // real
            robotXPos = rectMiddleX - scale*228;
            robotYPos = rectMiddleY + 165*scale;
            realTheta = 0;
        }

        startLocation.setX(robotXPos - 28*scale);
        startLocation.setY(robotYPos - 28*scale);

/*
        robotXPos = rectMiddleX + scale*(287 - 57);
        robotYPos = rectMiddleY + scale*(235-250);
        realTheta = -PI/2;
*/
        robotPosition.setX(robotXPos/scale);
        robotPosition.setY(robotYPos/scale);
        robotImagePos.setX(robotXPos/scale);
        robotImagePos.setY(robotYPos/scale);

        imageWidth = this->size().width() - offset;
        imageHeight = this->size().height() - offset;
        robotInitialized = true;
    }
}

void MapFrameWidget::removeAllPoints()
{
    if(!points.empty()){
        points.erase(points.begin(),points.end());
    }
}


void MapFrameWidget::updateRobotValuesForGUI(double& x, double& y, double& theta)
{
    robotXPos = x;
    robotYPos = y;

    robotPosition.setX(robotXPos*scale);
    robotPosition.setY(robotYPos*scale);

    robotImagePos.setX(robotXPos);
    robotImagePos.setY(robotYPos);
    realTheta = theta;
}

void MapFrameWidget::paintMap(QPainter *aPainter)
{
    line1.setLine((rectMiddleX-scale*287),(rectMiddleY+scale*235),(rectMiddleX+scale*287),(rectMiddleY+scale*235));
    line2.setLine((rectMiddleX+scale*287),(rectMiddleY+scale*235),(rectMiddleX+scale*287),(rectMiddleY-scale*225));
    line3.setLine((rectMiddleX+scale*287),(rectMiddleY-scale*225),(rectMiddleX+scale*263),(rectMiddleY-scale*225));
    line4.setLine((rectMiddleX+scale*263),(rectMiddleY-scale*225),(rectMiddleX+scale*263),(rectMiddleY-scale*236));
    lines = {line1, line2, line3, line4};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX+scale*263),(rectMiddleY-scale*236),(rectMiddleX-scale*232),(rectMiddleY-scale*236));
    line2.setLine((rectMiddleX-scale*232),(rectMiddleY-scale*236),(rectMiddleX-scale*232),(rectMiddleY-scale*196));
    line3.setLine((rectMiddleX-scale*232),(rectMiddleY-scale*196),(rectMiddleX-scale*287),(rectMiddleY-scale*196));
    line4.setLine((rectMiddleX-scale*287),(rectMiddleY-scale*196),(rectMiddleX-scale*287),(rectMiddleY+scale*235));
    lines = {line1, line2, line3, line4};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine(rectMiddleX-scale*23,rectMiddleY+scale*235,rectMiddleX-scale*23,rectMiddleY+scale*82);
    line2.setLine((rectMiddleX-scale*23),rectMiddleY+scale*82,rectMiddleX-scale*20,rectMiddleY+scale*82);
    line3.setLine((rectMiddleX-scale*20),rectMiddleY+scale*82,rectMiddleX-scale*20,rectMiddleY+scale*235);
    lines = {line1, line2, line3};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX-scale*23),(rectMiddleY+scale*85),(rectMiddleX-scale*23),(rectMiddleY+scale*82));
    line2.setLine((rectMiddleX-scale*23),(rectMiddleY+scale*82),(rectMiddleX-scale*177),(rectMiddleY+scale*82));
    line3.setLine((rectMiddleX-scale*177),(rectMiddleY+scale*82),(rectMiddleX-scale*177),(rectMiddleY+scale*85));
    line4.setLine((rectMiddleX-scale*177),(rectMiddleY+scale*85),(rectMiddleX-scale*23),(rectMiddleY+scale*85));
    lines = {line1, line2, line3, line4};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX-scale*177),(rectMiddleY+scale*85),(rectMiddleX-scale*177),(rectMiddleY-scale*74));
    line2.setLine((rectMiddleX-scale*177),(rectMiddleY-scale*74),(rectMiddleX-scale*173),(rectMiddleY-scale*74));
    line3.setLine((rectMiddleX-scale*173),(rectMiddleY-scale*74),(rectMiddleX-scale*173),(rectMiddleY+scale*82));
    lines = {line1, line2, line3};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX+scale*287),(rectMiddleY-scale*74),(rectMiddleX+scale*287),(rectMiddleY-scale*77));
    line2.setLine((rectMiddleX+scale*287),(rectMiddleY-scale*77),(rectMiddleX+scale*78),(rectMiddleY-scale*77));
    line3.setLine((rectMiddleX+scale*78),(rectMiddleY-scale*77),(rectMiddleX+scale*78),(rectMiddleY-scale*74));
    line4.setLine((rectMiddleX+scale*78),(rectMiddleY-scale*74),(rectMiddleX+scale*287),(rectMiddleY-scale*74));
    lines = {line1, line2, line3, line4};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX+scale*136),(rectMiddleY-scale*74),(rectMiddleX+scale*136),(rectMiddleY+scale*82));
    line2.setLine((rectMiddleX+scale*136),(rectMiddleY+scale*82),(rectMiddleX+scale*133),(rectMiddleY+scale*82));
    line3.setLine((rectMiddleX+scale*133),(rectMiddleY+scale*82),(rectMiddleX+scale*133),(rectMiddleY-scale*74));
    lines = {line1, line2, line3};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX+scale*78),(rectMiddleY-scale*77),(rectMiddleX+scale*78),(rectMiddleY-scale*131));
    line2.setLine((rectMiddleX+scale*78),(rectMiddleY-scale*131),(rectMiddleX+scale*81),(rectMiddleY-scale*131));
    line3.setLine((rectMiddleX+scale*81),(rectMiddleY-scale*131),(rectMiddleX+scale*81),(rectMiddleY-scale*74));
    lines = {line1, line2, line3};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX+scale*136),(rectMiddleY+scale*82),(rectMiddleX+scale*190),(rectMiddleY+scale*82));
    line2.setLine((rectMiddleX+scale*190),(rectMiddleY+scale*82),(rectMiddleX+scale*190),(rectMiddleY+scale*79));
    line3.setLine((rectMiddleX+scale*190),(rectMiddleY+scale*79),(rectMiddleX+scale*136),(rectMiddleY+scale*79));
    lines = {line1, line2, line3};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX-scale*20),(rectMiddleY-scale*19),(rectMiddleX-scale*20),(rectMiddleY-scale*74));
    line2.setLine((rectMiddleX-scale*20),(rectMiddleY-scale*74),(rectMiddleX-scale*23),(rectMiddleY-scale*74));
    line3.setLine((rectMiddleX-scale*23),(rectMiddleY-scale*74),(rectMiddleX-scale*23),(rectMiddleY-scale*19));
    line4.setLine((rectMiddleX-scale*20),(rectMiddleY-scale*19),(rectMiddleX-scale*23),(rectMiddleY-scale*19));
    lines = {line1, line2, line3, line4};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX-scale*20),(rectMiddleY-scale*74),(rectMiddleX-scale*20),(rectMiddleY-scale*77));
    line2.setLine((rectMiddleX-scale*20),(rectMiddleY-scale*77),(rectMiddleX-scale*74),(rectMiddleY-scale*77));
    line3.setLine((rectMiddleX-scale*74),(rectMiddleY-scale*77),(rectMiddleX-scale*74),(rectMiddleY-scale*74));
    line4.setLine((rectMiddleX-scale*74),(rectMiddleY-scale*74),(rectMiddleX-scale*20),(rectMiddleY-scale*74));
    lines = {line1, line2, line3, line4};

    aPainter->drawLines(lines);
    lines.clear();

    line1.setLine((rectMiddleX-scale*74),(rectMiddleY-scale*74),(rectMiddleX-scale*74),(rectMiddleY-scale*19));
    line2.setLine((rectMiddleX-scale*74),(rectMiddleY-scale*19),(rectMiddleX-scale*71),(rectMiddleY-scale*19));
    line3.setLine((rectMiddleX-scale*71),(rectMiddleY-scale*19),(rectMiddleX-scale*71),(rectMiddleY-scale*74));
    line4.setLine((rectMiddleX-scale*71),(rectMiddleY-scale*19),(rectMiddleX-scale*74),(rectMiddleY-scale*19));
    lines = {line1, line2, line3, line4};

    aPainter->drawLines(lines);
    lines.clear();
}

void MapFrameWidget::paintMouseCoord(QRect& rectangle, QPainter *aPainter)
{
    if((mouseXPos >= (rectMiddleX-scale*288)) && (mouseYPos >= (rectMiddleY-scale*236)) && (mouseXPos <= (rectMiddleX+scale*288)) && (mouseYPos <= (rectMiddleY+scale*233))){
        aPainter->setFont(QFont());

        if((mouseXPos < 50) && (mouseYPos > 50)){
           aPainter->drawText(mouseXPos + 10, mouseYPos, "[" + QString::number(mouseToMapX/scale/100, 'f', 3) + "m, " + QString::number(((rectangle.height() - mouseToMapY)/scale/100), 'f', 3) + "m]");
        }
        else if((mouseXPos < 50) && (mouseYPos < 50)){
           aPainter->drawText(mouseXPos + 10, mouseYPos + 25, "[" + QString::number(mouseToMapX/scale/100, 'f', 3) + "m, " + QString::number(((rectangle.height() - mouseToMapY)/scale/100), 'f', 3) + "m]");
          }
        else if((mouseXPos < (rectangle.width() - 50)) && (mouseYPos > 50)){
           aPainter->drawText(mouseXPos - 45, mouseYPos - 5, "[" + QString::number(mouseToMapX/scale/100, 'f', 3) + "m, " + QString::number(((rectangle.height() - mouseToMapY)/scale/100), 'f', 3) + "m]");
          }
        else if((mouseXPos > (rectangle.width() - 50)) && (mouseYPos < 50)){
            aPainter->drawText(mouseXPos - 100, mouseYPos + 25, "[" + QString::number(mouseToMapX/scale/100, 'f', 3) + "m, " + QString::number(((rectangle.height() - mouseToMapY)/scale/100), 'f', 3) + "m]");
          }
        else if(mouseYPos < 50){
            aPainter->drawText(mouseXPos - 50, mouseYPos + 25, "[" + QString::number(mouseToMapX/scale/100, 'f', 3) + "m, " + QString::number(((rectangle.height() - mouseToMapY)/scale/100), 'f', 3) + "m]");
          }
        else if(mouseXPos > (rectangle.width() - 50)){
            aPainter->drawText(mouseXPos - 100, mouseYPos, "[" + QString::number(mouseToMapX/scale/100, 'f', 3) + "m, " + QString::number(((rectangle.height() - mouseToMapY)/scale/100), 'f', 3) + "m]");
          }
    }
}

bool MapFrameWidget::isGoalVectorEmpty()
{
    if(points.empty()){
        return true;
    }
    return false;
}

bool MapFrameWidget::toggleMap()
{
    if(showMap){
        showMap = false;
    }
    else{
        showMap = true;
    }
    return showMap;
}

bool MapFrameWidget::toggleMouse()
{
    if(posMouseTrack){
        posMouseTrack = false;
    }
    else{
        posMouseTrack = true;
    }
    return posMouseTrack;
}

void MapFrameWidget::clearVectors()
{
    robotReplayPos.clear();
    lidarReplayPos.clear();
    robotTrajectory.clear();
    missionReplayPoints.clear();
}

bool MapFrameWidget::getShowDisconnectWarning() const
{
    return showDisconnectWarning;
}

void MapFrameWidget::pushBackTajcPoint(double x, double y)
{
    trajcPoints.push_back(QPoint(x,y));
}

void MapFrameWidget::setShowDisconnectWarning(bool newShowDisconnectWarning)
{
    showDisconnectWarning = newShowDisconnectWarning;
}

bool MapFrameWidget::getReplayFinished() const
{
    return replayFinished;
}

void MapFrameWidget::setReplayFinished(bool newReplayFinished)
{
    replayFinished = newReplayFinished;
}

void MapFrameWidget::setReplayIndex(int newReplayIndex)
{
    replayIndex = newReplayIndex;
}

void MapFrameWidget::setIsSimulation(bool newIsSimulation)
{
    isSimulation = newIsSimulation;
}

void MapFrameWidget::setRobotInitialized(bool newRobotInitialized)
{
    robotInitialized = newRobotInitialized;
}

double MapFrameWidget::getRealTheta() const
{
    return realTheta;
}

bool MapFrameWidget::getShowReplayWarning() const
{
    return showReplayWarning;
}


double MapFrameWidget::getDistanceToFirstPoint()
{
    if(!points.empty()){
        return std::sqrt((points[0].x() - robotPosition.x())^2 + (points[0].y() - robotPosition.y())^2);
    }
    return -1.0;
}

void MapFrameWidget::setScale(float newScale)
{
    scale = newScale;
}


void MapFrameWidget::setNumber2(int newNumber2)
{
    number2 = newNumber2;
}

std::vector<RobotGoal>* MapFrameWidget::getRobotGoals()
{
    return &points;
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

void MapFrameWidget::setCanTriggerEvent(bool state){
    canTriggerEvents = state;
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

void MapFrameWidget::setShowReplayWarning(bool newShowReplayWarning)
{
    showReplayWarning = newShowReplayWarning;
}

