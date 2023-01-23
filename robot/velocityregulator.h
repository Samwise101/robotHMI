#ifndef VELOCITYREGULATOR_H
#define VELOCITYREGULATOR_H


class VelocityRegulator
{
public:
    VelocityRegulator();
    ~VelocityRegulator();
    void regulateVelocity();

private:
    double setPoint;
    double currentVelocity;
    double e;
    double u;

    double propGain;
    double integGain;

    int dir;
};

#endif // VELOCITYREGULATOR_H
