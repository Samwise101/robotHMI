#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "alarmdialog.h"
#include "cameraFrameWidget.h"
#include "mapframewidget.h"
#include <iostream>
#include <string.h>

#include "robot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool startCamera();
    bool cameraUse;
    bool setBatteryLevelWidget();
    bool getIpAddress();

private slots:

    void on_actionGo_Offline_triggered();

    void on_actionGo_Online_triggered();

    void on_actionExit_triggered();

    void on_actionAlarms_triggered();

    void on_startButton_clicked();

    void on_connectToRobotButton_clicked();


    void on_replayMissionButton_clicked();

    void on_startButton_pressed();

    void on_checkBox_stateChanged(int arg1);

private:
    std::string ipAdress;
    QTimer* timer;

    Robot* robot;

    bool missionLoaded;
    bool missionRunning;
    bool robotRunning;

    double robotCenterSpeed;
    double robotRotationalSpeed;
    double batteryLevel;

    string ipAddress;

    Ui::MainWindow *ui;
    AlarmDialog *alarmHelpWindow;

    CameraFrameWidget* cameraFrame;
    MapFrameWidget* mapFrame;
};
#endif // MAINWINDOW_H
