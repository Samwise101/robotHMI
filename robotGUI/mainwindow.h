#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool setBatteryLevelWidget();
    bool getIpAddress();
    void connectRobotUiSetup();
    int processLidar(LaserMeasurement laserData);
    int processCamera(cv::Mat cameraData);
    int processRobot(TKobukiData robotData);

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
    std::string ipAddress;
    QTimer* timer;
    int interval; //in miliseconds

    Robot* robot;

    int dataCounter;

    bool missionLoaded;
    bool missionRunning;
    bool recordMission;
    bool robotConnected;
    bool robotRunning;

    int index;

    TKobukiData robotdata;

    double robotForwardSpeed;
    double robotRotationalSpeed;

    double batteryLevel;

    Ui::MainWindow *ui;
    AlarmDialog *alarmHelpWindow;

    CameraFrameWidget* cameraFrame;
    MapFrameWidget* mapFrame;

public slots:
    void setUiValues(double robotX,double robotY,double robotFi);
    void callbackTest();
signals:
    void uiValuesChanged(double newrobotX,double newrobotY,double newrobotFi);
};
#endif // MAINWINDOW_H
