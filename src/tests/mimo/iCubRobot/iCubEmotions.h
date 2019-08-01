//
// Created by hamed on 01.07.19.
//

#ifndef HELLO_ICUB_ICUBEMOTIONS_H
#define HELLO_ICUB_ICUBEMOTIONS_H


#include <cstdio>
#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/RpcClient.h>
#include <iostream>


using namespace std;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

class iCubEmotions {

public:
    RpcClient port;
    Bottle cmd;

    iCubEmotions(){

        port.open("/iCubWrapper/emotions/out");
 //     yarp::os::ContactStyle style;
 //     style.expectReply = false;
 //     style.persistent = true;
 //     Network::connect("/iCubWrapper/emotions/out","/icub/face/emotions/in",style); // Not Working Properly , it is better connect it with terminal  ` yarp connect /iCubWrapper/emotions/out /icub/face/emotions/in `
        cout << "In order to see the emotions please connect our wrapper output to icub face emotion using \r\n yarp connect /iCubWrapper/emotions/out /icub/face/emotions/in" << endl;
    }

    void setEmotion(string part, string mode){
        cmd.addString("set");
        cmd.addString(part);
        cmd.addString(mode);
        port.write(cmd);
    }

    void setAllNeutral(){
        cmd.addString("set");
        cmd.addString("all");
        cmd.addString("neu");
        port.write(cmd);
    }

    void setAllSad(){
        cmd.addString("set");
        cmd.addString("all");
        cmd.addString("sad");
        port.write(cmd);
    }
    void setAllHap(){
        cmd.addString("set");
        cmd.addString("all");
        cmd.addString("hap");
        port.write(cmd);
    }
};


#endif //HELLO_ICUB_ICUBEMOTIONS_H
