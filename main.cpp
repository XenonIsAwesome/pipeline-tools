#include <flow/blocks/Pipeline.hpp>
#include <modules/AddModule.h>

#include "modules/NumberSource.h"
#include "modules/PrintSink.hpp"

int main() {
    pt::flow::blocks::Pipeline p("");

    p.set_source(std::make_shared<pt::modules::NumberSource>(1));
    p.add_module(std::make_shared<pt::modules::AddModule>(1));
    p.add_module(std::make_shared<pt::modules::AddModule>(2));
    p.add_module(std::make_shared<pt::modules::AddModule>(3));
    p.set_sink(std::make_shared<pt::modules::PrintSink<int>>());

    p.execute();
}
