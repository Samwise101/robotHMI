#include "mainwindow.h"
#include "alarmdialog.h"
#include "ui_mainwindow.h"
#include <qgridlayout.h>
#include <iostream>
#include "cameraFrameWidget.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {

    std::cout << "Main window opened" << std::endl;

    cameraUse = false;
    missionLoaded = missionRunning = false;
    robotRunning = true;

    batteryLevel = 81.0;

    robot = new Robot();

    ui->setupUi(this);

    if(setBatteryLevelWidget()){
        std::cout  << "Success!" << std::endl;
    }

    cameraFrame = new CameraFrameWidget();
    ui->cameraMapLayout->addWidget(cameraFrame, 0, 1);

    mapFrame = new MapFrameWidget();
    ui->cameraMapLayout->addWidget(mapFrame, 0, 2);
}

MainWindow::~MainWindow()
{
    delete robot;
    delete cameraFrame;
    delete mapFrame;
    delete ui;
}

bool MainWindow::startCamera(){
    std::cout << "Camera before: " << cameraUse << std::endl;
    if(cameraUse == false){
        cameraUse = true;
        std::cout << "Camera after: " << cameraUse << std::endl;
        return true;
    }
    else{
        cameraUse = false;
        std::cout << "Camera before: " << cameraUse << std::endl;
        return true;
    }
    return false;
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
    std::cout << "Hello from start button!" << std::endl;
    if(robotRunning){
         robotRunning = false;
         ui->startButton->setStyleSheet("#startButton{"
                                        "background-color: silver;"
                                        "border-style:outset;"
                                        "border-radius: 10px;"
                                        "border-color:black;"
                                        "border-width:4px;"
                                        "padding: 5px;"
                                        "image: url(:/resource/stop_start/start.png);}"
                                        );
    }
    else{
         robotRunning = true;
         ui->startButton->setStyleSheet("#startButton{"
                                        "background-color: silver"
                                        ";border-style:outset;"
                                        "border-radius: 10px;"
                                        "border-color:black;"
                                        "border-width:4px;"
                                        "padding: 5px;"
                                        "image: url(:/resource/stop_start/stop.png);}"
                                        );
    }
}

void MainWindow::on_startButton_pressed()
{
    if(robotRunning){
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

    else{
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
    std::cout << "Hello from connect button!" << std::endl;
    if(!cameraUse){
        if(startCamera()){
            std::cout << "Camera connected!" << std::endl;
        }
        else{
            std::cout << "Camera NOT connected" << std::endl;
        }
    }
}



void MainWindow::on_replayMissionButton_clicked()
{
    std::cout << "Mission before:  " << missionRunning << std::endl;

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

bool MainWindow::setBatteryLevelWidget(){
    if(robotRunning){
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

void MainWindow::on_checkBox_stateChanged(int arg1)
{

}

