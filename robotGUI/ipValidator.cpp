#include "ipValidator.h"

IpValidator::IpValidator()
{
    *ipAdress = "127.0.0.1";
    std::cout << "Ip:" << *ipAdress << std::endl;
}

void IpValidator::setIpAdress(std::string *ipAdress)
{
    this->ipAdress = ipAdress;
}
