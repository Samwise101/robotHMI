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
#include "robotgoal.h"

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
    double getLidarAngle();

    LaserMeasurement copyOfLaserData;
    int updateLaserPicture;

    bool robotInitialized = false;

    void mousePressEvent(QMouseEvent *event);

    void updateRobotValuesForGUI(double& x, double& y, double& theta);

    void setCanTriggerEvent(bool state);
    bool isGoalVectorEmpty();
    int getGoalYPosition();
    int getGoalXPosition();
    int getGoalVectorSize();
    int getGoalType();
    void removeAllPoints();
    bool removeLastPoint();

    void setOffset(double newOffset);

    void setPointColor(const QColor &newPointColor);

    void setPointType(int newPointType);

    void setPlaceGoals(bool newPlaceGoals);

    void setScale(float newScale);

    QPoint robotPosition;

    QImage createImage();

    int imageHeight = 0;
    int imageWidth = 0;


protected:
    void paintEvent(QPaintEvent* event);

private:
    QPoint robotImagePos;

    std::vector<QPoint> robotPositionInTime;

    std::vector<RobotGoal> points;
    int pointType;
    QColor pointColor;

    bool placeGoals;
    float scale = 1.0f;

    double lidarDist = 0;
    double lidarDistImage = 0.0;

    int xp = 0;
    int yp = 0;
    double realTheta = 0.0;

    int capFreq = 0;

    int xp2 = 0;
    int yp2 = 0;

    int sectionsX = 0;
    int sectionsY = 0;

    double shortestLidarDistance = 10000.0;
    double shortestLidarAngle = 0.0;
    double offset;
    bool canTriggerEvents;
};

#endif // MAPFRAMEWIDGET_H
