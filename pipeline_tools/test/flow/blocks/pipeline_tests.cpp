#include <gtest/gtest.h>
#include <flow/blocks/Pipeline.hpp>
#include <modules/NumberSource.h>
#include <modules/AddModule.h>
#include <flow/blocks/Sink.hpp>

TEST(PipelineTests, AutoConnectSingleChain) {
    pt::flow::Pipeline p("p");
    p.add(std::make_shared<pt::modules::NumberSource>(1));
    p.add(std::make_shared<pt::modules::AddModule>(2));

    class CaptureSink : public pt::flow::Sink<int> {
    public:
        CaptureSink() : Sink<int>("CaptureSink") {}
        void process(const int& input) override { captured = input; }
        int captured = 0;
    };
    auto sink = p.add(std::make_shared<CaptureSink>());
    p.execute();

    EXPECT_EQ(3, sink->captured);
}

TEST(PipelineTests, ManualConnectBranching) {
    pt::flow::Pipeline p("p");
    auto src = p.add(std::make_shared<pt::modules::NumberSource>(10));
    auto mod1 = p.add(std::make_shared<pt::modules::AddModule>(1));
    auto mod2 = std::make_shared<pt::modules::AddModule>(5);

    class CaptureSink : public pt::flow::Sink<int> {
    public:
        CaptureSink() : Sink<int>("CaptureSink") {}
        void process(const int& input) override { collected.push_back(input); }
        std::vector<int> collected;
    };
    auto sink1 = p.add(std::make_shared<CaptureSink>());
    auto sink2 = std::make_shared<CaptureSink>();

    pt::flow::Pipeline::connect(src, mod2);
    pt::flow::Pipeline::connect(mod2, sink2);

    p.execute();
    EXPECT_EQ(11, sink1->collected[0]);
    EXPECT_EQ(15, sink2->collected[0]);
}

TEST(PipelineTests, EmptyPipelineDoesNothing) {
    pt::flow::Pipeline p("p");
    EXPECT_NO_THROW(p.execute());
}
