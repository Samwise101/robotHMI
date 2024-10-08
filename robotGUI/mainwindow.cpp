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
    turnRobot = false;

    buttonPressedCount = 0;
    buttonPressedCount2 = 0;

    dataCounter = 0;
    switchIndex = 0;

    baseWidth = 574.0f;
    baseHeight = 471.0f;

    cameraFrameWidth = 558;
    cameraFrameHeight = 471;

    mapFrameWidth = 1148;
    mapFrameHeight = 942;

    workerStarted = false;
    worker2Started = false;

    ui->setupUi(this);

    cameraFrame = new CameraFrameWidget();
    cameraFrame->setFixedWidth(cameraFrameWidth);
    cameraFrame->setFixedHeight(cameraFrameHeight);

    ui->cameraWidget->addWidget(cameraFrame, 0, 1);

    mapFrame = new MapFrameWidget();
    mapFrame->setFixedWidth(mapFrameWidth);
    mapFrame->setFixedHeight(mapFrameHeight);

    cameraFrame->setPointVector(mapFrame->getRobotGoals());

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
        mapFrame->update();
    }

    return 0;
}

int MainWindow::processRobot(TKobukiData robotData){

    if(!robot->getInitilize()){

        robot->overWriteOldEncValues(robotData);
        robot->robotOdometry(robotData, true);

        cameraFrame->setRobotOnline(true);
        cameraFrame->updateCameraPicture = 1;

        mapFrame->setRobotOnline(true);
        mapFrame->updateLaserPicture = 1;

        robot->setRobotPose(mapFrame->robotPosition.x(), mapFrame->robotPosition.y(), robot->getTheta());
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

    if(!robot->getAtGoal()){
        if(robotRunning){
           robot->robotOdometry(robotData, true);
        }
        if(!robotRunning || mapFrame->isGoalVectorEmpty() || (mapFrame->getShortestDistanceLidar() >= 180.0 && mapFrame->getShortestDistanceLidar() <= 240.0)){
            if(mapFrame->getShortestDistanceLidar() >= 180.0 && mapFrame->getShortestDistanceLidar() <= 240.0){
                if((mapFrame->getLidarAngle() >= 3*PI/2 && mapFrame->getLidarAngle() <= 2*PI) ||
                 (mapFrame->getLidarAngle() >= 0.0 && mapFrame->getLidarAngle() <= PI/2)){
                    cameraFrame->setDispOrangeWarning(false);
                    cameraFrame->setDispYellowWarning(false);
                    cameraFrame->setRobotStoppedWarning(true);
                    mapFrame->setShowRobotStopped(true);
                }
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
        if(!mapFrame->isGoalVectorEmpty() && !turnRobot){
            if(mapFrame->getGoalType() == 2){
                if(robotRunning){
                   robot->robotOdometry(robotData, false);
                }
                omega = robot->robotFullTurn(goalAngle);
                if(omega > -0.5 && omega < 0.15){
                    mapFrame->removeLastPoint();
                    robot->setAtGoal(false);
                }
                robotRotationalSpeed = omega;
            }
            else if(mapFrame->getGoalType() == 3){
                std::this_thread::sleep_for(2000ms);
                mapFrame->removeLastPoint();
                robot->setAtGoal(false);
            }
            else{
                mapFrame->removeLastPoint();
                robot->setAtGoal(false);
            }
        }
        else if(mapFrame->isGoalVectorEmpty() && turnRobot){
            if(robotRunning){
               robot->robotOdometry(robotData, false);
            }
            omega = robot->robotFullTurn(goalAngle);
            if(omega > -0.15 && omega < 0.15){
                turnRobot = false;
                robot->setAtGoal(false);
            }
            robotRotationalSpeed = omega;
            if(robotRunning){
               robot->robotOdometry(robotData, true);
            }
        }
        else{
            turnRobot = false;
            robot->setAtGoal(false);
            if(robotRunning){
               robot->robotOdometry(robotData, true);
            }
        }
    }

    mapFrame->updateRobotValuesForGUI(robot->getX(), robot->getY(), robot->getTheta());
    cameraFrame->setRobotParams(robot->getX(), robot->getY(),robot->getTheta());

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

/// Metóda na zavretie aplikácie pomocou menuItem Exit v HMI mainwindow menuBar.
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

/// Metóda pre zastavenie a opätovné spustenie robota Kobuki stlačením tlačidla "STOP" a "ŠTART".
///
/// Po stlačení tlačidla, ak robot stál, tak potom premenná robotRunning = true, ak bol robot v pohybe, tak robotRunning = false.
/// @note Stlačením tlačidla sa mení vizualizácia tlačidla v HMI.
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
        if(turnRobot && robot->getAtGoal()){
            turnRobot = false;
            robot->setAtGoal(false);
        }
    }
}


/// Metóda je len na zmenu vizualizácie tlačidla ŠTART a STOP pri držaní stlačeného tlačidla.
///
/// Nemá veľký význam.
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

/// Metóda na začatie komunikácie HMI s robotom Kobuki.
///
/// Spustenie komunikácie medzi HMI a robotom Kobuki, ak bola zadaná správna IP adresa robota.
/// @note Zistí port na ktorom komunikuje kamera, podľa IP adresy - simulátor komunikuje na 8889.
/// @see validateIpAdress
void MainWindow::setupConnectionToRobot(){


    if(!ipAddress.empty() && validateIPAdress(ipAddress) && !robotConnected){
        v = 0.0;
        omega = 0.0;
        robotForwardSpeed = 0;
        robotRotationalSpeed = 0;

        if(ipAddress == "127.0.0.1"){
            mapFrame->setIsSimulation(true);
            cameraPort = "8889";
        }
        else{
           mapFrame->setIsSimulation(false);
           cameraPort = "8000";
        }

        mapFrame->initializeRobot();

        robot = new Robot(ipAddress);

        robot->setLaserParameters(ipAddress,52999,5299,std::bind(&MainWindow::processLidar,this,std::placeholders::_1));
        robot->setRobotParameters(ipAddress,53000,5300,std::bind(&MainWindow::processRobot,this,std::placeholders::_1));
        robot->setCameraParameters("http://" + ipAddress + ":" + cameraPort + "/stream.mjpg",std::bind(&MainWindow::processCamera,this,std::placeholders::_1));

        robotConnected = true;

        robot->robotStart();
        cameraFrame->setTempSpeed(robot->getTempSpeed());
    }
}

void MainWindow::recordCamera()
{
    if(!missionLoaded && recordMission){
        video->open(path.toStdString() + "/camera_" + timeOfDay.currentTime().toString("hh_mm_ss").toStdString() + ".avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 15, cv::Size(mapFrame->imageWidth,mapFrame->imageHeight), true);

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
            mapFile.open(path.toStdString() + "/mapLog_" + timeOfDay.currentTime().toString("hh_mm_ss").toStdString() + ".txt", ios::out);
        }

        while(!isFinished2 && mapFile.is_open()){
            mapFrame->createFrameLog(mapFile);
            std::this_thread::sleep_for(70ms);
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
                std::this_thread::sleep_for(29.5ms);
            }
        }
        mapFrame->updateLaserPicture = 0;
        mapFrame->update();
    }
}


/// Metóda na ukončenie spustených threadov pre prehrávanie nahratej misie robota, ak bola otvorená a načítaná misia.
void MainWindow::destroyReplayMission()
{
    if(missionLoaded){

        mapFrame->clearVectors();
        mapFrame->setReplayIndex(0);

        if(workerStarted && !isFinishedReplay){
            isFinishedReplay = true;
            workerStarted = false;
            worker.join();
        }

        if(worker2Started && !isFinishedReplay2){
            isFinishedReplay2 = true;
            worker2Started = false;
            worker2.join();
        }

        missionLoaded = false;
        missionRunning = false;
        cameraFrame->setCanReplay(false);
        str.clear();
        mapFrame->setStr(str);
        mapFrame->updateLaserPicture = 1;
        mapFrame->update();
    }
}

/// Metóda na ukončenie spustených threadov pre nahrávanie priebehu misie robota.
void MainWindow::destroyRecordMission()
{
    if(!missionLoaded){
        if(workerStarted && !isFinished){
            isFinished = true;
            workerStarted = false;
            if(videoCreated){
                 videoCreated = false;
            }
            worker.join();
            delete video;
        }

        if(worker2Started && !isFinished2){
            isFinished2 = true;
            worker2Started = false;
            worker2.join();
        }
    }
}

/// Metóda na úpravu vizualizácie stavu robota a stavu vykonávania misie v HMI
///
/// Zisťuje stav robota na základe flagov robotRunning, robotConnected a veľkosti vektora bodov misie.
/// @note
/// Stav robota = "robot online/offline".
/// Stav misie napr. "robot sa presúva do cieľového bodu", "robot čaká na zadanie misie".
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
                ui->robotState->setText("Robot dosiahol zadaný cieľ\na otáča sa o 360 stupňou");
            }
            else if(mapFrame->getGoalType() == 3){
                ui->robotState->setText("Robot dosiahol zadaný cieľ\na čaká 2 sekundy");
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

/// Metóda používajúca regex na validáciu IP adresy zadanej používateľom.
///
/// Používa regex pattern. Ak sa zhoduje dĺžka stringu a počet a typ znakov v jednotlivých čatiach, tak je IP adresa vyhodnotená ako správna.
/// @warning Metóda zabezpečí, ža je možné do IP adresy zadať len celé čísla, a že nie je možné zadať menší alebo väčší počet znakov.
/// Avšak nie je schopná zistiť, či IP adresa patrí reálnemu zariadeniu.
bool MainWindow::validateIPAdress(std::string ipAdress)
{
    std::regex ipPattern(R"(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)");
    std::smatch matches;

    if (std::regex_match(ipAdress, matches, ipPattern)) {
        for (size_t i = 1; i <= 4; ++i) {
            int octet = std::stoi(matches[i]);
            if (octet < 0 || octet > 255) {
                return false;
            }
        }
        return true;
    }
    return false;
}

/// Metóda na spustenie nahrávania priebehu misie robota zvolením možnosti "Nahraj misiu" v HMI.
///
///
void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(robotControlMode && robotConnected){
        if(arg1 == 2 && !workerStarted && !worker2Started){
            path = dialog.getExistingDirectory(this, "Vyberte priečinok:", QDir::homePath(), QFileDialog::ShowDirsOnly);
            if(!path.isEmpty()){
                recordMission = true;
                if(robotConnected && !workerStarted && !missionLoaded){
                   if(isFinished)
                       isFinished = false;
                   workerStarted = true;
                   if(!videoCreated){
                        video = new cv::VideoWriter(path.toStdString() + "/camera_" + timeOfDay.currentTime().toString("hh_mm_ss").toStdString() + ".avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 15, cv::Size(mapFrame->imageWidth,mapFrame->imageHeight), true);
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
                s2 = dialog.getOpenFileName(this, "Select a text file to open...", s1, "txt(*.txt)");

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
                        func = std::bind(&MainWindow::recordCamera, this);
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
                    destroyReplayMission();
                    missionLoaded = false;
                    mapFrame->setRobotOnline(true);
                    cameraFrame->setRobotOnline(true);
                }
            }
            else{
                ui->zmazGoal->setText("Otvor nahranú\nmisiu");
                ui->zmenTypBoduButton->setStyleSheet("background-color: "
                                                       "silver;border-style:outset;border-radius: "
                                                       "10px;border-color:black;border-width:4px;padding: "
                                                        "5px;min-height: 60px;image:url(:/resource/stop_start/play.png);"
                                                       );

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
            cameraFrame->setPointColor(QColor(20,255,20));
            cameraFrame->setPointType(1);
        }
        else if(goalIndex%4 == 2){
            ui->zmenTypBoduButton->setText("Otočenie\n o 360°");
            mapFrame->setPointColor(Qt::darkMagenta);
            mapFrame->setPointType(2);
            cameraFrame->setPointColor(Qt::darkMagenta);
            cameraFrame->setPointType(2);
        }
        else if(goalIndex%4 == 3){
            ui->zmenTypBoduButton->setText("Čakaj\n 2 sekundy");
            mapFrame->setPointColor(Qt::cyan);
            mapFrame->setPointType(3);
            cameraFrame->setPointColor(Qt::cyan);
            cameraFrame->setPointType(3);
        }
        else{
            ui->zmenTypBoduButton->setText("Cieľový\n bod");
            mapFrame->setPointColor(QColor(192,192,192));
            mapFrame->setPointType(4);
            cameraFrame->setPointColor(QColor(192,192,192));
            cameraFrame->setPointType(4);
        }
    }
}


void MainWindow::on_switchButton_clicked()
{
    if(switchIndex == 0){

        cameraFrame->setFixedWidth(mapFrameWidth);
        cameraFrame->setFixedHeight(mapFrameHeight);
        cameraFrame->setScaleFactorHeight(942);
        cameraFrame->setScaleFactorWidth(1148);
        //cameraFrame->setScale(((cameraFrame->width()-15)/baseWidth));

        mapFrame->setFixedWidth(cameraFrameWidth);
        mapFrame->setFixedHeight(cameraFrameHeight);
        mapFrame->setScaleFactorHeight(471);
        mapFrame->setScaleFactorWidth(558);
        //mapFrame->setScale(((mapFrame->width()-15)/baseWidth)- ((mapFrame->width()-15)/baseWidth)/20);

        ui->cameraWidget->removeWidget(cameraFrame);
        ui->mapWidgetFrame->removeWidget(mapFrame);

        ui->cameraWidget->addWidget(mapFrame, 0, 1);
        ui->mapWidgetFrame->addWidget(cameraFrame, 0, 2);

        ui->switchButton->setText("Použi mapu");
        mapFrame->setPlaceGoals(false);
        cameraFrame->setCanPlacePoints(true);
        ++switchIndex;
    }
    else if(switchIndex == 1){

        cameraFrame->setFixedWidth(cameraFrameWidth);
        cameraFrame->setFixedHeight(cameraFrameHeight);
        cameraFrame->setScaleFactorHeight(471);
        cameraFrame->setScaleFactorWidth(558);
        //cameraFrame->setScale(((cameraFrame->width()-15)/baseWidth));

        mapFrame->setFixedWidth(mapFrameWidth);
        mapFrame->setFixedHeight(mapFrameHeight);
        mapFrame->setScaleFactorHeight(942);
        mapFrame->setScaleFactorWidth(1148);
        //mapFrame->setScale(((mapFrame->width()-15)/baseWidth)- ((mapFrame->width()-15)/baseWidth)/20);

        ui->cameraWidget->removeWidget(mapFrame);
        ui->mapWidgetFrame->removeWidget(cameraFrame);

        ui->cameraWidget->addWidget(cameraFrame, 0, 1);
        ui->mapWidgetFrame->addWidget(mapFrame, 0, 2);

        ui->switchButton->setText("Použi kameru");
        mapFrame->setPlaceGoals(true);
        cameraFrame->setCanPlacePoints(false);
        --switchIndex;
    }
}


void MainWindow::on_mouseTracking_clicked()
{
    if(mapFrame->toggleMouse()){
        ui->mouseTracking->setText("Skry\nukazovateľ\npozície");
    }
    else{
        ui->mouseTracking->setText("Zobraz\nukazovateľ\npozície");
    }
}


void MainWindow::on_useMapButton_clicked()
{
    if(mapFrame->toggleMap()){
        ui->useMapButton->setText("Skry mapu");
    }
    else{
        ui->useMapButton->setText("Zobraz mapu");
    }
}


void MainWindow::on_actionDocumentation_triggered()
{
    docReader = new DocumentDialog(this);
    docReader->setWindowTitle("Manuál");
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
    addressField = new AddressDialog(&ipAddress,this);
    std::function<void(void)> test = std::bind(&MainWindow::setupConnectionToRobot, this);
    addressField->setFunction(test);
    addressField->setWindowTitle("Target IP");
    addressField->show();
}

void MainWindow::on_actionPripoj_sa_triggered()
{
    if(!robotConnected && !missionLoaded && !ipAddress.empty()){
        destroyRecordMission();
        destroyReplayMission();
        setupConnectionToRobot();
    }
    else if(!robotConnected && !missionLoaded && ipAddress.empty()){
        addressField = new AddressDialog(&ipAddress,this);
        std::function<void(void)> test = std::bind(&MainWindow::setupConnectionToRobot, this);
        addressField->setFunction(test);
        addressField->setWindowTitle("Target IP");
        addressField->show();
    }
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


void MainWindow::on_actionOto_enie_o_90_triggered()
{
    if(robotRunning && !robot->getAtGoal() && !turnRobot){
        goalAngle = robot->getTheta() + PI/2;
        turnRobot = true;
        robot->setAtGoal(true);
    }
}


void MainWindow::on_actionOto_enie_o_91_triggered()
{
    if(robotRunning && !robot->getAtGoal() && !turnRobot){
        goalAngle = robot->getTheta() - PI/2;
        turnRobot = true;
        robot->setAtGoal(true);
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
            if(missionReplayMode){
                missionReplayMode = false;
                robotControlMode = true;
                mapFrame->setRobotControlOn(true);

                ui->modeLabel->setText("Mód plánovania\nmisie aktívny ");
                ui->switchingLabel->setText("Výber cieľového\n bodu misie");
                ui->zmazGoal->setText("Zmaž body\n misie");
                ui->zmenTypBoduButton->setText("Prejazdový\n bod");
                ui->zmenTypBoduButton->setStyleSheet("background-color: silver;border-style:outset;border-radius: 10px;min-height: 24px;border-color:black;min-height: 60px;border-width:4px;padding: 5px;font: 700 12pt Segoe UI;color:black");
            }

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
            cameraFrame->setRobotOnline(false);
            cameraFrame->updateCameraPicture = 0;
            cameraFrame->update();

            robotStateUiSignal();
        }
    }
}


void MainWindow::on_actionM_d_prehr_vania_triggered()
{
    if(!robotRunning){
        robotControlMode = false;
        missionReplayMode = true;
        mapFrame->setRobotControlOn(false);

        ui->modeLabel->setText("Mód prehrávania\naktívny");
        ui->zmazGoal->setText("Otvor nahranú\n misiu");
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
    else{
        mapFrame->setShowReplayWarning(true);
    }
}

