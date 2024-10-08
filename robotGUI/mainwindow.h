#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QDateTime>

#include <thread>
#include <fstream>
#include <iostream>
#include <string.h>
#include <regex>

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

/// statické boolovské premenné na odsledovanie ukončenia prehrávania .avi a parsovania .txt súborov misie
static bool isFinished = false;
static bool isFinished2 = false;
static bool isFinishedReplay = false;
static bool isFinishedReplay2 = false;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/// Trieda hlavného okna HMI
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// Konštruktor triedy hlavného okna HMI
    MainWindow(QWidget *parent = nullptr);
    /// Deštruktor triedy hlavného okna HMI
    ///
    /// Zničí všetky použité pointre a ukončí všetky thready (ak sú spustené).
    ~MainWindow();

    void setupConnectionToRobot();
    /// metóda slúžiaca na spracovanie dát z laserového diaľkomeru
    ///
    int processLidar(LaserMeasurement laserData);
    /// metóda slúžiaca na spracovanie dát z kamery robota Kobuki
    ///
    /// @param cameraData parameter je opencv maticou
    int processCamera(cv::Mat cameraData);
    /// metóda slúžiaca na riadenie robota
    ///
    /// metóda slúži na riadenie rýchlosti a orientácie robota, volanie metód na vykresľovanie v GUI
    ///, zisťovanie alarmových stavov, riešenie úloh robota v bodoch misie.
    /// @param robotData parameter je štruktúrou TKobukiData, ktorá obsahuje dáta, ktoré posiela robot Kobuki
    /// (napr. počet tickov enkóoderov kolies, uhol natočenia gyroskopu atď.)
    int processRobot(TKobukiData robotData);
    void recordCamera();
    void recordMap();
    void destroyReplayMission();
    void destroyRecordMission();
    void robotStateUiSignal();

    static bool validateIPAdress(std::string ipAdress);

private slots:
    void on_actionExit_triggered();

    void on_actionAlarms_triggered();

    void on_startButton_clicked();

    void on_startButton_pressed();

    void on_checkBox_stateChanged(int arg1);

    void on_zmazGoal_clicked();

    void on_zmenTypBoduButton_clicked();

    void on_switchButton_clicked();

    void on_mouseTracking_clicked();

    void on_useMapButton_clicked();

    void on_actionDocumentation_triggered();

    void on_actionPripoj_sa_triggered();

    void on_actionIP_adresa_triggered();

    void on_actionM_d_prehr_vania_triggered();

    void on_actionM_d_riadenia_triggered();

    void on_actionOto_enie_o_90_triggered();

    void on_actionOto_enie_o_91_triggered();

    void on_actionOdpoj_sa_triggered();

private:
    std::string cameraPort = "";        // pre simulaciu

    std::string ipAddress = "";

    QDate date;
    QTime timeOfDay;

    bool turnRobot = false;

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
    QString path;

    std::thread worker;
    bool workerStarted;

    std::thread worker2;
    bool worker2Started;

    bool alarmSet = false;
    bool videoCreated = false;

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
    AlarmDialog* alarmHelpWindow;
    DocumentDialog* docReader;
    AddressDialog* addressField;

    CameraFrameWidget* cameraFrame;
    MapFrameWidget* mapFrame;

};
#endif // MAINWINDOW_H
