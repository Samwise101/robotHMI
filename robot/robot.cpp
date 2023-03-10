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
    closesocket(las_s);
    closesocket(rob_s);

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
     // std::cout << "Arc radius: " << radius << std::endl;
     // std::cout << "Arc speed: " << mmpersec << std::endl;
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
        if(initilize){
        if(readyFuture.wait_for(std::chrono::seconds(0))==std::future_status::ready)
            break;
        cap >> frameBuf;



      //  frameBuf.copyTo(robotPicture);
        std::async(std::launch::async, [this](cv::Mat camdata) { camera_callback(camdata.clone()); },frameBuf);
        cv::waitKey(1);
        }
    }
    cap.release();
}

double Robot::rampPosFunction(double speed)
{
    if(speed < tempSpeed){
        return speed+5;
    }
    return tempSpeed;
}

void Robot::robotOdometry(TKobukiData &output, bool useGyro)
{
    nlOld = nlCurr;
    nrOld = nrCurr;

    nlCurr = output.EncoderLeft;
    nrCurr = output.EncoderRight;

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

    if(useGyro){
        if(output.GyroAngle < 0){
            theta = (360 + output.GyroAngle/100)*PI/180;
        }
        else{
            theta = (output.GyroAngle/100)*PI/180;
        }
    }
    else{
        deltaTheta = (deltaSr - deltaSl)/(robot.getReferenceToB());
        theta = theta + deltaTheta;
    }

    if(deltaS * std::cos(theta) < -1000)
        xdt += 0;
    else
        xdt += deltaS * std::cos(theta);
    if(deltaS * std::sin(theta) < -1000)
        ydt += 0;
    else
        ydt += deltaS * std::sin(theta);

    x = x + xdt*100;
    y = y - ydt*100;

    if(xdt / 1000 != 0.0)
        xdt = 0.0;
    if(ydt / 1000 != 0.0)
        ydt = 0.0;

}

double Robot::orientationRegulator(int xGoal, int yGoal, bool robotRunning)
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
    else if((w > 0.0 && w <= 0.1) || (w < 0.0 && w >= -0.1)){
        w = 0.0;
    }

    //std::cout << "xDistToGoal=" << eXDist << ", yDistToGoal=" << eYDist << ", thetaToGoal=" << thetaToGoal << ", theta=" << theta;
    //std::cout << ", eThetaToGoal=" << eThetaToGoal << ", w=" << w << std::endl;
    return w;
}

double Robot::regulateForwardSpeed(int xGoal, int yGoal, bool robotRunning, int goalType)
{
    if(!robotRunning){
        if(v > 5.0){
           v = Kp2*v;
        }
        else{
            v = 0.0;
        }
        return v;
    }

    //vzdialenost na osi x medzi robotom a cielom v mm
    eXDist2 = (xGoal - x)*10;

    //vzdialenost na osi y medzi robotom a cielom v mm
    eYDist2 = -1*(yGoal - y)*10;

    eDist = std::sqrt(std::pow(eXDist2,2)+std::pow(eYDist2,2));

    if(eDist >= 0.0 && eDist <= 30.0 && goalType != 1){
        if(v > 5.0){
           v = Kp2*v;
        }
        else{
            v = 0.0;
        }
    }
    else if(v <= 20 && eDist >= 20 && eDist <= 50){
        if(v == 0){
            v = 5;
        }
        else{
            v += Kp2*v;
        }
    }
    else if(eDist > tempSpeed){
        v = rampPosFunction(v);
    }
    else if(goalType == 1){
        if(v >= 260)
            v = v - 10;
    }
    else{
        v = Kp2*eDist;
    }
    return v;
}

double Robot::avoidObstacleRegulator(double distToObst, double angleToObst)
{

    xDistObst = (x + distToObst/10*sin((2*PI-(angleToObst)+PI/2)+theta));
    yDistObst = (y + distToObst/10*cos((2*PI-(angleToObst)+PI/2)+theta));

    eXObst = (xDistObst - x)/100;
    eYObst = -1*(yDistObst - y)/100;

    thetaToObst = std::atan2(eYObst, eXObst);
    eThetaToObst = theta - thetaToObst;
    eThetaToObst = std::atan2(std::sin(eThetaToObst), std::cos(eThetaToObst));

    if(eThetaToObst >= 0.0 && eThetaToObst <= PI/2){
           w = eThetaToObst + PI/2;
    }
    else if(eThetaToObst >= -PI/2 && eThetaToObst < 0.0){
           w = eThetaToObst - PI/2;
    }
    else{
        w = 0.0;
    }

    if(w > 2.0){
        w = 2.0;
    }
    else if(w < -2.0){
        w = -2.0;
    }

    return Kp3*w;
}

double Robot::getDistanceToGoal(int xGoal, int yGoal)
{
    return std::sqrt(std::pow((xGoal - x)*10, 2) + std::pow((yGoal - y)*10, 2));
}

double Robot::robotFullTurn(float goalAngle)
{
    eToGoalAngle = goalAngle - theta;
    if((eToGoalAngle > -0.15 && eToGoalAngle < 0.0) || (eToGoalAngle > 0.0 && eToGoalAngle < 0.15)){
        w = 0.0;
    }
    else{
        w = KpTurn*eToGoalAngle;
    }
    return w;
}

void Robot::robotAtGoal()
{
    if(w == 0.0 && v == 0.0){
        atGoal = true;
    }
    else{
        atGoal = false;
    }
}


void Robot::setRobotPose(int xPos, int yPos, float orientation)
{
    x = xPos;
    y = yPos;
    theta = orientation;
    std::cout << "Setting pose: x=" << x << ", y=" << y << ", theta=" << theta <<std::endl;
}

void Robot::resetRobotPose()
{
    x = 0;
    y = 0;
    theta = 0.0;
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

double& Robot::getTheta()
{
    return theta;
}

double& Robot::getDeltaTheta()
{
    return deltaTheta;
}

double& Robot::getYdt()
{
    return ydt;
}

double& Robot::getXdt()
{
    return xdt;
}

double& Robot::getX()
{
    return x;
}

double& Robot::getY()
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

double Robot::getW() const
{
    return w;
}

double Robot::getV() const
{
    return v;
}

bool Robot::getAtGoal() const
{
    return atGoal;
}

void Robot::setAtGoal(bool newAtGoal)
{
    atGoal = newAtGoal;
}

double Robot::getTempSpeed() const
{
    return tempSpeed;
}

void Robot::setRobotStarting(bool newRobotStarting)
{
    robotStarting = newRobotStarting;
}
