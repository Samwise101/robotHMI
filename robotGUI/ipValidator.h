#ifndef IPVALIDATOR_H
#define IPVALIDATOR_H

#include <string>
#include <iostream>

class IpValidator
{
public:
    explicit IpValidator();
    ~IpValidator();

    void setIpAdress(std::string* ipAdress);

private:
    std::string* ipAdress;
};

#endif // IPVALIDATOR_H
