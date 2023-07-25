#include "cameraFrameWidget.h"
#include <iostream>
#include <QWidget>
#include <QtGui>
#include <QFrame>


#define PI 3.14159

/**
 * @brief CameraFrameWidget::CameraFrameWidget
 * @param parent
 */
CameraFrameWidget::CameraFrameWidget(QWidget *parent): QWidget(parent)
{
    actIndex=-1;
    offset = 10;
    canReplay = false;
    dispOrangeWarning = false;
    dispYellowWarning = false;
    dispRobotStopped = false;

    rectangleWidthPx = 0;
    rectangleHeightPx = 0;
    alfa1 = 24.0;
    alfa2 = 32.0;

    pointColor = QColor(20,255,20);
    pointType = 1;

    updateCameraPicture = 0;
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    imageOnline = QImage(":/resource/Alarmy/online.png");
}

CameraFrameWidget::~CameraFrameWidget(){

}

void CameraFrameWidget::paintEvent(QPaintEvent*){
    QPainter painter(this);
    painter.setBrush(Qt::black);

    screen = this->window()->windowHandle()->screen();
    this->setFixedWidth(screen->geometry().width()*scaleFactorWidth/1920);
    this->setFixedHeight(screen->geometry().height()*scaleFactorHeight/1080);

    QRect rectangle(offset/2, offset/2, this->width() - offset, this->height() - offset);

    scale = (this->width()-15)/574.0f;

    rectangleHeightPx = rectangle.height();
    rectangleWidthPx = rectangle.width();

    painter.drawRect(rectangle);

    if(updateCameraPicture == 1){

        if(robotOnline){
              if(frame->empty()){
                  updateCameraPicture = 0;
                  return;
              }
              image = QImage((uchar*)frame[actIndex].data, frame[actIndex].cols, frame[actIndex].rows, frame[actIndex].step, QImage::Format_RGB888);
              painter.drawImage(rectangle,image.rgbSwapped());;
              setSpeedWidget(&painter, rectangle.width());
              setBatteryWidget(&painter, rectangle.width());

              if(robotOnline){
                 painter.drawImage(QPoint(10,10), imageOnline.scaled(50, 50, Qt::KeepAspectRatio));
              }
              if(dispOrangeWarning){
                 dispYellowWarning = false;
                 imageDistanceWarn = QImage(":/resource/Alarmy/warning_orange.png");
                 painter.drawImage(QPoint(60,10), imageDistanceWarn.scaled(100,100, Qt::KeepAspectRatio));
              }
              else if(dispYellowWarning){
                 dispOrangeWarning = false;
                 imageDistanceWarn = QImage(":/resource/Alarmy/warning_yellow.png");
                 painter.drawImage(QPoint(60,10), imageDistanceWarn.scaled(100,100, Qt::KeepAspectRatio));
              }
              else if(dispRobotStopped){
                 dispRobotStopped = false;
                 imageDistanceWarn = QImage(":/resource/Alarmy/warning_red.png");
                 painter.drawImage(QPoint(60,10), imageDistanceWarn.scaled(100,100, Qt::KeepAspectRatio));
              }
        }
        else if(canReplay){
            if(replayFrame.empty()){
                updateCameraPicture = 0;
                return;
            }
            image = QImage((uchar*)replayFrame.data, replayFrame.cols, replayFrame.rows, replayFrame.step, QImage::Format_RGB888);
            painter.drawImage(rectangle,image.rgbSwapped());;
        }
    }
}

void CameraFrameWidget::setScaleFactorHeight(int newScaleFactorHeight)
{
    scaleFactorHeight = newScaleFactorHeight;
}

void CameraFrameWidget::setScaleFactorWidth(int newScaleFactorWidth)
{
    scaleFactorWidth = newScaleFactorWidth;
}

void CameraFrameWidget::mousePressEvent(QMouseEvent *event){
    if(canPlacePoints && points->size() < 10 && robotOnline){
        if(event->y() >= rectangleHeightPx/2)
            alfa1 = 24*(event->y() - rectangleHeightPx/2)/(rectangleHeightPx/2.0);
        else
            alfa1 = -24*(rectangleHeightPx/2 - event->y())/(rectangleHeightPx/2.0);

        if(event->x() >= rectangleWidthPx/2)
            alfa2 = -32*(event->x() - rectangleWidthPx/2)/(rectangleWidthPx/2.0);
        else
            alfa2 = 32*(rectangleWidthPx/2 - event->x())/(rectangleWidthPx/2.0);

        if((alfa1 > 0 && alfa1 < 24) && (alfa2 < 32 && alfa2 > -32)){
            d1 = cameraV/std::tan(alfa1*PI/180)*0.8*scale;
            d2 = d1/std::cos(alfa2*PI/180);
            points->insert(points->begin(), RobotGoal(robotX + d2*cos(robotTheta+alfa2*PI/180), (robotY+8) - d2*sin(robotTheta+alfa2*PI/180) , pointType, pointColor));
        }
    }
}

void CameraFrameWidget::setPointType(int newPointType)
{
    pointType = newPointType;
}

void CameraFrameWidget::setPointColor(const QColor &newPointColor)
{
    pointColor = newPointColor;
}

bool CameraFrameWidget::getCanPlacePoints() const
{
    return canPlacePoints;
}

void CameraFrameWidget::setCanPlacePoints(bool newCanPlacePoints)
{
    canPlacePoints = newCanPlacePoints;
}

void CameraFrameWidget::setRobotParams(double &x, double &y, double &theta)
{
    robotX = x;
    robotY = y;
    robotTheta = theta;
}

