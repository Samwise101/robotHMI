#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <thread>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"

#include "alarmdialog.h"
#include "cameraFrameWidget.h"
#include "mapframewidget.h"
#include <iostream>
#include <string.h>

#include "robot.h"

static bool isFinished = false;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool getIpAddress();
    void connectRobotUiSetup();
    bool setupConnectionToRobot();
    int processLidar(LaserMeasurement laserData);
    int processCamera(cv::Mat cameraData);
    int processRobot(TKobukiData robotData);
    void doWork();

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

    void on_zmazGoal_clicked();

    void on_zmenTypBoduButton_clicked();

    void on_switchButton_clicked();

private:
    std::string ipAddress = "127.0.0.1";    // pre simulaciu
    std::string cameraPort = "8889";        // pre simulaciu

    //std::string ipAddress = "192.168.1.12";   // pre realneho robota 8000
    //std::string cameraPort = "8000";          // pre realneho robota 8000

    Robot* robot;

    std::thread worker;
    bool threadStarted;

    bool alarmSet = false;

    cv::VideoWriter* video;
    QImage image;

    cv::Mat frame;

    int dataCounter;

    float goalAngle = 0.0f;

    bool missionLoaded;
    bool missionRunning;
    bool recordMission;
    bool robotConnected;
    bool robotRunning;

    double omega = 0.0;
    double v = 0.0;

    float uhloprieckaCamera = 0.0f;
    float uhloprieckaMapa = 0.0f;

    int goalIndex = 1;
    int switchIndex = 0;

    TKobukiData robotdata;

    double robotForwardSpeed = 0;
    double robotRotationalSpeed = 0;

    double batteryLevel;

    Ui::MainWindow* ui;
    AlarmDialog* alarmHelpWindow;

    CameraFrameWidget* cameraFrame;
    MapFrameWidget* mapFrame;

};
#endif // MAINWINDOW_H
