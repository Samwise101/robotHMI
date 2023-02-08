#ifndef ROBOTGOAL_H
#define ROBOTGOAL_H

#include <QWidget>
#include <QPoint>
#include <QColor>
#include <QObject>
#include <QColor>

class RobotGoal : public QPoint
{

public:
    RobotGoal(int xpos, int ypos, int type, QColor color);
    ~RobotGoal();

    const QColor &getColor() const;
    void setColor(const QColor &newColor);

    int getType() const;

private:
    QColor color;
    bool reached;
    int type;
};

#endif // ROBOTGOAL_H
