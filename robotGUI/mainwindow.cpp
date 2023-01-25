#include "mainwindow.h"
#include "alarmdialog.h"
#include "qtimer.h"
#include "ui_mainwindow.h"
#include <qgridlayout.h>
#include <iostream>
#include "cameraFrameWidget.h"
#include <string>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {

    std::cout << "Main window opened" << std::endl;

    missionLoaded = false;
    missionRunning = false;

    robotConnected = false;
    robotRunning = false;

    recordMission = false;

    dataCounter = index = multiplicator = 0;
    tempSpeed = 500;

    accInterval = 1000;
    accTimer = new QTimer(this);
    connect(accTimer, SIGNAL(timeout()), this, SLOT(callbackAcc()));

    breakInterval = 300;
    breakTimer = new QTimer(this);
    connect(breakTimer, SIGNAL(timeout()), this, SLOT(callbackBreak()));

    ui->setupUi(this);

    ui->addressField->setMaxLength(20);

    cameraFrame = new CameraFrameWidget();
    ui->cameraMapLayout->addWidget(cameraFrame, 0, 1);

    mapFrame = new MapFrameWidget();
    ui->cameraMapLayout->addWidget(mapFrame, 0, 2);
}

MainWindow::~MainWindow()
{
    delete cameraFrame;
    delete mapFrame;
    delete robot;
    delete accTimer;
    delete breakTimer;
    delete ui;
}



void MainWindow::callbackAcc(){
    robot->setMultiBreak(5);
    if(dir > dirOld){
        if(robotForwardSpeed < tempSpeed){
           double temp = robot->rampPosFunction(1,tempSpeed,5);
           std::cout << temp << std::endl;
           robotForwardSpeed = temp;
           //robotRotationalSpeed = temp;
        }
        else{
           robot->setMultiAcc(0);
        }
    }
    else if(dir < dirOld){
        if((std::abs(robotForwardSpeed)) < tempSpeed){
           robotForwardSpeed = robot->rampPosFunction(-1,tempSpeed,5);;
        }
        else{
           robot->setMultiAcc(0);
        }
    }
    else{
        robot->setTranslationSpeed(0);
    }
}

void MainWindow::callbackBreak(){
    std::cout << "Hello form breaking calback!" << std::endl;
    robot->setMultiAcc(0);
    if(std::abs(robotForwardSpeed) > 0){
        double temp = robot->rampNegFunction(robotForwardSpeed,5);
        robotForwardSpeed = temp;
    }
    else if(std::abs(robotRotationalSpeed) > 0){
        double temp = robot->rampNegFunction(robotForwardSpeed,5);
        robotRotationalSpeed = temp;
    }
    else{
        robotRunning = false;
        robot->setMultiBreak(5);
        std::cout << "Robot is not moving" << std::endl;
    }
}

int MainWindow::processLidar(LaserMeasurement laserData){
    std::memcpy(&(mapFrame->copyOfLaserData), &laserData, sizeof(LaserMeasurement));
    mapFrame->updateLaserPicture = 1;
    mapFrame->update();

    return 0;
}

int MainWindow::processRobot(TKobukiData robotData){
    //printf("Battery: %lf", robotData.Battery);
    //std::cout << "Gyro angle: " << robotData.GyroAngle << std::endl;
    if(robotForwardSpeed==0 && robotRotationalSpeed !=0)
        robot->setRotationSpeed(robotRotationalSpeed);
    else if(robotForwardSpeed!=0 && robotRotationalSpeed==0){
        robot->setTranslationSpeed(robotForwardSpeed);
    }
    else if((robotForwardSpeed!=0 && robotRotationalSpeed!=0))
        robot->setArcSpeed(robotForwardSpeed,robotForwardSpeed/robotRotationalSpeed);
    else
        robot->setTranslationSpeed(0);

    if(dataCounter%5)
    {
        emit uiValuesChanged(robotdata.EncoderLeft,11,12);
    }
    dataCounter++;

    return 0;
}

int MainWindow::processCamera(cv::Mat cameraData){
    cameraData.copyTo(cameraFrame->frame[(cameraFrame->actIndex+1)%3]);
    cameraFrame->actIndex=(cameraFrame->actIndex+1)%3;
    cameraFrame->updateCameraPicture=1;
    cameraFrame->update();

    return 0;
}

