//
// Created by safoex on 23.07.19.
//

#include "MIMOCenter.h"

namespace bt {
    MIMOCenter::MIMOCenter(bt::ExecutorBase *executor) : executor(executor) {
        step = 0;
    }

    void MIMOCenter::process_once(const bt::sample &sample, MIMO_DIRECTION direction, IOBase *channel) {
        lock_guard lockGuard(lock);

        //process(sample, INPUT) -> apply out = executor->callback() and process(out, OUTPUT)
        if(direction == INPUT && channel == nullptr) {
            process_once(executor->callback(sample), OUTPUT);
        }

        //process(sample, OUTPUT, nullptr) -> put <sample> into <tasks> for all channels
        if(direction == OUTPUT && channel == nullptr) {
            if (!sample.empty()) {
                unordered_set<IOBase*> selected_channels;
                for(auto const& kv: sample) {
                    auto const& channels_from_var = vars_to_channels[kv.first];
                    selected_channels.insert(channels_from_var.begin(), channels_from_var.end());
                }

                for (auto const &m: selected_channels)
                    tasks.push({step, channels[m], m, OUTPUT, executor->update_sample(m->get_required_vars())});
                step++;
            }
        }

        //process(sample, OUTPUT, channel) -> push <sample> into channel->process()
        if(direction == OUTPUT && channel != nullptr) {
            channel->process(sample);
        }

        //process(sample, INPUT, channel) -> process(sample, INPUT) || might be changed in future
        if(direction == INPUT && channel != nullptr) {
            tasks.push({step, channels[channel], channel, INPUT, sample});
            step++;
        }
    }

    void MIMOCenter::process() {
        while(!tasks.empty()) {
            auto task = tasks.top();
            tasks.pop();
            process_once(task.msg, task.direction, task.channel);
        }
    }

    void MIMOCenter::process(const bt::sample &sample, bt::MIMO_DIRECTION direction, bt::IOBase *channel) {
        process_once(sample, direction, channel);
        process();
    }

    MIMOCenter::InputFunction MIMOCenter::registerIOchannel(bt::IOBase *channel, unsigned int priority) {
        channels[channel] = priority;
        if(!channel->get_trigger_vars().empty())  { // INPUT_ONLY()
            if(channel->get_trigger_vars().count(FULL_MEMORY().begin()->first)) {
                for (auto const &kv: channel->get_trigger_vars()) {
                    vars_to_channels[kv.first].insert(channel);
                }
            }
            else {
                //TODO: as soon as Memory<std::any> implemented & used, add passing full memory const& as sample for channel
            }
        }
        return [this, channel](sample const& s) {
            this->process(s, INPUT, channel);
        };
    }

    void MIMOCenter::start() {
        process(executor->init(), OUTPUT);
    }

    const sample MIMOCenter::INPUT_ONLY() {
        return {};
    }

    const sample MIMOCenter::FULL_MEMORY() {
        return {{"_____FULL__MEMORY______", 0}};
    }

}