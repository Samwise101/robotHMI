#include "mainwindow.h"
#include "alarmdialog.h"
#include "qtimer.h"
#include "ui_mainwindow.h"
#include <qgridlayout.h>
#include <iostream>
#include "cameraFrameWidget.h"
#include <string>

#define PI 3.14159

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {

    std::cout << "Main window opened" << std::endl;

    missionLoaded = false;
    missionRunning = false;

    robotConnected = false;
    robotRunning = false;

    recordMission = false;

    dataCounter = 0;

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
    delete ui;
}

int MainWindow::processLidar(LaserMeasurement laserData){
    std::memcpy(&(mapFrame->copyOfLaserData), &laserData, sizeof(LaserMeasurement));
    mapFrame->updateLaserPicture = 1;
    mapFrame->update();
    return 0;
}

int MainWindow::processRobot(TKobukiData robotData){

    if(!robot->getInitilize()){
        robot->setRobotPose(mapFrame->robotPosition.x(), mapFrame->robotPosition.y(), 0);
        robot->setInitilize(true);
    }

    robot->robotOdometry(robotData);

    cameraFrame->setBatteryLevel(robotData.Battery);
    cameraFrame->setV(v);

    ui->speedLabel->setText(QString::number(v) + " mm/s");
    ui->batteryLabel->setText(QString::number(cameraFrame->getBatteryPercantage()) + " %");

    if(!robot->getAtGoal()){
        if(!robotRunning || mapFrame->isGoalVectorEmpty()){

            if(omega > 0.0 || omega < 0.0){
                omega = robot->orientationRegulator(0, 0, false);
                robotRotationalSpeed = omega;
            }
            if(v > 0.0){
                v = robot->regulateForwardSpeed(0, 0, false, mapFrame->getGoalType());
                robotForwardSpeed = v;
            }
        }
        else if(robotRunning && !mapFrame->isGoalVectorEmpty()){

            if(mapFrame->getShortestDistanceLidar() <= 350.0){
               cameraFrame->setDispRedWarning(true);
            }
            else if(mapFrame->getShortestDistanceLidar() <= 450.0){
                cameraFrame->setDispYellowWarning(true);
            }

            if(mapFrame->getShortestDistanceLidar() < 450.0 &&
              ((mapFrame->getShortestDistanceLidarAngle() >= 3*PI/2.0 && mapFrame->getShortestDistanceLidarAngle() <= 2.0*PI) ||
               (mapFrame->getShortestDistanceLidarAngle() >= 0.0 && mapFrame->getShortestDistanceLidarAngle() <= PI/2.0))
               && robot->getDistanceToGoal(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition()) > 350.0){

                v = robot->regulateForwardSpeed(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition(), robotRunning, mapFrame->getGoalType());
                omega = robot->avoidObstacleRegulator(mapFrame->getShortestDistanceLidarAngle());
            }
            else{
                cameraFrame->resetWarnings();
                omega = robot->orientationRegulator(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition(), robotRunning);
                v = robot->regulateForwardSpeed(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition(), robotRunning, mapFrame->getGoalType());
            }

            robotRotationalSpeed = omega;
            robotForwardSpeed = v;


            if(mapFrame->getGoalType() == 1){
                if(robot->getDistanceToGoal(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition()) < 100.0){
                   robot->setAtGoal(true);
                }
            }
            else{
                if(v == 0.0 && omega == 0.0){
                    goalAngle = robot->getTheta() + 2*PI;
                    robot->setAtGoal(true);
                }
            }
        }
    }
    else{
        if(!mapFrame->isGoalVectorEmpty()){
            if(mapFrame->getGoalType() == 2){
                omega = robot->robotFullTurn(goalAngle);
                if(omega > -0.15 && omega < 0.15){
                    mapFrame->removeLastPoint();
                    robot->setAtGoal(false);
                }
                robotRotationalSpeed = omega;
            }
            else if(mapFrame->getGoalType() == 3){
                this_thread::sleep_for(5000ms);
                mapFrame->removeLastPoint();
                robot->setAtGoal(false);
            }
            else{
                mapFrame->removeLastPoint();
                robot->setAtGoal(false);
            }
        }
    }

    mapFrame->updateRobotValuesForGUI(robot->getX(), robot->getY(), robot->getTheta());

    //std::cout << "v=" << v << ", w=" << omega << std::endl;

    if((robotForwardSpeed < 1.0) && robotRotationalSpeed != 0.0){
        std::cout << "Rotation!" << std::endl;
        robot->setRotationSpeed(robotRotationalSpeed);
    }
    else if(robotForwardSpeed != 0.0 && (robotRotationalSpeed > -0.1 && robotRotationalSpeed < 0.1)){
        std::cout << "Translation!" << std::endl;
        robot->setTranslationSpeed(robotForwardSpeed);
    }
    else if((robotForwardSpeed != 0.0 && robotRotationalSpeed != 0.0)){
        std::cout << "Arc!" << std::endl;
        robot->setArcSpeed(robotForwardSpeed,robotForwardSpeed/robotRotationalSpeed);
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
    connectRobotUiSetup();
    if(!setupConnectionToRobot()){
        std::cout << "Something went wrong, can't connect to robot!" << std::endl;
    }
    else{
        std::cout << "Successfully connected to the robot!" << std::endl;
    }
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
        robotRunning = false;
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


void MainWindow::on_connectToRobotButton_clicked()
{
    connectRobotUiSetup();
    if(!setupConnectionToRobot()){
        std::cout << "Something went wrong, can't connect to robot!" << std::endl;
    }
    else{
        std::cout << "Successfully connected to the robot!" << std::endl;
    }

}

bool MainWindow::setupConnectionToRobot(){
    if(!ipAddress.empty()){
        robotForwardSpeed = 0;
        robotRotationalSpeed = 0;

        std::cout << "Connected!" << std::endl;
        robot = new Robot(ipAddress);
        robotConnected = true;

        //mapFrame->setCanTriggerEvent(true);

        robot->setLaserParameters(ipAddress,52999,5299,std::bind(&MainWindow::processLidar,this,std::placeholders::_1));
        robot->setRobotParameters(ipAddress,53000,5300,std::bind(&MainWindow::processRobot,this,std::placeholders::_1));
        robot->setCameraParameters("http://" + ipAddress + ":" + cameraPort + "/stream.mjpg",std::bind(&MainWindow::processCamera,this,std::placeholders::_1));
        robot->robotStart();

        cameraFrame->setTempSpeed(robot->getTempSpeed());
        cameraFrame->speedFrame = ui->speedWidget;
        cameraFrame->batteryFrame = ui->batteryWidget;
        cameraFrame->setRobotOnline(true);

        return true;
    }
    else{
        return false;
    }

}





void MainWindow::connectRobotUiSetup(){
    if(getIpAddress()){
        std::cout << "Ip address loaded" << std::endl;

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
    if(arg1 && cameraFrame->updateCameraPicture == 1){
        recordMission = true;
        //int frameWidth = cameraFrame->image->
        //int frameHeight =
    }
    else{
        recordMission = false;
    }
}


void MainWindow::on_zmazGoal_clicked()
{
    /*
  if(!mapFrame->isGoalVectorEmpty()){
     std::cout << "Hello:" << mapFrame->getGoalVectorSize()  << std::endl;
     if(mapFrame->removeLastPoint())
        std::cout << "Success:" << mapFrame->getGoalVectorSize()  << std::endl;
  }*/
  mapFrame->removeAllPoints();
}

void MainWindow::on_zmenTypBoduButton_clicked()
{
    goalIndex++;
    if(goalIndex > 4){
        goalIndex = 1;
    }

    if(goalIndex%4 == 1){
        ui->zmenTypBoduButton->setText("Prejazdový\n bod");
        mapFrame->setPointColor(Qt::yellow);
        mapFrame->setPointType(1);
    }
    else if(goalIndex%4 == 2){
        ui->zmenTypBoduButton->setText("Otočenie\n o 360°");
        mapFrame->setPointColor(Qt::darkMagenta);
        mapFrame->setPointType(2);
    }
    else if(goalIndex%4 == 3){
        ui->zmenTypBoduButton->setText("Čakaj\n 2 sekundy");
        mapFrame->setPointColor(Qt::cyan);
        mapFrame->setPointType(3);
    }
    else{
        ui->zmenTypBoduButton->setText("Cieľový\n bod");
        mapFrame->setPointColor(Qt::gray);
        mapFrame->setPointType(4);
    }
}

