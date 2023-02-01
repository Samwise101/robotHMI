#ifndef REGULATOR_H
#define REGULATOR_H


class Regulator
{

public:
    Regulator();
    ~Regulator();
    float getToGoalRegulator(int xGoal, int yGoal, int xRobot, int yRobot, int thetaRobot);

private:
    double SP;
    double PV;
    double e;
    float w;
    double KP;
    double KI;
    double out;

    int xDist;
    int yDist;
    float theta;
    float eTheta;

    double P;
    double I;
    double D;

    int dir;
};

#endif // REGULATOR_H
