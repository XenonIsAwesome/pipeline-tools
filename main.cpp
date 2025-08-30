#include <memory>
#include <flow/Pipeline.hpp>
#include <modules/io/ConstantSource.hpp>
#include <modules/io/PrintSink.hpp>
#include <modules/math/AdditionModule.hpp>

int main() {
    auto mod_a = std::make_shared<pt::modules::AdditionModule<int, int, int>>(1);
    auto mod_b = std::make_shared<pt::modules::AdditionModule<int, int, int>>(2);
    auto mod_c = std::make_shared<pt::modules::AdditionModule<int, int, int>>(3);
    auto sink = std::make_shared<pt::modules::PrintSink<int>>();

    auto p = pt::flow::make_pipeline(
        std::make_tuple(std::make_shared<pt::modules::ConstantSource<int> >(1)),
        std::make_tuple(
            mod_a,
            mod_b,
            mod_c
        ),
        std::make_tuple(sink)
    );

    pt::flow::connect(mod_a, mod_c);
    pt::flow::connect(mod_b, sink);

    p.execute();
}
