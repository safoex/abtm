//
// Created by hamed on 28.06.19.
//

#ifndef HELLO_ICUB_ICUBPART_H
#define HELLO_ICUB_ICUBPART_H

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>
#include <iostream>

using namespace std;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

class iCubPart {

public:
    string deviceName;
    string remotePort;
    string localPort;

    Vector positions;
    Vector encoders;
    Vector accelerations;
    Vector speeds;

public:
    PolyDriver* interfaceDevice;
    Property options;
    IPositionControl *pos;
    IEncoders *encs;
    int numberOfJoints=0;

public:
    iCubPart(std::string deviceName,std::string remotePort){
        this->remotePort = remotePort;
        this->deviceName = deviceName;


        localPort = "/iCubRobot" + this->remotePort;

        options.put("device", "remote_controlboard");
        options.put("local", localPort);   //local port names
        options.put("remote", remotePort); //where we connect to

        interfaceDevice = new PolyDriver(options);

        if (!interfaceDevice->isValid()) {
            printf("Device not available.  Here are the known devices:\n");
            printf("%s", Drivers::factory().toString().c_str());
        }

        bool ok;
        ok = interfaceDevice->view(pos);
        ok = ok && interfaceDevice->view(encs);


        if (!ok) {
            printf("Problems acquiring interfaces\n");
        }

        // Getting the number of axis (joints)
        pos->getAxes(&numberOfJoints);

        // Resize matrices according to number of joints or axis
        encoders.resize(numberOfJoints);
        accelerations.resize(numberOfJoints);
        positions.resize(numberOfJoints);
        speeds.resize(numberOfJoints);


        // Initialize all accelerations to default value

        for (int i = 0; i < numberOfJoints; i++) {
            accelerations[i] = 50.0;
        }
        applyAccelerations();


        // Initialize all speeds to default values

        for (int i = 0; i < numberOfJoints; i++) {
            speeds[i] = 10.0;
            pos->setRefSpeed(i, speeds[i]);
        }


        cout << this->deviceName  << " > waiting for encoders";

        while(!encs->getEncoders(encoders.data()))
        {
            Time::delay(0.1);
            printf(".");
        }
        printf("\n;");


        // Initialize all Positions to default values

        for (int i = 0; i < numberOfJoints; i++) {
            positions[i] = 0;
        }
        //applyPositions();

        cout << deviceName <<" Initialization finished. " << "Number of Joints : " << numberOfJoints << endl;
    }

    void keepCurrentJointPositions(){
        if( updateJointEncoders() == true){
            for (int i = 0; i < numberOfJoints; i++) {
                positions[i] = encoders[i];
            }
            cout << deviceName << " Current Joint Positions Saved!" << endl ;
        } else
        {
            cout << "Error in KeepCurrentJointPositions" << endl ;
        }
    }
    bool updateJointEncoders(){
        bool ret=encs->getEncoders(encoders.data());
        if (!ret)
        {
            fprintf(stderr, "Error receiving encoders, check connectivity with the robot\n");
        }
        return ret;
    }


    // -------------- Set Variables -------------------

    void setJointPosition(int jointIndex, double value){
        if(jointIndex <= numberOfJoints){
            positions[jointIndex] = value;
        }
    }
    void setJointAcceleration(int jointIndex, double value){
        if(jointIndex <= numberOfJoints){
            accelerations[jointIndex] = value;
        }
    }
    void setJointSpeed(int jointIndex, double value){
        if(jointIndex <= numberOfJoints){
            speeds[jointIndex] = value;
        }
    }


    // --------------- Get Variables ------------------

    double getJointPostion(int jointIndex){
        if(jointIndex <= numberOfJoints){
            return positions[jointIndex];
        }

        return -1;
    }


    double getJointEncoder(int jointIndex){
        if(jointIndex <= numberOfJoints){
            updateJointEncoders();
            return encoders[jointIndex];
        }

        return -1;
    }

    double getJointSpeed(int jointIndex){
        if(jointIndex <= numberOfJoints){
            return speeds[jointIndex];
        }

        return -1;
    }

    double getJointAcceleration(int jointIndex){
        if(jointIndex <= numberOfJoints){
            return accelerations[jointIndex];
        }

        return -1;
    }


    // ------------------------ Action Commands -------------------------
    void applySpeeds(){
        for (int i = 0; i < numberOfJoints; i++) {
            speeds[i] = 10.0;
            pos->setRefSpeed(i, speeds[i]);
        }
    }

    void applyPositions(){
        pos->positionMove(positions.data());
    }

    void applyAccelerations(){
        pos->setRefAccelerations(accelerations.data());
    }
    void applyAll(){
        //applySpeeds();
        applyPositions();
        applyAccelerations();

    }
};


#endif //HELLO_ICUB_ICUBPART_H
