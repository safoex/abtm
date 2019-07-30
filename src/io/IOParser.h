//
// Created by safoex on 26.07.19.
//

#ifndef ABTM_IOPARSER_H
#define ABTM_IOPARSER_H

#include "defs.h"
#include "IOBase.h"
#include "MIMOCenter.h"
#include "../parser/BaseParser.h"

namespace bt {
    class IOParser : public BaseParser {
    public:
        std::vector<IOBase*> io_modules;
        MIMOCenter& mimo;
        IOParser(Builder& builder, MIMOCenter& mimo) : BaseParser(builder), mimo(mimo) {};
        InputFunction register_module(IOBase* ioModule, unsigned priority = 0) {
            io_modules.push_back(ioModule);
            return mimo.registerIOchannel(ioModule, priority);
        }
        ~IOParser() override {
            for(auto const& io: io_modules)
                delete io;
        }
    };
}

#endif //ABTM_IOPARSER_H
