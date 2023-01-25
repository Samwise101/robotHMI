#ifndef VELOCITYREGULATOR_H
#define VELOCITYREGULATOR_H


class VelocityRegulator
{
public:
    VelocityRegulator();
    ~VelocityRegulator();
    double regulateVelocity(double procesValue);

private:
    double setPoint;
    double currentVelocity;
    double e;
    double u;
    double propGain;
    double integGain;
    double out;

    double P;
    double I;
    double D;

    int dir;
};

#endif // VELOCITYREGULATOR_H
