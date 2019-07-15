//
// Created by safoex on 15.07.19.
//

#ifndef ABTM_COMMONPARSER_H
#define ABTM_COMMONPARSER_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "Parser.h"
#include "BaseParser.h"

namespace bt {
    class CommonParser : public BaseParser{
    public:
        explicit CommonParser(Builder& builder);

        void parse(std::string const& id, YAML::Node const &yaml_node) override;

        ~CommonParser() override = default;

    };
}

#endif //ABTM_COMMONPARSER_H
