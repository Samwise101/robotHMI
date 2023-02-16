#include "mainwindow.h"
#include "alarmdialog.h"
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
    switchIndex = 0;

    workerStarted = false;
    worker2Started = false;

    ui->setupUi(this);

    ui->addressField->setMaxLength(20);

    cameraFrame = new CameraFrameWidget();
    ui->cameraWidget->addWidget(cameraFrame, 0, 1);

    mapFrame = new MapFrameWidget();
    ui->mapWidgetFrame->addWidget(mapFrame, 0, 2);
}

MainWindow::~MainWindow()
{
    if(workerStarted && !isFinished && !missionLoaded){
        isFinished = true;
        std::cout << "camera recording thread finished\n";
        worker.join();
        delete video;
    }

    if(worker2Started && !isFinished2 && !missionLoaded){
        isFinished2 = true;
        std::cout << "map recording thread finished\n";
        worker2.join();
    }

    if(workerStarted && missionLoaded){
        std::cout << "camera replay thread finished\n";
        if(missionLoaded)
            missionLoaded = false;
        worker.join();
    }

    if(worker2Started && missionLoaded){
        std::cout << "map replay thread finished\n";
        worker2.join();
    }

    if(robotConnected)
        delete robot;

    delete cameraFrame;
    delete mapFrame;
    delete ui;
}

int MainWindow::processLidar(LaserMeasurement laserData){
    std::memcpy(&(mapFrame->copyOfLaserData), &laserData, sizeof(LaserMeasurement));
    mapFrame->updateLaserPicture = 1;
    mapFrame->update();
    return 0;
}

