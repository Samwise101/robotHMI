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
#include <robotgoal.h>

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
    double getShortestDistanceLidar();
    double getShortestDistanceLidarAngle();

    LaserMeasurement copyOfLaserData;
    int updateLaserPicture;

    QPoint robotPosition;
    //RobotGoal robotGoal;

    QRect* rectangle;

    bool robotInitialized = false;

    void mousePressEvent(QMouseEvent *event);

    void updateRobotValuesForGUI(float& x, float& y, float& theta);

    void setCanTriggerEvent(bool state);
    bool isGoalVectorEmpty();
    int getGoalYPosition();
    int getGoalXPosition();
    int getGoalVectorSize();
    void removeAllPoints();
    bool removeLastPoint();

protected:
    void paintEvent(QPaintEvent* event);

private:
    std::vector<QPoint> points;
    double lidarDist = 0;
    int xp = 0;
    int yp = 0;
    int xpshort;
    int ypshort;
    float realTheta;
    double shortestLidarDistance = 10000.0;
    double shortestLidarAngle = 0.0;
    double offset;
    bool canTriggerEvents;
};

#endif // MAPFRAMEWIDGET_H
