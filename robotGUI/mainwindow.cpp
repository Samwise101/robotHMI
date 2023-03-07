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

    baseWidth = 574.0f;
    baseHeight = 471.0f;

    cameraFrameWidth = 430;
    cameraFrameHeight = 353;

    mapFrameWidth = 1148;
    mapFrameHeight = 942;

    workerStarted = false;
    worker2Started = false;

    ui->setupUi(this);

    ui->addressField->setMaxLength(20);

    cameraFrame = new CameraFrameWidget();
    cameraFrame->setFixedWidth(cameraFrameWidth);
    cameraFrame->setFixedHeight(cameraFrameHeight);
    cameraFrame->setScale(((cameraFrame->width()-15)/baseWidth));

    ui->cameraWidget->addWidget(cameraFrame, 0, 1);

    mapFrame = new MapFrameWidget();
    mapFrame->setFixedWidth(mapFrameWidth);
    mapFrame->setFixedHeight(mapFrameHeight);
    mapFrame->setScale(((mapFrame->width()-15)/baseWidth)- ((mapFrame->width()-15)/baseWidth)/20);

    ui->mapWidgetFrame->addWidget(mapFrame, 0, 2);
}

MainWindow::~MainWindow()
{
    destroyRecordMission();
    destroyReplayMission();

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

    if(!robot->getInitilize()){
        cameraFrame->setRobotOnline(true);
        cameraFrame->updateCameraPicture = 1;

        mapFrame->setRobotOnline(true);
        mapFrame->updateLaserPicture = 1;

        robot->setRobotPose(mapFrame->robotPosition.x(), mapFrame->robotPosition.y(), mapFrame->getRealTheta());
        robot->setInitilize(true);
    }

    robotStateUiSignal();

    if(mapFrame->getShowReplayWarning() && buttonPressedCount < 150)
        buttonPressedCount++;
    if(buttonPressedCount >= 150){
        mapFrame->setShowReplayWarning(false);
        buttonPressedCount = 0;
    }

    cameraFrame->setBatteryLevel(robotData.Battery);
    cameraFrame->setV(v);

    ui->speedLabel->setText(QString::number(v) + " mm/s");
    ui->batteryLabel->setText(QString::number(cameraFrame->getBatteryPercantage()) + " %");

    if(!robot->getAtGoal()){

        if(robotRunning){
           robot->robotOdometry(robotData, true);
        }

        if(!robotRunning || mapFrame->isGoalVectorEmpty() || (mapFrame->getShortestDistanceLidar() >= 180.0 && mapFrame->getShortestDistanceLidar() <= 240.0)){

            if(mapFrame->getShortestDistanceLidar() >= 180.0 && mapFrame->getShortestDistanceLidar() <= 240.0){
                cameraFrame->setDispRedWarning(false);
                cameraFrame->setDispYellowWarning(false);
                cameraFrame->setRobotStoppedWarning(true);
            }

            omega = robot->orientationRegulator(0, 0, false);
            robotRotationalSpeed = omega;
            v = robot->regulateForwardSpeed(0, 0, false, 0);
            robotForwardSpeed = v;
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

            robotRotationalSpeed = omega;
            robotForwardSpeed = v;

            if(mapFrame->getGoalType() == 1 && robot->getDistanceToGoal(mapFrame->getGoalXPosition(), mapFrame->getGoalYPosition()) < 100.0){
               robot->setAtGoal(true);
            }
            else if(v == 0.0 && omega == 0.0){
               goalAngle = robot->getTheta() + 2*PI;
               robot->setAtGoal(true);
            }
        }
    }
    else{

        if(!mapFrame->isGoalVectorEmpty()){
            if(mapFrame->getGoalType() == 2){
                if(robotRunning){
                   robot->robotOdometry(robotData, false);
                }
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

    if(robotForwardSpeed >= 0.1){
        radius = robotForwardSpeed/robotRotationalSpeed;
        if(radius == 0)
            robot->setArcSpeed(robotForwardSpeed,0);
        else
            robot->setArcSpeed(robotForwardSpeed,radius);
    }
    else if((robotForwardSpeed < 0.05) && ((robotRotationalSpeed >= 0.1) || (robotRotationalSpeed < -0.1))){
        robot->setRotationSpeed(robotRotationalSpeed);
    }
    else if((robotForwardSpeed <= 0.1) && ((robotRotationalSpeed >= -0.1) && (robotRotationalSpeed <= 0.1))){
        robot->setArcSpeed(0,0);
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
}


void MainWindow::on_connectToRobotButton_clicked()
{
    if(!robotConnected){
        destroyRecordMission();
        destroyReplayMission();
/*
        if(getIpAddress()){
            std::cout << "Ip address loaded" << std::endl;
        }
        else{
            std::cout << "No ip address loaded!" << std::endl;
        }*/

        if(!setupConnectionToRobot()){
            std::cout << "Something went wrong, can't connect to robot!" << std::endl;
        }
        else{
            ui->connectToRobotButton->setText("Odpoj sa");
            robotConnected = true;
            robot->robotStart();

            std::cout << "Successfully connected to the robot!" << std::endl;
            cameraFrame->setTempSpeed(robot->getTempSpeed());
            cameraFrame->speedFrame = ui->speedWidget;
            cameraFrame->batteryFrame = ui->batteryWidget;
        }
    }
    else{
        if(robotRunning){
           mapFrame->setShowReplayWarning(true);
        }
        else{
            destroyRecordMission();
            destroyReplayMission();

            robot->setInitilize(false);
            robotConnected = false;
            delete robot;

            mapFrame->setRobotOnline(false);
            mapFrame->updateLaserPicture = 0;
            mapFrame->setRobotInitialized(false);
            mapFrame->setIsSimulation(false);
            mapFrame->update();

            cameraFrame->setBatteryLevel(0);
            cameraFrame->setBatteryPercantage(200);
            cameraFrame->setV(0.0);
            cameraFrame->setSpeedWidget();
            cameraFrame->setBatteryWidget();
            cameraFrame->setRobotOnline(false);
            cameraFrame->updateCameraPicture = 0;
            cameraFrame->update();

            robotStateUiSignal();

            ui->connectToRobotButton->setText("Pripoj sa");
        }
    }
}

bool MainWindow::setupConnectionToRobot(){
    if(!ipAddress.empty()){
        v = 0.0;
        omega = 0.0;
        robotForwardSpeed = 0;
        robotRotationalSpeed = 0;

        if(ipAddress == "127.0.0.1"){
            mapFrame->setIsSimulation(true);
        }
        else{
           mapFrame->setIsSimulation(false);
        }

        mapFrame->initializeRobot();
        robot = new Robot(ipAddress);


        robot->setLaserParameters(ipAddress,52999,5299,std::bind(&MainWindow::processLidar,this,std::placeholders::_1));
        robot->setRobotParameters(ipAddress,53000,5300,std::bind(&MainWindow::processRobot,this,std::placeholders::_1));
        robot->setCameraParameters("http://" + ipAddress + ":" + cameraPort + "/stream.mjpg",std::bind(&MainWindow::processCamera,this,std::placeholders::_1));

        return true;
    }
    else{
        return false;
    }

}

void MainWindow::recordCamera()
{
    if(!missionLoaded){
        video->open("camera_1.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 15, cv::Size(mapFrame->imageWidth,mapFrame->imageHeight), true);

        while(!isFinished && video->isOpened()){
            frame = cameraFrame->getCameraFrame();
            cv::resize(frame, dest, cv::Size(mapFrame->imageWidth,mapFrame->imageHeight));
            video->write(dest);
        }
        video->release();
    }
    else{
        cap.open(s1.toStdString());

        if(cap.isOpened()){
            while(!isFinishedReplay){
                if(missionRunning){
                    if(!cap.read(cameraFrame->replayFrame)){
                        break;
                    }
                    cameraFrame->updateCameraPicture=1;
                    cameraFrame->update();
                }
            }
            cap.release();
            cameraFrame->updateCameraPicture = 0;
            cameraFrame->update();
        }
    }
}

void MainWindow::recordMap()
{
    if(!missionLoaded){
        if(!mapFile.is_open()){
            mapFile.open("mapLog.txt", ios::out);
        }

        while(!isFinished2 && mapFile.is_open()){
            mapFrame->createFrameLog(mapFile);
            this_thread::sleep_for(200ms);
        }
        mapFrame->setNumber2(0);
        mapFile.close();
    }
    else{
        if(!replayFile.is_open()){
            replayFile.open(s2.toStdString(), ios::in);
            while(!isFinishedReplay2 && replayFile.is_open()){
                if(missionRunning){
                    if(!std::getline(replayFile, str)){
                        break;
                    }
                    mapFrame->setStr(str);
                    mapFrame->updateLaserPicture = 1;
                    mapFrame->update();
                    this_thread::sleep_for(115ms);
                }
            }
            replayFile.close();
            mapFrame->updateLaserPicture = 0;
            mapFrame->update();
        }
    }
}

void MainWindow::destroyReplayMission()
{
    if(missionLoaded){
        if(workerStarted && !isFinishedReplay){
            isFinishedReplay = true;
            workerStarted = false;
            std::cout << "camera recording thread finished\n";
            worker.join();
        }

        if(worker2Started && !isFinishedReplay2){
            isFinishedReplay2 = true;
            worker2Started = false;
            std::cout << "map recording thread finished\n";
            worker2.join();
        }

        missionLoaded = false;
        missionRunning = false;
    }
}

void MainWindow::destroyRecordMission()
{
    if(!missionLoaded){
        if(workerStarted && !isFinished){
            isFinished = true;
            workerStarted = false;
            if(videoCreated){
                 videoCreated = false;
            }
            std::cout << "camera recording thread finished\n";
            worker.join();
            delete video;
        }

        if(worker2Started && !isFinished2){
            isFinished2 = true;
            worker2Started = false;
            std::cout << "map recording thread finished\n";
            worker2.join();
        }
    }
}


void MainWindow::robotStateUiSignal()
{
    if(!robotConnected){
        ui->robotState->setText("Robot offline");
        return;
    }

    if(robotRunning){
       if(!mapFrame->isGoalVectorEmpty()){
            if(!robot->getAtGoal()){
                if(mapFrame->getGoalType() == 1){
                   ui->robotState->setText("Robot sa presúva do\n prejazdového bodu");
                }
                else if(mapFrame->getGoalType() == 4){
                   ui->robotState->setText("Robot sa presúva\ndo cieľového bodu");
                }
                else{
                   ui->robotState->setText("Robot sa presúva\ndo bodu misie");
                }
            }
            else if(mapFrame->getGoalType() == 2){
                ui->robotState->setText("Robot dosiahol\nzadaný cieľ\nRobot sa otáča\no 360 stupňou");
            }
            else if(mapFrame->getGoalType() == 3){
                ui->robotState->setText("Robot dosiahol\nzadaný cieľ\nRobot čaká\n2 sekundy");
            }
       }
       else{
            ui->robotState->setText("Robot čaká na\nzadanie misie");
       }
    }
    else{
        ui->robotState->setText("Robot je neaktívny");
    }
}



void MainWindow::on_loadMissionButton_clicked()
{
    if(!robotConnected){
        destroyReplayMission();

        ui->replayMissionButton->setStyleSheet("#replayMissionButton{background-color: "
                                               "silver;border-style:outset;border-radius: "
                                               "10px;border-color:black;border-width:4px;padding: "
                                                "5px;image:url(:/resource/stop_start/play.png)}"
                                               );

        missionLoaded = true;
        s1 = dialog.getOpenFileName(this, "Select a video file to open...", QDir::homePath(), "avi(*.avi);;mp4(*.mp4)");
        s2 = dialog.getOpenFileName(this, "Select a text file to open...", QDir::homePath(), "txt(*.txt)");

        if(!s1.isEmpty() && !s2.isEmpty()){
            if(!workerStarted){
                isFinishedReplay = false;
                workerStarted = true;
                func =std::bind(&MainWindow::recordCamera, this);
                worker = std::thread(func);
            }

            if(!worker2Started){
                isFinishedReplay2 = false;
                worker2Started = true;
                func =std::bind(&MainWindow::recordMap, this);
                worker2 = std::thread(func);
            }
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
    if(robotConnected){
        if(buttonPressedCount >= 5){
            mapFrame->setShowReplayWarning(true);
        }
        buttonPressedCount++;
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
           if(isFinished)
               isFinished = false;
           workerStarted = true;
           if(!videoCreated){
                video = new cv::VideoWriter("camera_1.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 15, cv::Size(mapFrame->imageWidth,mapFrame->imageHeight), true);
                videoCreated = true;
           }
           func =std::bind(&MainWindow::recordCamera, this);
           worker = std::thread(func);
        }
        if(robotConnected && !worker2Started && !missionLoaded){
            if(isFinished2)
                isFinished2 = false;
            worker2Started = true;
            func =std::bind(&MainWindow::recordMap, this);
            worker2 = std::thread(func);
        }
    }
    else{
        destroyRecordMission();
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
        cameraFrame->setFixedWidth(mapFrameWidth);
        cameraFrame->setFixedHeight(mapFrameHeight);
        cameraFrame->setScale(((cameraFrame->width()-15)/baseWidth));

        mapFrame->setFixedWidth(cameraFrameWidth);
        mapFrame->setFixedHeight(cameraFrameHeight);
        mapFrame->setScale(((mapFrame->width()-15)/baseWidth)- ((mapFrame->width()-15)/baseWidth)/20);

        ui->cameraWidget->removeWidget(cameraFrame);
        ui->mapWidgetFrame->removeWidget(mapFrame);
        ui->cameraWidget->addWidget(mapFrame);
        ui->mapWidgetFrame->addWidget(cameraFrame);

        ui->switchButton->setText("Použi mapu");
        mapFrame->setPlaceGoals(false);
        ++switchIndex;
    }
    else if(switchIndex == 1){

        cameraFrame->setFixedWidth(cameraFrameWidth);
        cameraFrame->setFixedHeight(cameraFrameHeight);
        cameraFrame->setScale(((cameraFrame->width()-15)/baseWidth));

        mapFrame->setFixedWidth(mapFrameWidth);
        mapFrame->setFixedHeight(mapFrameHeight);
        mapFrame->setScale(((mapFrame->width()-15)/baseWidth)- ((mapFrame->width()-15)/baseWidth)/20);

        ui->cameraWidget->removeWidget(mapFrame);
        ui->mapWidgetFrame->removeWidget(cameraFrame);
        ui->cameraWidget->addWidget(cameraFrame);
        ui->mapWidgetFrame->addWidget(mapFrame);

        ui->switchButton->setText("Použi kameru");
        mapFrame->setPlaceGoals(true);
        --switchIndex;
    }
}


void MainWindow::on_mouseTracking_clicked()
{
    if(mapFrame->toggleMouse()){
        ui->mouseTracking->setText("Zapni ukazovateľ\npozície v mape");
    }
    else{
        ui->mouseTracking->setText("Vypni ukazovateľ\npozície v mape");
    }
}


void MainWindow::on_useMapButton_clicked()
{
    if(mapFrame->toggleMap()){
        ui->useMapButton->setText("Skri mapu");
    }
    else{
        ui->useMapButton->setText("Zobraz mapu");
    }
}

