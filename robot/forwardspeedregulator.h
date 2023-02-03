#ifndef FORWARDSPEEDREGULATOR_H
#define FORWARDSPEEDREGULATOR_H


class forwardSpeedRegulator
{
public:
    forwardSpeedRegulator();
    ~forwardSpeedRegulator();
    float regulateForwardSpeed(int xGoal, int yGoal, float x, float y, float theta);
private:
    float Kp = 2.0;
    float eXDist;
    float eYDist;
    float theta;
};

#endif // FORWARDSPEEDREGULATOR_H
