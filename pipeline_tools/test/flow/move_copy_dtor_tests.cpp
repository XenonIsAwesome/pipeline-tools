#include <gtest/gtest.h>
#include "flow_test_utils.hpp"
#include <flow/Pipeline.hpp>
#include <flow/blocks/Source.hpp>
#include <flow/blocks/Sink.hpp>

// --- Custom test blocks ---
class TracerSource : public pt::flow::Source<Tracer> {
public:
    std::optional<Tracer> process() override {
        return Tracer{42};
    }
};

class TracerSink : public pt::flow::Sink<Tracer> {
public:
    void process(Tracer t) override {
        collected.push_back(t.value);
    }

    std::vector<int> collected;
};

// --- Tests ---

TEST(MoveCopyTests, FanoutCopiesToAllButMovesToLast) {
    Tracer::reset();

    pt::flow::Pipeline p;
    auto src = p.add(std::make_shared<TracerSource>());
    auto sink1 = p.add(std::make_shared<TracerSink>());
    auto sink2 = std::make_shared<TracerSink>();
    pt::flow::connect(src, sink2);

    p.execute();

    // Both sinks should see value 42
    ASSERT_EQ(42, sink1->collected[0]);
    ASSERT_EQ(42, sink2->collected[0]);

    // Expect: ctor (in process) + copy (to sink1) + move (to sink2) + dtor’s
    EXPECT_NE(std::find(Tracer::events.begin(), Tracer::events.end(), "copy"), Tracer::events.end());
    EXPECT_NE(std::find(Tracer::events.begin(), Tracer::events.end(), "move"), Tracer::events.end());
}

TEST(MoveCopyTests, RoundRobinMovesWithoutCopies) {
    Tracer::reset();

    class RoundRobinSource : public pt::flow::Source<std::vector<Tracer> > {
    public:
        RoundRobinSource(): Source(pt::flow::ProductionPolicy::RoundRobin) {
        }

        std::optional<std::vector<Tracer> > process() override {
            std::vector<Tracer> v;
            v.reserve(2); // avoid reallocations
            v.emplace_back(1);
            v.emplace_back(2);
            return v;
        }
    };

    pt::flow::Pipeline p;
    p.add(std::make_shared<RoundRobinSource>());
    auto sink1 = p.add(std::make_shared<TracerSink>());
    auto sink2 = p.add(std::make_shared<TracerSink>());

    p.execute();

    ASSERT_EQ(1, sink1->collected.size());
    ASSERT_EQ(1, sink2->collected.size());

    // Should only see moves from vector into sinks
    EXPECT_EQ(std::count(Tracer::events.begin(), Tracer::events.end(), "copy"), 0);
    EXPECT_GT(std::count(Tracer::events.begin(), Tracer::events.end(), "move"), 0);
}

TEST(MoveCopyTests, DestructorsCalledAfterPipelineScope) {
    Tracer::reset(); {
        pt::flow::Pipeline p;
        p.add(std::make_shared<TracerSource>());
        p.add(std::make_shared<TracerSink>());
        p.execute();
    }
    // After pipeline destruction, at least one dtor should be called
    EXPECT_NE(std::find(Tracer::events.begin(), Tracer::events.end(), "dtor"), Tracer::events.end());
}
