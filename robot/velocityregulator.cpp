#include "velocityregulator.h"

VelocityRegulator::VelocityRegulator()
{
    setPoint = 15;
    propGain = 10;
    integGain = 5;
    P = I = D = 0;
}

VelocityRegulator::~VelocityRegulator()
{

}

double VelocityRegulator::regulateVelocity(double procesValue)
{
    e = setPoint - procesValue;
    P = propGain * e;
    I = integGain * e;
    u = P + I + D;

    return u;
}
