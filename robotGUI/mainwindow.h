#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "alarmdialog.h"
#include <iostream>
#include <string.h>

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

private slots:

    void on_actionGo_Offline_triggered();

    void on_actionGo_Online_triggered();

    void on_actionExit_triggered();

    void on_actionAlarms_triggered();

    void on_startButton_clicked();

    void on_connectToRobotButton_clicked();


    void on_replayMissionButton_clicked();

    void on_startButton_pressed();

private:
    std::string ipAdress;
    QTimer* timer;

    bool missionLoaded;
    bool missionRunning;
    bool robotRunning;

    double robotCenterSpeed;
    double robotRotationalSpeed;

    Ui::MainWindow *ui;
    AlarmDialog *alarmHelpWindow;
};
#endif // MAINWINDOW_H