void CameraFrameWidget::setSpeedWidget(QPainter* aPainter, int frameWidth)
{
    if(v == 0.0 || v < 0.0){
        aPainter->drawText(QPoint(frameWidth-130,75),"");
        return;
    }

    if(v > 0.0 && v <= ((tempSpeed/100)*20)){
        speedImage = QImage(":/resource/speed_indicator/speed1.png");
        aPainter->drawImage(QPoint(frameWidth-120,10), speedImage.scaled(50,50, Qt::KeepAspectRatio));

    }
    else if(v > ((tempSpeed/100)*20) && v <= ((tempSpeed/100)*40)){
        speedImage = QImage(":/resource/speed_indicator/speed2.png");
        aPainter->drawImage(QPoint(frameWidth-120,10), speedImage.scaled(50,50, Qt::KeepAspectRatio));

    }
    else if(v > ((tempSpeed/100)*40) && v <= ((tempSpeed/100)*60)){
        speedImage = QImage(":/resource/speed_indicator/speed3.png");
        aPainter->drawImage(QPoint(frameWidth-120,10), speedImage.scaled(50,50, Qt::KeepAspectRatio));

    }
    else if(v > ((tempSpeed/100)*60) && v <= ((tempSpeed/100)*80)){
        speedImage = QImage(":/resource/speed_indicator/speed4.png");
        aPainter->drawImage(QPoint(frameWidth-120,10), speedImage.scaled(50,50, Qt::KeepAspectRatio));

    }
    else{
        speedImage = QImage(":/resource/speed_indicator/speed5.png");
        aPainter->drawImage(QPoint(frameWidth-120,10), speedImage.scaled(50,50, Qt::KeepAspectRatio));
    }

    aPainter->setFont(QFont("Segoe UI",10,450));
    aPainter->drawText(QPoint(frameWidth-145,75),QString::number(v, 'f', 2) + " mm/s");
}

void CameraFrameWidget::setBatteryWidget(QPainter* aPainter,int frameWidth)
{
    if(batteryPercantage == 200){
        batteryPercantage = 0;
        return;
    }

    batteryPercantage = (batteryLevel*100)/255;

    if((batteryLevel >= ((255.0/100.0)*80.0))){
        batteryImage = QImage(":/pages/battery5NEW.png");
        aPainter->drawImage(QPoint(frameWidth-60,15), batteryImage.scaled(60,60, Qt::KeepAspectRatio));

    }
    else if((batteryLevel < ((255.0/100.0)*80.0)) && (batteryLevel >= ((255.0/100.0)*60.0))){
        batteryImage = QImage(":/pages/battery4NEW.png");
        aPainter->drawImage(QPoint(frameWidth-60,15), batteryImage.scaled(60,60, Qt::KeepAspectRatio));

    }
    else if((batteryLevel < ((255.0/100.0)*60.0)) && (batteryLevel >= ((255.0/100.0)*40.0))){
        batteryImage = QImage(":/pages/battery3NEW.png");
        aPainter->drawImage(QPoint(frameWidth-60,15), batteryImage.scaled(60,60, Qt::KeepAspectRatio));

    }
    else if((batteryLevel < ((255.0/100.0)*40.0)) && (batteryLevel >= ((255.0/100.0)*20.0))){
        batteryImage = QImage(":/pages/battery2NEW.png");
        aPainter->drawImage(QPoint(frameWidth-60,15), batteryImage.scaled(60,60, Qt::KeepAspectRatio));

    }
    else if(batteryLevel > 0 && (batteryLevel < ((255.0/100.0)*20.0))){
        batteryImage = QImage(":/pages/battery1NEW.png");
        aPainter->drawImage(QPoint(frameWidth-60,15), batteryImage.scaled(60,60, Qt::KeepAspectRatio));

    }
    else if(batteryLevel == 0){
        batteryImage = QImage(":/pages/battery0NEW.png");
        aPainter->drawImage(QPoint(frameWidth-60,15), batteryImage.scaled(60,60, Qt::KeepAspectRatio));
    }

    aPainter->setFont(QFont("Segoe UI",10,450));
    aPainter->drawText(QPoint(frameWidth-40,35),QString::number(batteryPercantage) + "%");
}


void CameraFrameWidget::setPointVector(std::vector<RobotGoal>* goals)
{
    points = goals;
}

void CameraFrameWidget::setCanReplay(bool newCanReplay)
{
    canReplay = newCanReplay;
}

void CameraFrameWidget::setBatteryPercantage(unsigned short newBatteryPercantage)
{
    batteryPercantage = newBatteryPercantage;
}

bool CameraFrameWidget::getRobotOnline() const
{
    return robotOnline;
}


void CameraFrameWidget::setScale(float newScale)
{
    scale = newScale;
}

void CameraFrameWidget::setRobotOnline(bool newRobotOnline)
{
    robotOnline = newRobotOnline;
}

void CameraFrameWidget::setDispOrangeWarning(bool newDispOrangeWarning)
{
    dispOrangeWarning = newDispOrangeWarning;
}

void CameraFrameWidget::setRobotStoppedWarning(bool newWarning)
{
    dispRobotStopped = newWarning;
}

void CameraFrameWidget::setDispYellowWarning(bool newDispYellowWarning)
{
    dispYellowWarning = newDispYellowWarning;
}


unsigned short CameraFrameWidget::getBatteryPercantage() const
{
    return batteryPercantage;
}

void CameraFrameWidget::resetWarnings()
{
    dispOrangeWarning = dispYellowWarning = false;
}

void CameraFrameWidget::setBatteryLevel(const unsigned char newBatteryLevel)
{
    batteryLevel = newBatteryLevel;
}

void CameraFrameWidget::setV(double newV)
{
    v = newV;
}

void CameraFrameWidget::setTempSpeed(double newTempSpeed)
{
    tempSpeed = newTempSpeed;
}

cv::Mat CameraFrameWidget::getCameraFrame(){
    return this->frame[0];
}
