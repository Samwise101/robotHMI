#include "robotgoal.h"
#include <QColor>

RobotGoal::RobotGoal(int xpos, int ypos, int type, QColor color):QPoint(xpos, ypos)
{
    this->color = color;
    this->reached = false;
    this->type = type;
}


RobotGoal::~RobotGoal()
{

}

const QColor &RobotGoal::getColor() const
{
    return color;
}

void RobotGoal::setColor(const QColor &newColor)
{
    color = newColor;
}

int RobotGoal::getType() const
{
    return type;
}
