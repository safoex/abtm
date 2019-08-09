//
// Created by safoex on 07.08.19.
//

#include <yaml-cpp/yaml.h>
#include <bits/stdc++.h>
int main() {

    std::ifstream in("../src/tests/misc/test.yaml");

    YAML::Node config(YAML::Load(in));

    if(config["A"])
        std::cout << "OK" << std::endl;

    std::stringstream ss;

    ss << config["A"]["b"];
    config["B"].push_back("C");
    config["B"].push_back("D");

    config["C"]["D"] = "E";
    config["B"][0] = "2";

    for(auto const& p: config["C"])
        config["C"][p.first] = "EE";

    std::cout << config << std::endl;
}
