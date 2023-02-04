#include "robot.h"


#ifdef _WIN32
#include<windows.h>

#else
#include <termios.h>
#include <unistd.h>
#include "unistd.h"
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#endif

std::function<int(TKobukiData)> Robot::do_nothing_robot=[](TKobukiData data){std::cout<<"data z kobuki "<<std::endl; return 0;};
std::function<int(LaserMeasurement)> Robot::do_nothing_laser=[](LaserMeasurement data){std::cout<<"data z rplidar "<<std::endl; return 0;};

Robot::~Robot()
{
    ready_promise.set_value();
    robotthreadHandle.join();
    laserthreadHandle.join();
    camerathreadhandle.join();
#ifdef _WIN32
WSACleanup();
#endif;
}

Robot::Robot(std::string ipaddressRobot, std::string ipaddressLaser,int laserportRobot, int laserportMe,std::function<int(LaserMeasurement)> &lascallback,int robotportRobot, int robotportMe,std::function<int(TKobukiData)> &robcallback): wasLaserSet(0),wasRobotSet(0),wasCameraSet(0)
{
    setLaserParameters(ipaddressLaser,laserportRobot,laserportMe,lascallback);
    setRobotParameters(ipaddressRobot,robotportRobot,robotportMe,robcallback);
    readyFuture=ready_promise.get_future();
}
void Robot::robotprocess()
{
#ifdef _WIN32
    WSADATA wsaData = {0};
    int iResult = 0;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
#endif
    rob_slen = sizeof(las_si_other);
    if ((rob_s=::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {

    }

    char rob_broadcastene=1;
    DWORD timeout=100;

    ::setsockopt(rob_s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof timeout);
    ::setsockopt(rob_s,SOL_SOCKET,SO_BROADCAST,&rob_broadcastene,sizeof(rob_broadcastene));
    // zero out the structure
    memset((char *) &rob_si_me, 0, sizeof(rob_si_me));

    rob_si_me.sin_family = AF_INET;
    rob_si_me.sin_port = htons(robot_ip_portOut);
    rob_si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    rob_si_posli.sin_family = AF_INET;
    rob_si_posli.sin_port = htons(robot_ip_portIn);
    rob_si_posli.sin_addr.s_addr =inet_addr(robot_ipaddress.data());//inet_addr("10.0.0.1");// htonl(INADDR_BROADCAST);
    rob_slen = sizeof(rob_si_me);
    ::bind(rob_s , (struct sockaddr*)&rob_si_me, sizeof(rob_si_me) );

    std::vector<unsigned char> mess=robot.setDefaultPID();
    if (::sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
#ifdef _WIN32
    Sleep(100);
#else
    usleep(100*1000);
#endif
    mess=robot.setSound(440,1000);
    if (::sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
    unsigned char buff[50000];
    while(1)
    {

        if(readyFuture.wait_for(std::chrono::seconds(0))==std::future_status::ready)
            break;
        memset(buff,0,50000*sizeof(char));
        if ((rob_recv_len = ::recvfrom(rob_s, (char*)&buff, sizeof(char)*50000, 0, (struct sockaddr *) &rob_si_other, &rob_slen)) == -1)
        {

            continue;
        }
        //tu mame data..zavolame si funkciu

        //     memcpy(&sens,buff,sizeof(sens));
        struct timespec t;
        //      clock_gettime(CLOCK_REALTIME,&t);

        int returnval=robot.fillData(sens,(unsigned char*)buff);
        if(returnval==0)
        {
            //     memcpy(&sens,buff,sizeof(sens));

            std::chrono::steady_clock::time_point timestampf=std::chrono::steady_clock::now();





            ///---toto je callback funkcia...
         //   robot_callback(sens);
            std::async(std::launch::async, [this](TKobukiData sensdata) { robot_callback(sensdata); },sens);

        }


    }

    std::cout<<"koniec thread2"<<std::endl;
}


void Robot::setTranslationSpeed(int mmpersec)
{
    std::vector<unsigned char> mess=robot.setTranslationSpeed(mmpersec);
    if (::sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
}

void Robot::setRotationSpeed(double radpersec) //left
{

    std::vector<unsigned char> mess=robot.setRotationSpeed(radpersec);
    if (::sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
    {

    }
}

void Robot::setArcSpeed(int mmpersec,int radius)
{
     std::cout << "Arc radius: " << radius << std::endl;
     std::cout << "Arc speed: " << mmpersec << std::endl;
     std::vector<unsigned char> mess=robot.setArcSpeed(mmpersec,radius);
     if (::sendto(rob_s, (char*)mess.data(), sizeof(char)*mess.size(), 0, (struct sockaddr*) &rob_si_posli, rob_slen) == -1)
     {

     }
 }
void Robot::laserprocess()
{
#ifdef _WIN32
    WSADATA wsaData = {0};
    int iResult = 0;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
#endif
    las_slen = sizeof(las_si_other);
    if ((las_s=::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {

    }

    char las_broadcastene=1;
#ifdef _WIN32
    DWORD timeout=100;

    ::setsockopt(las_s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof timeout);
    ::setsockopt(las_s,SOL_SOCKET,SO_BROADCAST,&las_broadcastene,sizeof(las_broadcastene));
#else
    ::setsockopt(las_s,SOL_SOCKET,SO_BROADCAST,&las_broadcastene,sizeof(las_broadcastene));
#endif
    // zero out the structure
    memset((char *) &las_si_me, 0, sizeof(las_si_me));

    las_si_me.sin_family = AF_INET;
    las_si_me.sin_port = htons(laser_ip_portOut);
    las_si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    las_si_posli.sin_family = AF_INET;
    las_si_posli.sin_port = htons(laser_ip_portIn);
    las_si_posli.sin_addr.s_addr = inet_addr(laser_ipaddress.data());;//htonl(INADDR_BROADCAST);
    ::bind(las_s , (struct sockaddr*)&las_si_me, sizeof(las_si_me) );
    char command=0x00;
    if (::sendto(las_s, &command, sizeof(command), 0, (struct sockaddr*) &las_si_posli, rob_slen) == -1)
    {

    }
    LaserMeasurement measure;
    while(1)
    {

        if(readyFuture.wait_for(std::chrono::seconds(0))==std::future_status::ready)
            break;
        if ((las_recv_len = ::recvfrom(las_s, (char *)&measure.Data, sizeof(LaserData)*1000, 0, (struct sockaddr *) &las_si_other, &las_slen)) == -1)
        {

            continue;
        }
        measure.numberOfScans=las_recv_len/sizeof(LaserData);
        //tu mame data..zavolame si funkciu

        std::async(std::launch::async, [this](LaserMeasurement sensdata) { laser_callback(sensdata); },measure);
        //     memcpy(&sens,buff,sizeof(sens));
      //  int returnValue=autonomouslaser(measure);

    }
    std::cout<<"koniec thread"<<std::endl;
}


void Robot::robotStart()
{
    std::cout << "Robot ip adress: " << robot_ipaddress << std::endl;
    if(wasRobotSet==1)
    {
        std::function<void(void)> f =std::bind(&Robot::robotprocess,this);
        robotthreadHandle=std::move(std::thread(f));
    }
    if(wasLaserSet==1)
    {
        std::function<void(void)> f2 =std::bind(&Robot::laserprocess, this);
        laserthreadHandle=std::move(std::thread(f2));
    }
    if(wasCameraSet==1)
    {
        std::function<void(void)> f3 =std::bind(&Robot::imageViewer, this);
        camerathreadhandle=std::move(std::thread(f3));
    }

}

int Robot::getWasRobotSet(){
    return wasRobotSet;
}

void Robot::imageViewer()
{
    cv::VideoCapture cap;
    cap.open(camera_link);
    cv::Mat frameBuf;
    while(1)
    {
        if(readyFuture.wait_for(std::chrono::seconds(0))==std::future_status::ready)
            break;
        cap >> frameBuf;



      //  frameBuf.copyTo(robotPicture);
        std::async(std::launch::async, [this](cv::Mat camdata) { camera_callback(camdata.clone()); },frameBuf);
        cv::waitKey(1);

    }
    cap.release();
}

float Robot::rampPosFunction(float speed)
{
    if(speed < tempSpeed){
        return speed+30;
    }
    return tempSpeed;
}

void Robot::callbackAcc(int dir, double& mmpersec, double& radpersec){
    if(std::abs(mmpersec) < tempSpeed){
        mmpersec = rampPosFunction(std::abs(mmpersec));
    }
    else{
        mmpersec = tempSpeed;
    }
}

double Robot::rampNegFunction(double speed)
{
    if(speed > 0 && (speed-30) > 0){
        return speed-30;
    }
    else
        speed = 0;
    if(speed < 0 && (speed+30) < 0){
        return speed+30;
    }
    else
        speed = 0;
    return 0.0;
}

void Robot::callbackBreak(double& mmpersec, double& radpersec){
    if(std::abs(mmpersec) > 0){
        mmpersec = rampNegFunction(mmpersec);
        std::cout << mmpersec << std::endl;;
    }
    else if(std::abs(radpersec) > 0){
        radpersec = rampNegFunction(radpersec);
        std::cout << mmpersec << std::endl;;
    }
    else{
        //std::cout << "Robot is not moving" << std::endl;
    }
}

void Robot::robotOdometry(TKobukiData &output)
{
    nlOld = nlCurr;
    nrOld = nrCurr;

    // max encoder = 65 536 => 2^16

    nlCurr = output.EncoderLeft;
    nrCurr = output.EncoderRight;

//    std::cout << "nlOld=" << nlOld << "; nrOld=" << nrOld << std::endl;
//    std::cout << "nlCurr=" << nlCurr << "; nrCurr=" << nrCurr << std::endl;

    if((nlOld - nlCurr) < -(UINT16_MAX/2)){
        nlDiff = (nlCurr - nlOld) - UINT16_MAX;
    }
    else if((nlOld - nlCurr) > (UINT16_MAX/2)){
        nlDiff = UINT16_MAX - nlOld + nlCurr;
    }
    else{
        nlDiff = nlCurr - nlOld;
    }

    if((nrOld - nrCurr) < -(UINT16_MAX/2)){
        nrDiff = (nrCurr - nrOld) - UINT16_MAX;
    }
    else if((nrOld - nrCurr) > (UINT16_MAX/2)){
        nrDiff = UINT16_MAX - nrOld + nrCurr;
    }
    else{
        nrDiff = nrCurr - nrOld;
    }

    deltaSl = nlDiff*robot.getReferenceToTickToMeter();
    deltaSr = nrDiff*robot.getReferenceToTickToMeter();
    deltaS = (deltaSr + deltaSl)/2;

    deltaTheta = (deltaSr - deltaSl)/(robot.getReferenceToB());
    theta = theta + deltaTheta;


    if(deltaS * std::cos(theta) < -1000)
        xdt += 0;
    else
        xdt += deltaS * std::cos(theta);
    if(deltaS * std::sin(theta) < -1000)
        ydt += 0;
    else
        ydt += deltaS * std::sin(theta);

    //std::cout  << "deltaS=" << deltaS << "xdt=" << xdt << ", ydt=" << ydt << std::endl;

    x = x + xdt*100;
    y = y - ydt*100;

    xReal = xReal + xdt;
    yReal = yReal - ydt;

    if(xdt / 1000 != 0.0)
        xdt = 0.0;
    if(ydt / 1000 != 0.0)
        ydt = 0.0;

    std::cout << "New pose: x=" << x << ", y=" << y << ", theta=" << theta << std::endl;
}

float Robot::orientationRegulator(int xGoal, int yGoal, bool robotRunning)
{
    if(!robotRunning){
        if(w >= 0.5 || w <= -0.5){
           w = 0.5 * w;
        }
        else{
            w = 0;
        }
        return w;
    }

    //vzdialenost na osi x medzi robotom a cielom v m
    eXDist = (xGoal - x)/100;

    //vzdialenost na osi y medzi robotom a cielom v m
    eYDist = -1*(yGoal - y)/100;

    if(std::abs(eYDist) <= 0.05 && std::abs(eXDist) <= 0.05){
        std::cout << "Goal reached -w !" << std::endl;
        w = 0.0;
        return w;
    }

    //uhol medzi stredom robota a cielom
    thetaToGoal = std::atan2(eYDist, eXDist);

    //rozdiel medzi uhlom robota a uhlom stredu robota a cielu
    eThetaToGoal = thetaToGoal - theta;

    // rozdiel v rozmedzi od -pi/2 do pi/2
    eThetaToGoal = std::atan2(std::sin(eThetaToGoal), std::cos(eThetaToGoal));

    w = Kp*eThetaToGoal;

    if(w >= 2.0){
        w = 2.0;
    }
    else if(w <= -2.0){
        w = -2.0;
    }
    else if((w > 0.0 && w <= 0.05) || (w < 0.0 && w >= -0.05)){
        w = 0.0;
    }

    std::cout << "xDistToGoal=" << eXDist << ", yDistToGoal=" << eYDist << ", thetaToGoal=" << thetaToGoal << ", theta=" << theta;
    std::cout << ", eThetaToGoal=" << eThetaToGoal << ", w=" << w << std::endl;
    return w;
}

float Robot::regulateForwardSpeed(int xGoal, int yGoal, bool robotRunning)
{
    if(!robotRunning){
        if(v > 5){
           v = Kp2*v;
        }
        else{
            v = 0;
        }
        return v;
    }

    //vzdialenost na osi x medzi robotom a cielom v mm
    eXDist2 = (xGoal - x)*10;

    //vzdialenost na osi y medzi robotom a cielom v mm
    eYDist2 = -1*(yGoal - y)*10;

    eDist = std::sqrt(std::pow(eXDist2,2)+std::pow(eYDist2,2));

    if(eDist >= 0 && eDist <= 30){
        std::cout << "Goal reached -v !" << std::endl;
        v = 0.0;
        return v;
    }

    if(Kp2*eDist > 200){
        v = rampPosFunction(v);
    }
    else{
        v = Kp2*eDist;
    }
    std::cout << "xDist2ToGoal=" << eXDist2 << ", yDist2ToGoal=" << eYDist2 << ", eDistToGoal=" << eDist;
    std::cout << "v=" << v << std::endl;
    return v;
}


void Robot::setRobotPose(int xPos, int yPos, float orientation)
{
    x = xPos;
    y = yPos;
    theta = orientation;
    xReal = 0.0;
    yReal = 0.0;
    std::cout << "Setting pose: x=" << x << ", y=" << y << ", theta=" << theta <<std::endl;
}

void Robot::resetRobotPose()
{
    x = 0;
    y = 0;
    theta = 0.0;
    xReal = 0.0;
    yReal = 0.0;
}

bool Robot::emergencyStop(int dist)
{

    if(dist >= 0 && dist < 35){  // [m]
        std::cout << "Robot emergency stop called" << std::endl;
        return true;
    }
    return false;
}

//---------------------------------gettre----------------------------------------//

double Robot::getDeltaSl()
{
    return deltaSl;
}

double Robot::getDeltaSr()
{
    return deltaSr;
}


double Robot::getDeltaS()
{
    return deltaS;
}

float& Robot::getTheta()
{
    return theta;
}

float& Robot::getDeltaTheta()
{
    return deltaTheta;
}

float& Robot::getYdt()
{
    return ydt;
}

float& Robot::getXdt()
{
    return xdt;
}

float& Robot::getX()
{
    return x;
}

float& Robot::getY()
{
    return y;
}

bool Robot::getInitilize() const
{
    return initilize;
}

void Robot::setInitilize(bool newInitilize)
{
    initilize = newInitilize;
}

float Robot::getW() const
{
    return w;
}

float Robot::getV() const
{
    return v;
}

