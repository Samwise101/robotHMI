#ifndef CAMERAFRAMEWIDGET_H
#define CAMERAFRAMEWIDGET_H

#include <QWidget>
#include <QFrame>


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
    int updateCameraPicture;
    int actIndex;
    QImage image;

    QFrame* speedFrame;
    QFrame* batteryFrame;

    cv::Mat frame[3];

protected:
    void paintEvent(QPaintEvent* event);

private:
    double offset;
    QImage image2;
    float v;
    unsigned short batteryPercantage = 0;
    double tempSpeed = 0.0;
    unsigned short batteryLevel = 0;

public:

    void setTempSpeed(double newTempSpeed);
    void setV(float newV);
    void setBatteryLevel(const unsigned char newBatteryLevel);
    unsigned short getBatteryPercantage() const;
};

#endif // CAMERAFRAMEWIDGET_H
