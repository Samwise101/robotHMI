#include "cameraFrameWidget.h"
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
    imageWarnRed = QImage(":/resource/Alarmy/warning_red.png");
    imageWarnYellow = QImage(":/resource/Alarmy/warning_yellow.png");
    imageOnline = QImage(":/resource/Alarmy/online.png");
}

CameraFrameWidget::~CameraFrameWidget(){
    if(robotOnline){
        delete batteryFrame;
        delete speedFrame;
    }
}

void CameraFrameWidget::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.setBrush(Qt::black);
    QRect rectangle(offset/2, offset/2, this->size().width() - offset, this->size().height() - offset);
    painter.drawRect(rectangle);


    if(updateCameraPicture == 1){

        if(!missionLoaded){
              image = QImage((uchar*)frame[actIndex].data, frame[actIndex].cols, frame[actIndex].rows, frame[actIndex].step, QImage::Format_RGB888);
              painter.drawImage(rectangle,image.rgbSwapped());;
              setSpeedWidget();
              setBatteryWidget();

              if(robotOnline){
                 painter.drawImage(QPoint(10,10), imageOnline.scaled(50, 50, Qt::KeepAspectRatio));
              }
              if(dispRedWarning){
                 dispYellowWarning = false;
                 painter.drawImage(QPoint(60,10), imageWarnRed.scaled(100,100, Qt::KeepAspectRatio));
              }
              else if(dispYellowWarning){
                 dispRedWarning = false;
                 painter.drawImage(QPoint(60,10), imageWarnYellow.scaled(100,100, Qt::KeepAspectRatio));
              }
        }
        else{
            //cv::resize(replayFrame, dest, cv::Size(rectangle.width(), rectangle.height()));
            image = QImage((uchar*)replayFrame.data, replayFrame.cols, replayFrame.rows, replayFrame.step, QImage::Format_RGB888);
            painter.drawImage(rectangle,image.rgbSwapped());;
        }
    }
}

void CameraFrameWidget::setMissionLoaded(bool newMissionLoaded)
{
    missionLoaded = newMissionLoaded;
}

void CameraFrameWidget::setCap(const cv::VideoCapture &newCap)
{
    cap = newCap;
}

void CameraFrameWidget::setSpeedWidget()
{
    if(v == 0.0 || v < 0.0){
        speedFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         );
    }
    else if(v > 0.0 && v <= ((tempSpeed/100)*20)){
        speedFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/speed_indicator/speed1.png)"
                                         );
    }
    else if(v > ((tempSpeed/100)*20) && v <= ((tempSpeed/100)*40)){
        speedFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/speed_indicator/speed2.png)"
                                         );
    }
    else if(v > ((tempSpeed/100)*40) && v <= ((tempSpeed/100)*60)){
        speedFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/speed_indicator/speed3.png)"
                                         );
    }
    else if(v > ((tempSpeed/100)*60) && v <= ((tempSpeed/100)*80)){
        speedFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/speed_indicator/speed4.png)"
                                         );
    }
    else{
        speedFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/speed_indicator/speed5.png)"
                                         );
    }
}

void CameraFrameWidget::setBatteryWidget()
{
    batteryPercantage = (batteryLevel*100)/255;

    if((batteryLevel >= ((255.0/100.0)*80.0))){
        batteryFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/Baterka/battery5.png)"
                                         );
    }
    else if((batteryLevel < ((255.0/100.0)*80.0)) && (batteryLevel >= ((255.0/100.0)*60.0))){
        batteryFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/Baterka/battery4.png)"
                                         );
    }
    else if((batteryLevel < ((255.0/100.0)*60.0)) && (batteryLevel >= ((255.0/100.0)*40.0))){
        batteryFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/Baterka/battery3.png)"
                                         );
    }
    else if((batteryLevel < ((255.0/100.0)*40.0)) && (batteryLevel >= ((255.0/100.0)*20.0))){
        batteryFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/Baterka/battery2.png)");
    }
    else if(batteryLevel > 0 && (batteryLevel < ((255.0/100.0)*20.0))){
        batteryFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/Baterka/battery1.png)"
                                         );
    }
    else{
        batteryFrame->setStyleSheet("background-color: silver; "
                                         "border-style:outset; "
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "min-width: 10em;"
                                         "padding: 5px;"
                                         "image:url(:/resource/Baterka/battery0.png)"
                                         );
    }
}


void CameraFrameWidget::setScale(float newScale)
{
    scale = newScale;
}

void CameraFrameWidget::setRobotOnline(bool newRobotOnline)
{
    robotOnline = newRobotOnline;
}

void CameraFrameWidget::setDispRedWarning(bool newDispRedWarning)
{
    dispRedWarning = newDispRedWarning;
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
    dispRedWarning = dispYellowWarning = false;
}

void CameraFrameWidget::setBatteryLevel(const unsigned char newBatteryLevel)
{
    batteryLevel = newBatteryLevel;
    //std::cout << "Battery level = " << batteryLevel << std::endl;
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
