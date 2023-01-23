#include "velocityregulator.h"

VelocityRegulator::VelocityRegulator():currentVelocity(0),e(0),u(0)
{
    setPoint = 15;
    propGain = 10;
    integGain = 5;
}

VelocityRegulator::~VelocityRegulator()
{

}

void VelocityRegulator::regulateVelocity()
{
    e = setPoint - currentVelocity;
    u = propGain * e;
}
