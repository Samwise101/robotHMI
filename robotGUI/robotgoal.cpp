#include "robotgoal.h"
#include <QColor>

RobotGoal::RobotGoal(int xpos, int ypos, int type, QColor color):QPoint(xpos, ypos)
{
    this->color = color;
    this->reached = false;
    this->type = type;
    this->checked = false;
}


RobotGoal::~RobotGoal()
{

}

const QColor &RobotGoal::getColor() const
{
    return color;
}

int RobotGoal::getType() const
{
    return type;
}

bool RobotGoal::getChecked() const
{
    return checked;
}

void RobotGoal::setChecked(bool newChecked)
{
    checked = newChecked;
}
