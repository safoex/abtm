//
// Created by safoex on 15.07.19.
//

#ifndef ABTM_PARSERWITHMODULES_H
#define ABTM_PARSERWITHMODULES_H

#include <defs.h>
#include <Tree.h>
#include <Node.h>
#include "Builder.h"
#include "BaseParser.h"

namespace bt {
    class ParserWithModules : public BaseParser {
    protected:
        dictOf<BaseParser*> parsers; // {scope e.g. "nodes", parser [...]}
    public:
        explicit ParserWithModules(Builder& builder);
        explicit ParserWithModules(Builder& builder,
                std::vector<std::pair<std::vector<std::string>, BaseParser*>> const& parsers);

        void registerModule(std::string const& type, BaseParser *parser);
        void registerModules(std::vector<std::pair<std::vector<std::string>, BaseParser*>> const& parsers);

        ~ParserWithModules() override;
    };
}

#endif //ABTM_PARSERWITHMODULES_H
