#ifndef CAMERAFRAMEWIDGET_H
#define CAMERAFRAMEWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QTimer>
#include <cmath>
#include <QScreen>

#include "robotgoal.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"

class QFrame;
class QPaintEvent;

class CameraFrameWidget : public QWidget

{
    Q_OBJECT

public:
    CameraFrameWidget(QWidget *parent = 0);
    ~CameraFrameWidget();
    void draw(QPainter* painter, QPaintEvent event);
    void setSpeedWidget(QPainter* aPainter, int frameWidth);
    void setBatteryWidget(QPainter* aPainter, int frameWidth);
    cv::Mat getCameraFrame();

    void mousePressEvent(QMouseEvent *event);

    int updateCameraPicture;
    int actIndex=-1;
    QImage image;

    cv::Mat frame[3];
    cv::Mat replayFrame;
    cv::Mat dest;

protected:
    void paintEvent(QPaintEvent* event);

private:
    std::vector<RobotGoal>* points;
    double offset;
    QImage imageDistanceWarn;
    QImage imageOnline;
    QImage batteryImage;
    QImage speedImage;
    cv::VideoCapture cap;
    double v = 0.0;
    bool robotOnline = false;
    bool dispYellowWarning = false;
    bool dispOrangeWarning = false;
    bool dispRobotStopped = false;
    unsigned short batteryPercantage = 0;
    double tempSpeed = 0.0;
    unsigned short batteryLevel = 0;
    float scale = 1.0f;
    bool missionLoaded = false;
    bool canReplay = false;
    double robotX = 0.0;
    double robotY = 0.0;
    double robotTheta = 0.0;
    int frameHeight = 0;
    int frameWidth = 0;
    double yT = 0.0;
    double xT = 0.0;
    bool canPlacePoints = false;
    QColor pointColor;
    int pointType = 1;

    int rectangleWidthPx = 0;
    int rectangleHeightPx = 0;
    double alfa1 = 24.0;
    double alfa2 = 32.0;
    double d1 = 0.0;
    double d2 = 0.0;
    double cameraV = 17.5; //[cm]

    QScreen *screen;

    int scaleFactorWidth = 558;
    int scaleFactorHeight = 471;

public:
    void setRobotParams(double& x, double& y, double& theta);
    void setPointVector(std::vector<RobotGoal>* goals);
    void setTempSpeed(double newTempSpeed);
    void setV(double newV);
    void setBatteryLevel(const unsigned char newBatteryLevel);
    unsigned short getBatteryPercantage() const;
    void resetWarnings();
    void setDispYellowWarning(bool newDispYellowWarning);
    void setDispOrangeWarning(bool newDispOrangeWarning);
    void setRobotStoppedWarning(bool newWarning);
    void setRobotOnline(bool newRobotOnline);
    void setScale(float newScale);
    void setReplayFrame(const cv::Mat &newReplayFrame);
    bool getRobotOnline() const;
    void setBatteryPercantage(unsigned short newBatteryPercantage);
    void setCanReplay(bool newCanReplay);
    bool getCanPlacePoints() const;
    void setCanPlacePoints(bool newCanPlacePoints);
    void setPointColor(const QColor &newPointColor);
    void setPointType(int newPointType);
    void setScaleFactorWidth(int newScaleFactorWidth);
    void setScaleFactorHeight(int newScaleFactorHeight);
};

#endif // CAMERAFRAMEWIDGET_H
