#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

#include <thread>
#include <fstream>
#include <iostream>
#include <string.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"

#include "alarmdialog.h"
#include "documentdialog.h"
#include "addressdialog.h"
#include "cameraFrameWidget.h"
#include "mapframewidget.h"
#include "robot.h"

static bool isFinished = false;
static bool isFinished2 = false;
static bool isFinishedReplay = false;
static bool isFinishedReplay2 = false;

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
    bool setupConnectionToRobot();
    int processLidar(LaserMeasurement laserData);
    int processCamera(cv::Mat cameraData);
    int processRobot(TKobukiData robotData);
    void recordCamera();
    void recordMap();
    void destroyReplayMission();
    void destroyRecordMission();
    void robotStateUiSignal();

private slots:
    void on_actionExit_triggered();

    void on_actionAlarms_triggered();

    void on_startButton_clicked();

    void on_replayMissionButton_clicked();

    void on_startButton_pressed();

    void on_checkBox_stateChanged(int arg1);

    void on_zmazGoal_clicked();

    void on_zmenTypBoduButton_clicked();

    void on_switchButton_clicked();

    void on_loadMissionButton_clicked();

    void on_mouseTracking_clicked();

    void on_useMapButton_clicked();

    void on_actionDocumentation_triggered();

    void on_actionPripoj_sa_triggered();

    void on_actionIP_adresa_triggered();

    void on_actionOdpoj_sa_triggered();

    void on_actionM_d_prehr_vania_triggered();

    void on_actionM_d_riadenia_triggered();

private:
    //std::string ipAddress = "127.0.0.1";    // pre simulaciu
    std::string cameraPort = "8889";        // pre simulaciu

    //std::string ipAddress = "192.168.1.11";   // pre realneho robota 8000
    //std::string cameraPort = "8000";          // pre realneho robota 8000

    std::string ipAddress = "";

    bool robotControlMode = true;
    bool missionReplayMode = false;

    Robot* robot;

    float baseWidth;
    float baseHeight;

    int mapFrameWidth;
    int mapFrameHeight;

    int cameraFrameWidth;
    int cameraFrameHeight;

    fstream mapFile;
    fstream replayFile;

    std::function<void(void)> func;

    std::string str;

    cv::VideoCapture cap;

    QFileDialog dialog;

    std::thread worker;
    bool workerStarted;

    std::thread worker2;
    bool worker2Started;

    bool alarmSet = false;
    bool videoCreated = false;

    bool finReplay1 = false;
    bool finReplay2 = false;

    cv::VideoWriter* video;

    int buttonPressedCount = 0;
    int buttonPressedCount2 = 0;
    int buttonPressedCount3 = 0;

    QImage image;
    QString s1;
    QString s2;

    cv::Mat frame;
    cv::Mat dest;

    int dataCounter;

    float goalAngle = 0.0f;

    bool missionLoaded;
    bool missionRunning;
    bool recordMission;
    bool robotConnected;
    bool robotRunning;

    double omega = 0.0;
    double v = 0.0;
    int radius = 0;

    float uhloprieckaCamera = 0.0f;
    float uhloprieckaMapa = 0.0f;

    int goalIndex = 1;
    int switchIndex = 0;

    TKobukiData robotdata;

    double robotForwardSpeed = 0;
    double robotRotationalSpeed = 0;

    double batteryLevel;

    Ui::MainWindow* ui;
    AlarmDialog alarmHelpWindow;
    DocumentDialog docReader;
    AddressDialog addressField;

    CameraFrameWidget* cameraFrame;
    MapFrameWidget* mapFrame;

};
#endif // MAINWINDOW_H
