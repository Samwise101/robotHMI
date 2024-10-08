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

    bool toggleMap();
    bool toggleMouse();

    void clearVectors();

    void paintMap(QPainter* aPainter);
    void paintMouseCoord(QRect& rectangle, QPainter* aPainter);

    LaserMeasurement copyOfLaserData;
    int updateLaserPicture;

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
    void initializeRobot();
    void parseMapFile();

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

    std::vector<RobotGoal>* getRobotGoals();

    void setShowReplayWarning(bool newShowReplayWarning);

    bool getShowReplayWarning() const;

    double getRealTheta() const;

    void setRobotInitialized(bool newRobotInitialized);

    void setIsSimulation(bool newIsSimulation);

    void setReplayIndex(int newReplayIndex);

    bool getReplayFinished() const;
    void setReplayFinished(bool newReplayFinished);

    void setShowDisconnectWarning(bool newShowDisconnectWarning);

    bool getShowDisconnectWarning() const;

    void pushBackTajcPoint(double x, double y);

    void setShowRobotStopped(bool newShowRobotStopped);

    bool getShowRobotStopped() const;

    void setRobotControlOn(bool newRobotControlOn);

    void insertToGoals(int x, int y, int type, QColor color);

    const std::vector<QPoint> &getTrajcPoints() const;

    std::vector<QPoint>* getTrajectories();

    void setScaleFactorWidth(int newScaleFactorWidth);

    void setScaleFactorHeight(int newScaleFactorHeight);

protected:
    void paintEvent(QPaintEvent* event);

private:
    QPoint robotImagePos;
    std::vector<QPoint> trajcPoints;
    double imageTheta = 0.0;

    bool robotInitialized = false;
    bool robotControlOn = true;

    std::vector<std::vector<int>>robotReplayPos;
    std::vector<int>robotPos;
    std::vector<double>robotAngle;
    std::vector<std::vector<QPoint>>lidarReplayPos;
    std::vector<QPoint>lidarReplayPoints;
    std::vector<std::vector<QLine>>robotTrajectory;
    std::vector<QLine>replayTrajectory;
    std::vector<std::vector<int>> missionReplayPoints;
    std::vector<int> missionPoints;
    int replayIndex;

    std::vector<QPoint> trajectories;

    bool replayFinished = false;

    int robotXPos;
    int robotYPos;

    bool showReplayWarning = false;
    bool showDisconnectWarning = false;
    bool showRobotStopped = false;
    bool showMap = true;
    bool posMouseTrack = true;
    bool isSimulation = false;

    int mouseXPos = -1;
    int mouseYPos = -1;

    float mouseToMapX = 0.f;
    float mouseToMapY = 0.f;

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
    QPoint startLocation;

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

    int xp2 = 0;
    int yp2 = 0;
    int number = 0;
    int number2 = 0;

    int sectionsX = 0;
    int sectionsY = 0;

    double shortestLidarDistance = 10000.0;
    double shortestLidarAngle = 0.0;
    double offset = 0.0;
    bool canTriggerEvents = false;

    QScreen *screen;

    int scaleFactorWidth = 1148;
    int scaleFactorHeight = 942;
};

#endif // MAPFRAMEWIDGET_H