int MainWindow::processRobot(TKobukiData robotData){

    //std::cout << "Gyro angle = "<< robotData.GyroAngle/100 << std::endl; //<-32768, 32767>

    if(!robot->getInitilize()){
        /*int initialTheta =  robotData.GyroAngle/100;
        if(robotData.GyroAngle/100.0 < 0){
            initialTheta = 360 + initialTheta;
        }
        std::cout << "initialTheta=" << initialTheta << std::endl;*/
        robot->setRobotPose(mapFrame->robotPosition.x(), mapFrame->robotPosition.y(), 0 /*initialTheta*PI/180*/);
        robot->setInitilize(true);
    }

    robot->robotOdometry(robotData);

    cameraFrame->setBatteryLevel(robotData.Battery);
    cameraFrame->setV(v);

    ui->speedLabel->setText(QString::number(v) + " mm/s");
    ui->batteryLabel->setText(QString::number(cameraFrame->getBatteryPercantage()) + " %");

    if(!robot->getAtGoal()){
        if(!robotRunning || mapFrame->isGoalVectorEmpty() || (mapFrame->getShortestDistanceLidar() < 180.0)){
            //td::cout << "Shortest lidar distance: " << mapFrame->getShortestDistanceLidar() << std::endl;
            if(omega > 0.0 || omega < 0.0){
                omega = robot->orientationRegulator(0, 0, false);
                robotRotationalSpeed = omega;
            }
            else{
                robotRotationalSpeed = 0.0;
            }
            if(v > 0.0){
                v = robot->regulateForwardSpeed(0, 0, false, 0);
                robotForwardSpeed = v;
            }
            else{
                robotForwardSpeed = 0;
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
              ((mapFrame->getLidarAngle() >= 3*PI/2 && mapFrame->getLidarAngle() <= 2*PI) ||
               (mapFrame->getLidarAngle() >= 0.0 && mapFrame->getLidarAngle() <= PI/2))
               && robot->getDistanceToGoal(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition()) > 350.0){

                v = robot->regulateForwardSpeed(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition(), robotRunning, mapFrame->getGoalType());
                omega = robot->avoidObstacleRegulator(mapFrame->getShortestDistanceLidar(), mapFrame->getLidarAngle());
            }
            else{
                cameraFrame->resetWarnings();
                omega = robot->orientationRegulator(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition(), robotRunning);
                v = robot->regulateForwardSpeed(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition(), robotRunning, mapFrame->getGoalType());
            }

            //std::cout << "omega=" << omega << std::endl;
            //std::cout << "v=" << v << std::endl;

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
                this_thread::sleep_for(2000ms);
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

    if((robotForwardSpeed < 1.0) && robotRotationalSpeed != 0.0){
            //std::cout << "Rotation!" << std::endl;
            robot->setRotationSpeed(robotRotationalSpeed);
        }
    else if(robotForwardSpeed != 0.0 && (robotRotationalSpeed > -0.1 && robotRotationalSpeed < 0.1)){
            //std::cout << "Translation!" << std::endl;
            robot->setTranslationSpeed(robotForwardSpeed);
        }
    else if((robotForwardSpeed != 0.0 && robotRotationalSpeed != 0.0)){
            //std::cout << "Arc!" << std::endl;
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
        cameraFrame->setMissionLoaded(false);
        cameraFrame->setTempSpeed(robot->getTempSpeed());
        cameraFrame->speedFrame = ui->speedWidget;
        cameraFrame->batteryFrame = ui->batteryWidget;
    }

}

bool MainWindow::setupConnectionToRobot(){
    if(!ipAddress.empty()){
        v = 0.0;
        omega = 0.0;
        robotForwardSpeed = 0;
        robotRotationalSpeed = 0;

        if(workerStarted && missionLoaded){
            missionRunning = false;
            missionLoaded = false;
            workerStarted = false;
            worker.join();
        }

        if(worker2Started && missionLoaded){
            missionRunning = false;
            missionLoaded = false;
            worker2Started = false;
            worker2.join();
        }

        robot = new Robot(ipAddress);
        robotConnected = true;

        cameraFrame->setRobotOnline(true);
        cameraFrame->updateCameraPicture = 1;

        mapFrame->setRobotOnline(true);
        mapFrame->updateLaserPicture = 1;

        robot->setLaserParameters(ipAddress,52999,5299,std::bind(&MainWindow::processLidar,this,std::placeholders::_1));
        robot->setRobotParameters(ipAddress,53000,5300,std::bind(&MainWindow::processRobot,this,std::placeholders::_1));
        robot->setCameraParameters("http://" + ipAddress + ":" + cameraPort + "/stream.mjpg",std::bind(&MainWindow::processCamera,this,std::placeholders::_1));
        robot->robotStart();

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

void MainWindow::recordCamera()
{
    if(!missionLoaded){
        video->open("camera_1.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), robot->getFps(), cv::Size(mapFrame->imageWidth,mapFrame->imageHeight), true);

        while(!isFinished && video->isOpened()){
            frame = cameraFrame->getCameraFrame();
            cv::Mat dest;
            cv::resize(frame, dest, cv::Size(mapFrame->imageWidth,mapFrame->imageHeight));
            video->write(dest);
            //this_thread::sleep_for(10ms);
        }
        video->release();
    }
    else{
        cameraFrame->setMissionLoaded(true);
        cap.open(s1.toStdString());

        if(!cap.isOpened()){
            std::cout << "Could not open the video file" << std::endl;
        }

        while(missionLoaded){
            if(missionRunning){
                if(!cap.read(cameraFrame->replayFrame)){
                    cameraFrame->updateCameraPicture = 0;
                    break;
                }
                cameraFrame->updateCameraPicture=1;
                cameraFrame->update();
            }
        }
        cap.release();
    }
}

void MainWindow::recordMap()
{
    if(!missionLoaded){
        if(!mapFile.is_open()){
            mapFile.open("mapLog.txt", ios::out);
        }

        while(!isFinished2 && mapFile.is_open()){
            mapFrame->createFrameLog(timepassed2, mapFile);
            this_thread::sleep_for(200ms);
        }
        mapFile.close();
    }
    else{

    }
}



void MainWindow::on_loadMissionButton_clicked()
{
    if(workerStarted && missionLoaded){
        missionRunning = false;
        missionLoaded = false;
        workerStarted = false;
        worker.join();
    }

    if(worker2Started && missionLoaded){
        missionRunning = false;
        missionLoaded = false;
        worker2Started = false;
        worker2.join();
    }

    ui->replayMissionButton->setStyleSheet("#replayMissionButton{background-color: "
                                           "silver;border-style:outset;border-radius: "
                                           "10px;border-color:black;border-width:4px;padding: "
                                            "5px;image:url(:/resource/stop_start/play.png)}"
                                           );


    if(!robotConnected){
        missionLoaded = true;
        s1 = dialog.getOpenFileName(this, "Select a video file to open...", QDir::homePath());
        s2 = dialog.getOpenFileName(this, "Select a text file to open...", QDir::homePath());

        if(!workerStarted){
            workerStarted = true;
            std::function<void(void)> func =std::bind(&MainWindow::recordCamera, this);
            worker = std::thread(func);
        }
        if(!worker2Started){
            worker2Started = true;
            std::function<void(void)> func =std::bind(&MainWindow::recordMap, this);
            worker2 = std::thread(func);
        }
    }
}



void MainWindow::on_replayMissionButton_clicked()
{
    if(!robotConnected && missionLoaded){
        if(!missionRunning){
            missionRunning = true;
            ui->replayMissionButton->setStyleSheet("#replayMissionButton{background-color: "
                                                   "silver;border-style:outset;border-radius: "
                                                   "10px;border-color:black;border-width:4px;padding: "
                                                    "5px;image:url(:/resource/stop_start/stop_play.png)}"
                                                   );

            }
        else{
            missionRunning = false;
            ui->replayMissionButton->setStyleSheet("#replayMissionButton{background-color: "
                                                   "silver;border-style:outset;border-radius: "
                                                   "10px;border-color:black;border-width:4px;padding: "
                                                    "5px;image:url(:/resource/stop_start/play.png)}"
                                                   );
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
    std::cout << "Frame width="  << mapFrame->imageWidth << ", frame height=" <<  mapFrame->imageHeight << std::endl;
    if(arg1 && cameraFrame->updateCameraPicture == 1){
        if(robotConnected && !workerStarted && !missionLoaded){
           recordMission = true;
           workerStarted = true;
           if(!videoCreated){
                video = new cv::VideoWriter("camera_1.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), robot->getFps(), cv::Size(mapFrame->imageWidth,mapFrame->imageHeight), true);
                videoCreated = true;
           }
           std::function<void(void)> func =std::bind(&MainWindow::recordCamera, this);
           worker = std::thread(func);
        }
        if(robotConnected && !worker2Started && !missionLoaded){
            worker2Started = true;
            std::function<void(void)> func =std::bind(&MainWindow::recordMap, this);
            worker2 = std::thread(func);
        }
    }
    else{
        if(robotConnected || missionLoaded){
           recordMission = false;
        }
    }
}


void MainWindow::on_zmazGoal_clicked()
{
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


void MainWindow::on_switchButton_clicked()
{
    std::cout << "Camera widget [width, height]=[" << cameraFrame->width() << ", " << cameraFrame->height() << "]" << std::endl;
    std::cout << "Map widget [width, height]=[" << mapFrame->width() << ", " << mapFrame->height() << "]" << std::endl;

    if(switchIndex == 0){
        ui->cameraWidget->removeWidget(cameraFrame);
        ui->mapWidgetFrame->removeWidget(mapFrame);
        ui->cameraWidget->addWidget(mapFrame);
        ui->mapWidgetFrame->addWidget(cameraFrame);

        uhloprieckaCamera = std::sqrt(std::pow(cameraFrame->width(),2) + std::pow(cameraFrame->height(),2));
        uhloprieckaMapa = std::sqrt(std::pow(mapFrame->width(),2) + std::pow(mapFrame->height(),2));

        mapFrame->setScale(uhloprieckaCamera/uhloprieckaMapa);
        ui->switchButton->setText("Použi mapu");
        mapFrame->setPlaceGoals(false);
        ++switchIndex;
    }
    else if(switchIndex == 1){
        ui->cameraWidget->removeWidget(mapFrame);
        ui->mapWidgetFrame->removeWidget(cameraFrame);
        ui->cameraWidget->addWidget(cameraFrame);
        ui->mapWidgetFrame->addWidget(mapFrame);

        mapFrame->setScale(1.0);
        ui->switchButton->setText("Použi kameru");
        mapFrame->setPlaceGoals(true);
        --switchIndex;
    }
}

