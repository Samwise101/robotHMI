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
    int updateCameraPicture;
    int actIndex;
    //    cv::Mat frame[3];

    cv::Mat frame[3];

protected:
    void paintEvent(QPaintEvent* event);

private:
    double offset;

public:

};

#endif // CAMERAFRAMEWIDGET_H
