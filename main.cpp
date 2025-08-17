#include <flow/blocks/Pipeline.hpp>
#include <modules/AddModule.h>

#include "modules/NumberSource.h"
#include "modules/PrintSink.hpp"
#include "modules/QueueSink.hpp"
#include "modules/QueueSource.hpp"
#include "util/queues/LockFreeQueue.hpp"

int main() {

    auto queue = std::make_shared<pt::queues::LockFreeQueue<int>>();

    pt::flow::blocks::Pipeline p1("p1");
    p1.set_source(std::make_shared<pt::modules::NumberSource>(1));
    p1.add_module(std::make_shared<pt::modules::AddModule>(1));
    p1.add_module(std::make_shared<pt::modules::AddModule>(2));
    p1.add_module(std::make_shared<pt::modules::AddModule>(3));
    p1.set_sink(std::make_shared<pt::modules::QueueSink<int>>("p1->p2 SINK", queue));

    pt::flow::blocks::Pipeline p2("p2");
    p2.set_source(std::make_shared<pt::modules::QueueSource<int>>("p1->p2 SOURCE", queue));
    p2.add_module(std::make_shared<pt::modules::AddModule>(1));
    p2.add_module(std::make_shared<pt::modules::AddModule>(2));
    p2.add_module(std::make_shared<pt::modules::AddModule>(3));
    p2.set_sink(std::make_shared<pt::modules::PrintSink<int>>());

    p1.execute();
    p2.execute();
    p2.execute();
}