void MainWindow::on_actionGo_Offline_triggered()
{

}


void MainWindow::on_actionGo_Online_triggered()
{
    std::cout << "Hello from go online action!" << std::endl;
    connectRobotUiSetup();
}


void MainWindow::on_actionExit_triggered()
{
    std::cout << "Hello from exit!" << std::endl;
    QApplication::quit();
}





void MainWindow::on_actionAlarms_triggered()
{
    std::cout << "Hello from alarms!" << std::endl;
    alarmHelpWindow = new AlarmDialog(this);
    alarmHelpWindow->setWindowTitle("Help");
    alarmHelpWindow->setAttribute(Qt::WA_DeleteOnClose);
    alarmHelpWindow->show();
}





void MainWindow::on_startButton_clicked()
{
    if(robotConnected && !robotRunning){
        ui->startButton->setStyleSheet("#startButton{"
                                        "background-color: silver;"
                                        "border-style:outset;"
                                        "border-radius: 10px;"
                                        "border-color:black;"
                                        "border-width:4px;"
                                        "padding: 5px;"
                                        "image: url(:/resource/stop_start/stop.png);}"
                                        );
        robotRunning = true;
        if(breakTimer->isActive()){
           breakTimer->stop();
        }
        if(!mapFrame->points.empty()){
            if(mapFrame->points[index].y() <= mapFrame->middle.y()){
                dir = 1;
            }
            else if(mapFrame->points[index].y() > mapFrame->middle.y()){
                dir = -1;
            }
            accTimer->start(accInterval);
        }
    }
    else if(robotConnected && robotRunning){
         ui->startButton->setStyleSheet("#startButton{"
                                        "background-color: silver"
                                        ";border-style:outset;"
                                        "border-radius: 10px;"
                                        "border-color:black;"
                                        "border-width:4px;"
                                        "padding: 5px;"
                                        "image: url(:/resource/stop_start/start.png);}"
                                        );
        if(accTimer->isActive()){
           accTimer->stop();
        }
        breakTimer->start(breakInterval);
    }
}





void MainWindow::on_startButton_pressed()
{
    if(robotConnected && !robotRunning){
        ui->startButton->setStyleSheet("#startButton{"
                                       "background-color: silver;"
                                       "border-style:outset;"
                                       "border-radius: 10px;"
                                       "border-color:black;"
                                       "border-width:4px;"
                                       "padding: 5px;"
                                       "image: url(:/resource/stop_start/start_clicked.png);}"
                                       );
    }

    else if(robotConnected && robotRunning){
        ui->startButton->setStyleSheet("#startButton{"
                                       "background-color: silver;"
                                       "border-style:outset;"
                                       "border-radius: 10px;"
                                       "border-color:black;"
                                       "border-width:4px;"
                                       "padding: 5px;"
                                       "image: url(:/resource/stop_start/stop_clicked.png);}"
                                       );
    }
    else{
        std::cout << "Robot is not connected!" << std::endl;
    }
}


void MainWindow::setUiValues(double robotX, double robotY, double robotFi){
    ui->xValue->setText(QString::number(robotX));
    ui->yValue->setText(QString::number(robotY));
    ui->rotValue->setText(QString::number(robotFi));
}




void MainWindow::on_connectToRobotButton_clicked()
{
    connectRobotUiSetup();
    if(!ipAddress.empty()){
        robotForwardSpeed = 0;
        robotRotationalSpeed = 0;

        robot = new Robot(ipAddress);
        robotConnected = true;

        //mapFrame->setCanTriggerEvent(true);

        robot->setLaserParameters(ipAddress,52999,5299,std::bind(&MainWindow::processLidar,this,std::placeholders::_1));
        robot->setRobotParameters(ipAddress,53000,5300,std::bind(&MainWindow::processRobot,this,std::placeholders::_1));
        robot->setCameraParameters("http://" + ipAddress + ":" + cameraPort + "/stream.mjpg",std::bind(&MainWindow::processCamera,this,std::placeholders::_1));
        connect(this,SIGNAL(uiValuesChanged(double,double,double)),this,SLOT(setUiValues(double,double,double)));
        robot->robotStart();
    }
}





