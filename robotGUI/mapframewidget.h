#ifndef MAPFRAMEWIDGET_H
#define MAPFRAMEWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QMouseEvent>
#include <QEvent>
#include <vector>
#include <fstream>
#include <cstring>
#include <QVector>

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
    void mouseMoveEvent(QMouseEvent *event);
    bool event(QEvent * event);
    void hoverLeave(QHoverEvent *event);

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

    void createFrameLog(fstream& file);

    int imageHeight = 0;
    int imageWidth = 0;

    void setRobotOnline(bool newRobotOnline);

    const std::string &getStr() const;

    void setStr(const std::string &newStr);

    void setNumber2(int newNumber2);

    void setPosMouseTrack(bool newPosMouseTrack);

    bool getPosMouseTrack() const;

    void setShowReplayWarning(bool newShowReplayWarning);

    bool getShowReplayWarning() const;

protected:
    void paintEvent(QPaintEvent* event);

private:
    QPoint robotImagePos;
    double imageTheta = 0.0;

    int robotXPos;
    int robotYPos;

    bool showReplayWarning = false;

    int mouseXPos = -1;
    int mouseYPos = -1;
    bool posMouseTrack;
    QString mouseString;

    std::vector<QPoint> robotPositionInTime;
    std::string str;
    std::string token;
    std::string temp1;
    std::string temp2;
    std::string temp3;
    size_t pos;

    int rectMiddleX = 0;
    int rectMiddleY = 0;

    QVector<QLine> lines;
    QLine line1;
    QLine line2;
    QLine line3;
    QLine line4;
    std::vector<RobotGoal> points;
    int pointType;
    QColor pointColor;

    bool robotOnline;

    bool placeGoals;
    float scale = 1.0f;

    double lidarDist = 0.0;
    double lidarDistImage = 0.0;

    int xp = 0;
    int yp = 0;
    int goalColor = 1;

    double realTheta = 0.0;

    int capFreq = 0;

    int xp2 = 0;
    int yp2 = 0;
    int number = 0;
    int number2 = 0;

    int sectionsX = 0;
    int sectionsY = 0;

    double shortestLidarDistance = 10000.0;
    double shortestLidarAngle = 0.0;
    double offset;
    bool canTriggerEvents;
};

#endif // MAPFRAMEWIDGET_H
