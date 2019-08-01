//
// Created by hamed on 27.06.19.
//

#ifndef HELLO_ICUB_ICUBROBOT_H
#define HELLO_ICUB_ICUBROBOT_H

#include <bits/stdc++.h>
#include "iCubPart.h"
#include "iCubEmotions.h"
#include <opencv2/opencv.hpp>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::sig::file;

class iCubRobot {
public:
    Network yarp;
    string robotName;
    iCubPart* head ;
    iCubPart* leftArm;
    iCubPart* rightArm;
    iCubPart* rightLeg;
    iCubPart* leftLeg;
    iCubPart* torso;
    iCubEmotions* emotions;

    IplImage *cvIplImageLeft;
    IplImage *cvIplImageRight;

    BufferedPort<ImageOf<PixelRgb> > imagePortLeft;     // make a port for reading left images
    BufferedPort<ImageOf<PixelRgb> > imagePortRight;  // make a port for reading right images

    iCubRobot(string robotName){
        this->robotName = robotName;
        string icub_prefix = "/icubSim";
        head = new iCubPart("Head", icub_prefix + "/head");
        leftArm = new iCubPart("LeftArm", icub_prefix + "/left_arm");
        rightArm = new iCubPart("RightArm", icub_prefix + "/right_arm");
        rightLeg = new iCubPart("RightLeg", icub_prefix + "/right_leg");
        leftLeg = new iCubPart("LeftLeg", icub_prefix + "/left_leg");
        torso = new iCubPart("torso", icub_prefix + "/torso");

        cout << "<<<<<<<<<< ---------------- Motors are initialized ------------------- >>>>>>>>>>>>" << endl;

        emotions = new iCubEmotions();

        imagePortLeft.open("/icubWrapper/view/left/in");  // give the left port a name
        imagePortRight.open("/icubWrapper/view/right/in");  // give the left port a name



//        while(!Network::connect( icub_prefix + "/camcalib/left/out", "/icubWrapper/view/left/in"))
//        {
//            Time::delay(3);
//            cout << "try to connect left camera, please wait ..." << endl;
//        }
//
//        while(!Network::connect( icub_prefix + "/camcalib/right/out", "/icubWrapper/view/right/in"))
//        {
//            Time::delay(3);
//            cout << "try to connect right camera, please wait ..." << endl;
//        }
//
//
//
//        cout << "Camera connected ! " << endl;

    }
    void getLeftCameraImage(cv::Mat *destImage){

        ImageOf<PixelRgb> *yarpImageLeft = imagePortLeft.read();

        if ( cvIplImageLeft == NULL )
        {
            cvIplImageLeft = cvCreateImage(cvSize(yarpImageLeft->width(),yarpImageLeft->height()),
                                           IPL_DEPTH_8U,3);
        }
        cvCvtColor((IplImage*)yarpImageLeft->getIplImage(), cvIplImageLeft, CV_RGB2BGR);

        *destImage = cv::cvarrToMat(cvIplImageLeft);

    }

    void getRightCameraImage(cv::Mat *destImage){

        ImageOf<PixelRgb> *yarpImageRight = imagePortRight.read();

        if ( cvIplImageRight == NULL )
        {
            cvIplImageRight = cvCreateImage(cvSize(yarpImageRight->width(),yarpImageRight->height()),
                                           IPL_DEPTH_8U,3);
        }
        cvCvtColor((IplImage*)yarpImageRight->getIplImage(), cvIplImageRight, CV_RGB2BGR);

        *destImage = cv::cvarrToMat(cvIplImageRight);

    }
    void ApplyAllJointsCommands(){
        head->applyAll();
        leftArm->applyAll();
        rightArm->applyAll();
        rightLeg->applyAll();
        leftLeg->applyAll();
    }
    void homeAllJoints(){
        // Head Home Positions
        head->setJointPosition(0,0);
        head->setJointPosition(1,0);
        head->setJointPosition(2,0);
        head->setJointPosition(3,0);
        head->setJointPosition(4,0);
        head->setJointPosition(5,0);

        // Left Arm
        leftArm->setJointPosition(0,-34.147);
        leftArm->setJointPosition(1,30.396);
        leftArm->setJointPosition(2,0);
        leftArm->setJointPosition(3,50.038);
        leftArm->setJointPosition(4,0);
        leftArm->setJointPosition(5,0);
        leftArm->setJointPosition(6,0);
        leftArm->setJointPosition(7,20);
        leftArm->setJointPosition(8,30);
        leftArm->setJointPosition(9,0);
        leftArm->setJointPosition(10,0);
        leftArm->setJointPosition(11,0);
        leftArm->setJointPosition(12,0);
        leftArm->setJointPosition(13,0);
        leftArm->setJointPosition(14,0);
        leftArm->setJointPosition(15,0);
        leftArm->setJointPosition(16,0);


        // Right Arm
        rightArm->setJointPosition(0,-34.147);
        rightArm->setJointPosition(1,30.396);
        rightArm->setJointPosition(2,0);
        rightArm->setJointPosition(3,50.038);
        rightArm->setJointPosition(4,0);
        rightArm->setJointPosition(5,0);
        rightArm->setJointPosition(6,0);
        rightArm->setJointPosition(7,20);
        rightArm->setJointPosition(8,30);
        rightArm->setJointPosition(9,0);
        rightArm->setJointPosition(10,0);
        rightArm->setJointPosition(11,0);
        rightArm->setJointPosition(12,0);
        rightArm->setJointPosition(13,0);
        rightArm->setJointPosition(14,0);
        rightArm->setJointPosition(15,0);
        rightArm->setJointPosition(16,0);

        // Left Leg
        leftLeg->setJointPosition(0,0);
        leftLeg->setJointPosition(1,0);
        leftLeg->setJointPosition(2,0);
        leftLeg->setJointPosition(3,0);
        leftLeg->setJointPosition(4,0);
        leftLeg->setJointPosition(5,0);

        // Right Leg
        rightLeg->setJointPosition(0,0);
        rightLeg->setJointPosition(1,0);
        rightLeg->setJointPosition(2,0);
        rightLeg->setJointPosition(3,0);
        rightLeg->setJointPosition(4,0);
        rightLeg->setJointPosition(5,0);

        // Right Leg
        torso->setJointPosition(0,0);
        torso->setJointPosition(1,0);
        torso->setJointPosition(2,0);
        torso->setJointPosition(3,0);
        torso->setJointPosition(4,0);
        torso->setJointPosition(5,0);

        ApplyAllJointsCommands();
    }
    void keepAllJoinsStates(){
        head->keepCurrentJointPositions();
        leftArm->keepCurrentJointPositions();
        rightArm->keepCurrentJointPositions();
        rightLeg->keepCurrentJointPositions();
        leftLeg->keepCurrentJointPositions();
    }
    ~iCubRobot(){

    }


    void Init();
};


#endif //HELLO_ICUB_ICUBROBOT_H
