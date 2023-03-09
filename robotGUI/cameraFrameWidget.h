#ifndef CAMERAFRAMEWIDGET_H
#define CAMERAFRAMEWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QTimer>


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
    void setSpeedWidget();
    void setBatteryWidget();
    cv::Mat getCameraFrame();
    int updateCameraPicture;
    int actIndex=-1;
    QImage image;

    QFrame* speedFrame;
    QFrame* batteryFrame;

    cv::Mat frame[3];
    cv::Mat replayFrame;
    cv::Mat dest;

protected:
    void paintEvent(QPaintEvent* event);

private:
    double offset;
    QImage imageDistanceWarn;
    QImage imageOnline;
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

public:

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
};

#endif // CAMERAFRAMEWIDGET_H
