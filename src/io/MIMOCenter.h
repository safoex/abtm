//
// Created by safoex on 23.07.19.
//

#ifndef ABTM_MIMOCENTER_H
#define ABTM_MIMOCENTER_H

#include "defs.h"
#include "ExecutorBase.h"
#include "IOBase.h"
#include <map>
#include <vector>
#include <queue>
#include <functional>
#include <unordered_set>
#include <mutex>

namespace bt {
    using namespace std;
    enum MIMO_DIRECTION{
        MIMO_INPUT,
        MIMO_OUTPUT
    };

    class MIMOTask {
    public:
//        unsigned long long ticket;
//        unsigned channel_priority;
        std::pair<unsigned long long, unsigned> ticket_and_priority;
        IOBase* channel;
        MIMO_DIRECTION direction;
        sample msg;

        bool operator<(MIMOTask const& rhs) const {
            return ticket_and_priority < rhs.ticket_and_priority;
        }
        MIMOTask(unsigned long long ticket, unsigned channel_priority, IOBase* channel, MIMO_DIRECTION direction, sample msg)
            : ticket_and_priority(ticket, channel_priority), channel(channel), direction(direction), msg(msg) {}

    };
    class MIMOCenter {
    public:
        ExecutorBase* executor;

        // vector< <priority, channel> >
        std::unordered_map<IOBase*, unsigned> channels;

        //quickly find whether we need to feed the channel
        dictOf<unordered_set<IOBase*>> vars_to_channels;
        std::unordered_set<IOBase*> all_vars_channels;


        // step accumulates number of processed samples and used as "time when sample executed"
        unsigned long long step;

        // < <time when put into the queue, priority>, <channel, <output or input,  sample> > > >

        priority_queue<MIMOTask> tasks;

        //thread safety
        mutex lock;

        sample sample_for_channel(IOBase* ch, sample const& s);

    public:
        virtual void process_once(sample sample, MIMO_DIRECTION direction, IOBase *channel = nullptr);
        //process_once(sample, OUTPUT, nullptr) -> put <sample> into <tasks> for all channels
        //process_once(sample, OUTPUT, channel) -> push <sample> into channel->process()
        //process_once(sample, INPUT, nullptr) -> apply out = executor->callback() and process(out, OUTPUT)
        //process_once(sample, INPUT, channel) -> put <sample> into <tasks>

        virtual void process(sample const &sample, MIMO_DIRECTION direction, IOBase *channel = nullptr);
        //same as above, but after performs process_tasks()

        //TODO: as soon as Memory<std::any> implemented & used, add passing full memory const& as sample for channel
        //sample const& get_sample_for(IOBase* channel);
    public:
        explicit MIMOCenter(ExecutorBase* executor);
        virtual void process();
        virtual InputFunction registerIOchannel(IOBase* channel, unsigned priority = 0);
        static const sample ALL_MEMORY();
        static const sample ALL_CHANGED();
        static const sample ON_EVERY();
        virtual void start();
    };
}

#endif //ABTM_MIMOCENTER_H
