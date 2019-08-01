//
// Created by safoex on 26.07.19.
//

#ifndef ABTM_ICUB_FUNCTION_EXAMPLE_H
#define ABTM_ICUB_FUNCTION_EXAMPLE_H


#include <cstdio>
#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>
#include <zconf.h>
#include "iCubRobot/iCubRobot.h"
#include <yarp/os/RpcClient.h>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

#include "../../io/cpp/SynchronousAction.h"
#include "../../io/cpp/CPPFunctionParser.h"
#include "../../io/cpp/CPPFunction.h"

using namespace cv;
using namespace std;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;


namespace  bt {
    class iCubRobotDemo {
    public:
        iCubRobot *SBiCub;

        iCubRobotDemo() {
            icub_init();
        }

        int NaturalMovementDemo() {
            int r = (rand() % 40) + 1;
            int jointRandom = r - 20;
            SBiCub->rightArm->setJointPosition(0, -34 + jointRandom);
            r = (rand() % 40) + 1;
            jointRandom = r - 20;
            SBiCub->rightArm->setJointPosition(1, 30 + jointRandom);
            r = (rand() % 40) + 1;
            jointRandom = r - 20;
            SBiCub->rightArm->setJointPosition(2, jointRandom);
            r = (rand() % 40) + 1;
            jointRandom = r - 20;
            SBiCub->leftArm->setJointPosition(0, -34 + jointRandom);
            r = (rand() % 40) + 1;
            jointRandom = r - 20;
            SBiCub->leftArm->setJointPosition(1, 30 + jointRandom);
            r = (rand() % 40) + 1;
            jointRandom = r - 20;
            SBiCub->leftArm->setJointPosition(2, jointRandom);
            SBiCub->leftArm->applyAll();
            SBiCub->rightArm->applyAll();


        }

        void icub_init() {
            srand(time(0));  // Initialize random number generator.
            BufferedPort<Bottle> readport;
            SBiCub = new iCubRobot("Sberbank iCub");
            SBiCub->keepAllJoinsStates();
        }

        std::vector<CPPFunction> functions() {
            return {
                    CPPFunction([this](sample const& s) -> sample {
                        this->NaturalMovementDemo();
//                        Time::delay(3);
                        return sample();
                    }, "iCub_move")
            };
        }
    };
}


#endif //ABTM_ICUB_FUNCTION_EXAMPLE_H
