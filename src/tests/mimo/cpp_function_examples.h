//
// Created by safoex on 26.07.19.
//

#ifndef ABTM_CPP_FUNCTION_EXAMPLES_H
#define ABTM_CPP_FUNCTION_EXAMPLES_H

#include "../../io/cpp/SynchronousAction.h"
#include "../../io/cpp/CPPFunctionParser.h"
#include "../../io/cpp/CPPFunction.h"

namespace bt {
    namespace examples {
        std::vector<CPPFunction> functions {
                CPPFunction([](sample const& s) -> sample {
                    std::cout << "\n\n-----------------------\nHello, World!\n\n-------------------------" << std::endl;
                    return sample();
                }, "helloworld"),
                CPPFunction([](sample const& s) -> sample {
                    for(int x = 0; x < any_cast<double>(s.at("times")); x++)
                        std::cout << "Hello, Sber!" << std::endl;
                    return sample();
                }, "hellosber",{{"times",double(0)}})
        };
    }
}


#endif //ABTM_CPP_FUNCTION_EXAMPLES_H
