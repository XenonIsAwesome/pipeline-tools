#include <flow/blocks/Pipeline.hpp>
#include <modules/AddModule.h>
#include <modules/NumberSource.h>
#include <modules/PrintSink.hpp>

#include <memory>

int main() {
    pt::flow::Pipeline p("");

    p.add(std::make_shared<pt::modules::NumberSource>(1));

    auto mod_a = std::make_shared<pt::modules::AddModule>(1);
    p.add(mod_a);

    auto mod_b = std::make_shared<pt::modules::AddModule>(2);
    p.add(mod_b);

    auto mod_c = std::make_shared<pt::modules::AddModule>(3);
    p.add(mod_c);

    pt::flow::Pipeline::connect(mod_a, mod_c);

    auto sink = std::make_shared<pt::modules::PrintSink<int>>();
    p.add(sink);

    pt::flow::Pipeline::connect(mod_b, sink);

    p.execute();
}
