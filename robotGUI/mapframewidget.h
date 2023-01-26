#ifndef MAPFRAMEWIDGET_H
#define MAPFRAMEWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QMouseEvent>
#include <QEvent>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgcodecs.hpp"

#include "robot.h"


class QFrame;
class QPaintEvent;

class MapFrameWidget : public QWidget
{
    Q_OBJECT
public:
    MapFrameWidget(QWidget *parent = 0);
    ~MapFrameWidget();
    void draw(QPainter* painter, QPaintEvent event);
    double getDistanceToFirstPoint();
    int getShortestDistanceLidar();

    LaserMeasurement copyOfLaserData;
    int updateLaserPicture;
    std::vector<QPoint> points;
    QPoint middle;

    int distance;
    int shortestLidarDistance = 30;
    int angle = 30;
    double stepY = 0;
    double stepX = 0;

    std::vector<QPoint> samples;

    void mousePressEvent(QMouseEvent *event);

    void setCanTriggerEvent(bool state);
    void setDistance(int s);
    std::vector<QPoint> getPoints();

protected:
    void paintEvent(QPaintEvent* event);

private:
    double offset;
    bool canTriggerEvents;
};

#endif // MAPFRAMEWIDGET_H
