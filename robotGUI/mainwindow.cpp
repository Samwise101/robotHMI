#include "mainwindow.h"
#include "alarmdialog.h"
#include "ui_mainwindow.h"
#include <qgridlayout.h>
#include <iostream>
#include "cameraFrameWidget.h"
#include <string>

#define PI 3.14159

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {

    missionLoaded = false;
    missionRunning = false;

    robotConnected = false;
    robotRunning = false;

    robotControlMode = true;
    missionReplayMode = false;

    recordMission = false;

    buttonPressedCount = 0;
    buttonPressedCount2 = 0;

    dataCounter = 0;
    switchIndex = 0;

    baseWidth = 574.0f;
    baseHeight = 471.0f;

    cameraFrameWidth = 480;
    cameraFrameHeight = 393;

    mapFrameWidth = 1148;
    mapFrameHeight = 942;

    workerStarted = false;
    worker2Started = false;

    ui->setupUi(this);

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

    if(!missionLoaded){
        mapFrame->updateLaserPicture = 1;
    }

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

    if(mapFrame->getShowReplayWarning() && buttonPressedCount < 150){
        buttonPressedCount++;
    }
    else if(mapFrame->getShowReplayWarning() && buttonPressedCount >= 150){
        mapFrame->setShowReplayWarning(false);
        buttonPressedCount = 0;
    }

    if(mapFrame->getShowRobotStopped() && buttonPressedCount3 < 150){
        buttonPressedCount3++;
    }
    else if(mapFrame->getShowRobotStopped() && buttonPressedCount3 >= 150){
        mapFrame->setShowRobotStopped(false);
        buttonPressedCount3 = 0;
    }

    if(mapFrame->getShowDisconnectWarning() && buttonPressedCount2 < 150){
        buttonPressedCount2++;
    }
    else if(mapFrame->getShowDisconnectWarning() && buttonPressedCount2 >= 150){
        mapFrame->setShowDisconnectWarning(false);
        buttonPressedCount2 = 0;
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
                cameraFrame->setDispOrangeWarning(false);
                cameraFrame->setDispYellowWarning(false);
                cameraFrame->setRobotStoppedWarning(true);
                mapFrame->setShowRobotStopped(true);
            }

            omega = robot->orientationRegulator(0, 0, false);
            robotRotationalSpeed = omega;
            v = robot->regulateForwardSpeed(0, 0, false, 0);
            robotForwardSpeed = v;
        }
        else if(robotRunning && !mapFrame->isGoalVectorEmpty()){
            if(mapFrame->getShortestDistanceLidar() <= 350.0){
               cameraFrame->setDispOrangeWarning(true);
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
        if(!missionLoaded)
            cameraFrame->update();

    return 0;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_startButton_clicked()
{
    if(robotControlMode && robotConnected && !robotRunning){
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
    else if(robotControlMode && robotConnected && robotRunning){
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
    if(robotControlMode && robotConnected && !robotRunning){
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

    else if(robotControlMode && robotConnected && robotRunning){
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

bool MainWindow::setupConnectionToRobot(){

    getIpAddress();

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
    if(!missionLoaded && recordMission){
        video->open("camera_" + timeOfDay.currentTime().toString("hh:mm:ss").toStdString() + ".avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 15, cv::Size(mapFrame->imageWidth,mapFrame->imageHeight), true);

        while(!isFinished && video->isOpened()){
            frame = cameraFrame->getCameraFrame();
            cv::resize(frame, dest, cv::Size(mapFrame->imageWidth,mapFrame->imageHeight));
            video->write(dest);
        }
        video->release();
    }
    else if(missionLoaded && !recordMission){
        cap.open(s1.toStdString());

        if(cap.isOpened()){
            cameraFrame->setCanReplay(true);
            while(!isFinishedReplay){
                if(missionRunning){
                    if(!cap.read(cameraFrame->replayFrame)){
                        cameraFrame->setCanReplay(false);
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
    if(!missionLoaded && recordMission){
        if(!mapFile.is_open()){
            mapFile.open(path.toStdString() + "/mapLog" + timeOfDay.currentTime().toString("hh:mm:ss").toStdString() + ".txt", ios::out);
        }

        while(!isFinished2 && mapFile.is_open()){
            mapFrame->createFrameLog(mapFile);
            this_thread::sleep_for(200ms);
        }
        mapFrame->setNumber2(0);
        mapFile.close();
    }
    else if(missionLoaded && !recordMission){

        while(!isFinishedReplay2){
            if(mapFrame->getReplayFinished()){
                mapFrame->setReplayFinished(true);
                mapFrame->setReplayIndex(0);
                break;
            }
            if(missionRunning){
                mapFrame->updateLaserPicture = 1;
                mapFrame->update();
                this_thread::sleep_for(130ms);
            }
        }

        mapFrame->updateLaserPicture = 0;
        mapFrame->update();
    }
}

void MainWindow::destroyReplayMission()
{
    if(missionLoaded){

        mapFrame->clearVectors();
        mapFrame->setReplayIndex(0);

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
        cameraFrame->setCanReplay(false);
        str.clear();
        mapFrame->setStr(str);
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

bool MainWindow::getIpAddress()
{
    if(!addressField->getAdressFieldIP().isEmpty() && ipAddress.compare(addressField->getAdressFieldIP().toStdString())){
        ipAddress = addressField->getAdressFieldIP().toStdString();
        std::cout << ipAddress << std::endl;
        return 1;
    }
    return 0;
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(robotControlMode && robotConnected){
        if(arg1 == 2 && !workerStarted && !worker2Started){
            path = dialog.getExistingDirectory(this, "Vyberte priečinok:", QDir::homePath(), QFileDialog::ShowDirsOnly);
            if(!path.isEmpty()){
                std::cout << path.toStdString() << std::endl;
                recordMission = true;
                if(robotConnected && !workerStarted && !missionLoaded){
                   if(isFinished)
                       isFinished = false;
                   workerStarted = true;
                   if(!videoCreated){
                        video = new cv::VideoWriter("camera_" + timeOfDay.currentTime().toString("hh:mm:ss").toStdString() + ".avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 15, cv::Size(mapFrame->imageWidth,mapFrame->imageHeight), true);
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
        }
        else{
            destroyRecordMission();
            recordMission = false;
        }
    }
}


void MainWindow::on_zmazGoal_clicked()
{
    if(robotControlMode){
       mapFrame->removeAllPoints();
    }
    else if(missionReplayMode){
        if(robotRunning && !missionLoaded){
            mapFrame->setShowDisconnectWarning(false);
            mapFrame->setShowReplayWarning(true);
        }
        if(missionReplayMode){
            if(!robotRunning && !missionLoaded){

                ui->zmenTypBoduButton->setStyleSheet("background-color: "
                                                       "silver;border-style:outset;border-radius: "
                                                       "10px;border-color:black;border-width:4px;padding: "
                                                        "5px;min-height: 60px;image:url(:/resource/stop_start/play.png);"
                                                       );

                missionLoaded = true;
                s1 = dialog.getOpenFileName(this, "Select a video file to open...", QDir::homePath(), "avi(*.avi);;mp4(*.mp4)");
                s2 = dialog.getOpenFileName(this, "Select a text file to open...", QDir::homePath(), "txt(*.txt)");

                std::cout << s1.toStdString() << std::endl;
                if(!s1.isEmpty() && !s2.isEmpty()){

                    if(!replayFile.is_open()){
                        replayFile.open(s2.toStdString(), ios::in);

                        while(true){
                            if(!std::getline(replayFile, str)){
                               break;
                                }
                            mapFrame->setStr(str);
                            mapFrame->parseMapFile();
                            }
                        }
                    replayFile.close();

                    mapFrame->setRobotOnline(false);
                    cameraFrame->setRobotOnline(false);

                    ui->zmazGoal->setText("Zruš\nprehrávanie");

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
                else{
                    std::cout << "HELLO" << std::endl;
                    destroyReplayMission();
                    missionLoaded = false;
                    mapFrame->setRobotOnline(true);
                    cameraFrame->setRobotOnline(true);
                }
            }
            else{
                ui->zmazGoal->setText("Otvor nahratú\nmisiu");

                destroyReplayMission();

                mapFrame->setRobotOnline(true);
                cameraFrame->setRobotOnline(true);
            }
        }
    }
}

void MainWindow::on_zmenTypBoduButton_clicked()
{
    if(missionReplayMode){
        if(!robotRunning && missionLoaded){
            if(!missionRunning){
                missionRunning = true;
                ui->zmenTypBoduButton->setStyleSheet("background-color: "
                                                       "silver;border-style:outset;border-radius: "
                                                       "10px;border-color:black;border-width:4px;padding: "
                                                        "5px;min-height: 60px;image:url(:/resource/stop_start/stop_play.png);"
                                                       );

                }
            else{
                missionRunning = false;
                ui->zmenTypBoduButton->setStyleSheet("background-color: "
                                                       "silver;border-style:outset;border-radius: "
                                                       "10px;border-color:black;border-width:4px;padding: "
                                                        "5px;min-height: 60px;image:url(:/resource/stop_start/play.png);"
                                                       );
                }
        }
    }
    else if(robotControlMode){

        goalIndex++;
        if(goalIndex > 4){
            goalIndex = 1;
        }

        if(goalIndex%4 == 1){
            ui->zmenTypBoduButton->setText("Prejazdový\n bod");
            mapFrame->setPointColor(QColor(20,255,20));
            mapFrame->setPointType(1);
        }
        else if(goalIndex%4 == 2){
            ui->zmenTypBoduButton->setText("Otočenie\n o 360°");
            mapFrame->setPointColor(Qt::magenta);
            mapFrame->setPointType(2);
        }
        else if(goalIndex%4 == 3){
            ui->zmenTypBoduButton->setText("Čakaj\n 2 sekundy");
            mapFrame->setPointColor(Qt::cyan);
            mapFrame->setPointType(3);
        }
        else{
            ui->zmenTypBoduButton->setText("Cieľový\n bod");
            mapFrame->setPointColor(QColor(192,192,192));
            mapFrame->setPointType(4);
        }
    }
}


void MainWindow::on_switchButton_clicked()
{
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


void MainWindow::on_actionDocumentation_triggered()
{
    docReader = new DocumentDialog(this);
    docReader->setWindowTitle("Návod");
    docReader->show();
}

void MainWindow::on_actionAlarms_triggered()
{
    alarmHelpWindow = new AlarmDialog(this);
    alarmHelpWindow->setWindowTitle("Alarmy");
    alarmHelpWindow->show();
}

void MainWindow::on_actionIP_adresa_triggered()
{
    addressField = new AddressDialog(this);
    addressField->setWindowTitle("Target IP");
    addressField->show();
}

void MainWindow::on_actionPripoj_sa_triggered()
{
    if(!robotConnected && !missionLoaded){
        destroyRecordMission();
        destroyReplayMission();

        if(setupConnectionToRobot()){
            robotConnected = true;
            robot->robotStart();
            cameraFrame->setTempSpeed(robot->getTempSpeed());
            cameraFrame->speedFrame = ui->speedWidget;
            cameraFrame->batteryFrame = ui->batteryWidget;
        }
    }
}


void MainWindow::on_actionOdpoj_sa_triggered()
{
    if(robotConnected){
        if(robotRunning){
           mapFrame->setShowReplayWarning(false);
           mapFrame->setShowDisconnectWarning(true);
        }
        else{
            robotControlMode = true;
            missionReplayMode = false;

            ipAddress.clear();
            destroyRecordMission();
            destroyReplayMission();

            mapFrame->removeAllPoints();
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
        }
    }
}


void MainWindow::on_actionM_d_prehr_vania_triggered()
{
    robotControlMode = false;
    missionReplayMode = true;
    mapFrame->setRobotControlOn(false);

    ui->modeLabel->setText("Mód prehrávania\naktívny");
    ui->zmazGoal->setText("Otvor nahratú\n misiu");
    ui->switchingLabel->setText("Prehraj\n misiu");
    ui->zmenTypBoduButton->setText("");
    ui->zmenTypBoduButton->setStyleSheet("background-color: silver;"
                                         "border-style:outset;"
                                         "border-radius: 10px;"
                                         "border-color:black;"
                                         "border-width:4px;"
                                         "padding: 5px;"
                                         "min-height: 60px;"
                                         "image:url(:/resource/stop_start/play.png)");
}


void MainWindow::on_actionM_d_riadenia_triggered()
{
    missionReplayMode = false;
    robotControlMode = true;
    mapFrame->setRobotControlOn(true);

    ui->modeLabel->setText("Mód plánovania\nmisie aktívny ");
    ui->switchingLabel->setText("Výber cieľového\n bodu misie");
    ui->zmazGoal->setText("Zmaž body\n misie");
    ui->zmenTypBoduButton->setText("Prejazdový\n bod");
    ui->zmenTypBoduButton->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;min-height: 24px;border-color:black;min-height: 60px;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black");
}

