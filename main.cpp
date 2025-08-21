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

    p.add(std::make_shared<pt::modules::AddModule>(2));

    auto mod_b = std::make_shared<pt::modules::AddModule>(3);
    p.add(mod_b);

    pt::flow::Pipeline::connect(mod_a, mod_b);

    p.add(std::make_shared<pt::modules::PrintSink<int>>());

    p.execute();
}
