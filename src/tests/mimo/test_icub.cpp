#include <cstdio>
#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>
#include <zconf.h>
#include "iCubRobot/iCubRobot.h"
#include <yarp/os/RpcClient.h>
#include <opencv/cv.hpp>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

using namespace cv;
using namespace std;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;


iCubRobot* SBiCub;

int NaturalMovementDemo(){
    int r = (rand() % 40) + 1;
    int jointRandom = r - 20;
    SBiCub->rightArm->setJointPosition(0,-34 + jointRandom);
    r = (rand() % 40) + 1;
    jointRandom = r - 20;
    SBiCub->rightArm->setJointPosition(1,30+jointRandom);
    r = (rand() % 40) + 1;
    jointRandom = r - 20;
    SBiCub->rightArm->setJointPosition(2,jointRandom);
    r = (rand() % 40) + 1;
    jointRandom = r - 20;
    SBiCub->leftArm->setJointPosition(0,-34+jointRandom);
    r = (rand() % 40) + 1;
    jointRandom = r - 20;
    SBiCub->leftArm->setJointPosition(1,30+jointRandom);
    r = (rand() % 40) + 1;
    jointRandom = r - 20;
    SBiCub->leftArm->setJointPosition(2,jointRandom);
    SBiCub->leftArm->applyAll();
    SBiCub->rightArm->applyAll();


}
int main() {
    srand(time(0));  // Initialize random number generator.
    BufferedPort<Bottle> readport;
    SBiCub = new iCubRobot("Sberbank iCub");
    SBiCub->keepAllJoinsStates();


    while (1) {

        NaturalMovementDemo();
        Time::delay(3);


    }
}