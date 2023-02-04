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
private:

    bool reached;
};

#endif // ROBOTGOAL_H
