#include "regulator.h"
#include <cmath>

Regulator::Regulator()
{
    KP = 10;
    KI = 5;
    P = I = D = 0;
}

Regulator::~Regulator()
{

}

float Regulator::getToGoalRegulator(int xGoal, int yGoal, int xRobot, int yRobot, int thetaRobot)
{
    //vzdialenost na osi x medzi robotom a cielom
    xDist = xGoal - xRobot;

    //vzdialenost na osi y medzi robotom a cielom
    yDist = yGoal - yRobot;

    //uhol medzi stredom robota a cielom
    theta = std::atan2(yDist, xDist);

    //rozdiel medzi uhlom robota a uhlom stredu robota a cielu
    eTheta = theta - thetaRobot;

    // rozdiel v rozmedzi od -pi/2 do pi/2
    eTheta = std::atan2(std::sin(eTheta), std::cos(eTheta));

    w = KP*eTheta;

    return w;
}
