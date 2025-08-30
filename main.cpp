#include <memory>
#include <flow/Pipeline.hpp>
#include <modules/io/ConstantSource.hpp>
#include <modules/io/PrintSink.hpp>
#include <modules/math/AdditionModule.hpp>

int main() {
    pt::flow::Pipeline p;

    p.add(std::make_shared<pt::modules::ConstantSource<int> >(1));

    auto mod_a = std::make_shared<pt::modules::AdditionModule<int, int, int> >(1);
    p.add(mod_a);

    auto mod_b = std::make_shared<pt::modules::AdditionModule<int, int, int> >(2);
    p.add(mod_b);

    auto mod_c = std::make_shared<pt::modules::AdditionModule<int, int, int> >(3);
    p.add(mod_c);

    pt::flow::connect(mod_a, mod_c);

    auto sink = std::make_shared<pt::modules::PrintSink<int> >();
    p.add(sink);

    pt::flow::connect(mod_b, sink);

    p.execute();
}