void MainWindow::connectRobotUiSetup(){
    if(getIpAddress()){
        std::cout << "Ip address loaded" << std::endl;

        if(setBatteryLevelWidget()){
            std::cout  << "Success!" << std::endl;
        }
    }

    else{
        std::cout << "No ip address loaded!" << std::endl;
    }
}





void MainWindow::on_replayMissionButton_clicked()
{
    std::cout << "Mission before:  " << missionRunning << std::endl;

    if(robotConnected){
        if(!missionLoaded){
            if(!missionRunning){
                missionRunning = true;
                std::cout << "Mission: " << missionRunning << std::endl;
                ui->replayMissionButton->setStyleSheet("#replayMissionButton{background-color: "
                                                       "silver;border-style:outset;border-radius: "
                                                       "10px;border-color:black;border-width:4px;padding: "
                                                        "5px;image:url(:/resource/stop_start/stop_play.png)}"
                                                       );

               }
            else{
                missionRunning = false;
                std::cout << "Mission: " << missionRunning << std::endl;
                ui->replayMissionButton->setStyleSheet("#replayMissionButton{background-color: "
                                                       "silver;border-style:outset;border-radius: "
                                                       "10px;border-color:black;border-width:4px;padding: "
                                                        "5px;image:url(:/resource/stop_start/play.png)}"
                                                       );

               }
            }
    }
}




bool MainWindow::setBatteryLevelWidget(){
    if(robotConnected){
        if(batteryLevel > 80.0){
            ui->batteryWidget->setStyleSheet("background-color: silver; "
                                             "border-style:outset; "
                                             "border-radius: 10px;"
                                             "border-color:black;"
                                             "border-width:4px;"
                                             "min-width: 10em;"
                                             "padding: 5px;"
                                             "image:url(:/resource/Baterka/battery5.png)"
                                             );
        }
        else if(batteryLevel > 60.0){
            ui->batteryWidget->setStyleSheet("background-color: silver; "
                                             "border-style:outset; "
                                             "border-radius: 10px;"
                                             "border-color:black;"
                                             "border-width:4px;"
                                             "min-width: 10em;"
                                             "padding: 5px;"
                                             "image:url(:/resource/Baterka/battery4.png)"
                                             );
        }
        else if(batteryLevel > 40.0){
            ui->batteryWidget->setStyleSheet("background-color: silver; "
                                             "border-style:outset; "
                                             "border-radius: 10px;"
                                             "border-color:black;"
                                             "border-width:4px;"
                                             "min-width: 10em;"
                                             "padding: 5px;"
                                             "image:url(:/resource/Baterka/battery3.png)"
                                             );
        }
        else if(batteryLevel > 20.0){
            ui->batteryWidget->setStyleSheet("background-color: silver; "
                                             "border-style:outset; "
                                             "border-radius: 10px;"
                                             "border-color:black;"
                                             "border-width:4px;"
                                             "min-width: 10em;"
                                             "padding: 5px;"
                                             "image:url(:/resource/Baterka/battery2.png)");
        }
        else if(batteryLevel > 0.0){
            ui->batteryWidget->setStyleSheet("background-color: silver; "
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
            ui->batteryWidget->setStyleSheet("background-color: silver; "
                                             "border-style:outset; "
                                             "border-radius: 10px;"
                                             "border-color:black;"
                                             "border-width:4px;"
                                             "min-width: 10em;"
                                             "padding: 5px;"
                                             "image:url(:/resource/Baterka/battery0.png)"
                                             );
        }
        return 1;
    }
    return 0;
}




bool MainWindow::getIpAddress()
{
    if(!ui->addressField->text().isEmpty() && ipAddress.compare(ui->addressField->text().toStdString())){
        ipAddress = ui->addressField->text().toStdString();
        std::cout << ipAddress << std::endl;
        return 1;
    }

    return 0;
}




void MainWindow::on_checkBox_stateChanged(int arg1)
{
    std::cout << "Hello from checkbox" << std::endl;
    if(!recordMission){
        recordMission = true;
    }
    else{
        recordMission = false;
    }
}


void MainWindow::on_pushButton_clicked()
{

}

