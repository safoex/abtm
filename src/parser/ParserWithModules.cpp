//
// Created by safoex on 15.07.19.
//

#include "ParserWithModules.h"

namespace bt {
    ParserWithModules::ParserWithModules(bt::Builder &builder) : BaseParser(builder) {}

    void ParserWithModules::registerModule(std::string const &type, bt::BaseParser *parser) {
        parsers[type] = parser;
    }

    void ParserWithModules::registerModules(std::vector<std::pair<std::vector<std::string> , bt::BaseParser *>> const& parsers) {
        for(auto const& pp: parsers)
            for(auto const& pn: pp.first)
            registerModule(pn, pp.second);
    }

    ParserWithModules::~ParserWithModules() {}

    ParserWithModules::ParserWithModules(bt::Builder &builder,
         std::vector<std::pair<std::vector<std::string>, bt::BaseParser *>> const& parsers) : BaseParser(builder) {
        registerModules(parsers);
    }
}