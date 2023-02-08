#ifndef ROBOTGOAL_H
#define ROBOTGOAL_H

#include <QWidget>
#include <QPoint>
#include <QColor>
#include <QObject>

class RobotGoal : public QPoint
{

public:
    RobotGoal();
    ~RobotGoal();
    QColor color;

    bool reached;
private:


};

#endif // ROBOTGOAL_H
