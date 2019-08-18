//
// Created by safoex on 23.07.19.
//

#include "MIMOCenter.h"
#include <iostream>
#include "memory/MemoryBase.h"

namespace bt {
    MIMOCenter::MIMOCenter(bt::ExecutorBase *executor) : executor(executor) {
        step = 0;
        started = false;
    }

    sample MIMOCenter::sample_for_channel(bt::IOBase *ch, sample const& s) {
        auto rs = ch->get_required_vars();
        if(rs.count(ALL_MEMORY().begin()->first)) {
            sample r;
            r["memory"] = &executor->get_memory();
            return r;
        }
        else if(rs.count(ALL_CHANGED().begin()->first)) {
            return s;
        }
        else return executor->update_sample(rs);
    }


    void MIMOCenter::process_once(bt::sample sample, MIMO_DIRECTION direction, IOBase *channel) {
        std::lock_guard lockGuard(lock);

        std::cout << " ----- process_once ------" << std::endl;
        std::cout << (direction == MIMO_INPUT ? "INPUT" : "OUTPUT") << std::endl;
        for(auto p : sample) std::cout << p.first << '\t';
        std::cout << std::endl;


        //process(sample, MIMO_INPUT) -> apply out = executor->callback() and process(out, MIMO_OUTPUT)
        if(direction == MIMO_INPUT && channel == nullptr) {
            sample = executor->callback(sample);
            direction = MIMO_DIRECTION::MIMO_OUTPUT;
        }

        //process(sample, OUTPUT, channel) -> push <sample> into channel->process()
        if(direction == MIMO_OUTPUT && channel != nullptr) {
            sample = channel->process(sample);
            direction = MIMO_INPUT;
        }

        //process(sample, MIMO_OUTPUT, nullptr) -> put <sample> into <tasks> for all channels
        if(direction == MIMO_OUTPUT && channel == nullptr) {

            std::cout << "------- OUTPUT sample ---------" << std::endl;
            for(auto const & p: sample) {
                std::cout << p.first << '\t' << std::any_cast<std::string>(p.second) << std::endl;
            }
            std::cout << std::endl << "--------------" << std::endl;

            if (!sample.empty()) {
                unordered_set<IOBase*> selected_channels;
                for(auto const& kv: sample) {
                    if(vars_to_channels.count(kv.first)) {
                        auto const &channels_from_var = vars_to_channels[kv.first];
                        selected_channels.insert(channels_from_var.begin(), channels_from_var.end());
                    }
                }
                selected_channels.insert(all_vars_channels.begin(), all_vars_channels.end());

                for (auto const &m: selected_channels)
                    tasks.push({step, channels[m], m, MIMO_OUTPUT, sample_for_channel(m, sample)});
                step++;
            }
        }



        //process(sample, MIMO_INPUT, channel) -> process(sample, MIMO_INPUT) || might be changed in future
        if(direction == MIMO_INPUT && channel != nullptr) {
            tasks.push({step, channels[channel], nullptr, MIMO_INPUT, sample});
            step++;
        }

        std::cout << " ------ process once ended-------------------" << std::endl;
    }

    void MIMOCenter::process() {
        while(!tasks.empty()) {
            auto task = tasks.top();
            tasks.pop();
            process_once(task.msg, task.direction, task.channel);
        }
    }

    void MIMOCenter::process(const bt::sample &sample, bt::MIMO_DIRECTION direction, bt::IOBase *channel) {
        if(started) {
            process_once(sample, direction, channel);
            process();
        }
    }

    InputFunction MIMOCenter::registerIOchannel(bt::IOBase *channel, unsigned int priority) {
        channels[channel] = priority;

        std::cout << "registering channel" << std::endl;
        for(auto const& p: channel->get_trigger_vars()) {
            std::cout << p.first << std::endl;
        }
        std::cout << std::endl;

        if(channel->get_trigger_vars().count(ON_EVERY().begin()->first))  {
            all_vars_channels.insert(channel);
        }
        else {
            for (auto const &kv: channel->get_trigger_vars()) {
                vars_to_channels[kv.first].insert(channel);
            }
        }
        return [this, channel](sample const& s) {
            std::cout << "WTF" << std::endl;
            this->process(s, MIMO_INPUT, channel);
        };
    }

    void MIMOCenter::start() {
        started = true;
        process(executor->init(), MIMO_OUTPUT);
    }

    const sample MIMOCenter::ALL_CHANGED() {
        return {{"____ALL_CHANGED____", std::any()}};
    }

    const sample MIMOCenter::ALL_MEMORY() {
        return {{"____ALL_MEMORY____", std::any()}};
    }

    const sample MIMOCenter::ON_EVERY() {
        return {{"____ON_EVERY____", std::any()}};
    }

}