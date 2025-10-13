#include "flow_test_utils.hpp"
#include <flow/Mission.hpp>
#include <modules/io/ConstantSource.hpp>
#include <gtest/gtest.h>

class MockMission: public pt::flow::Mission<> {
public:
    [[nodiscard]] std::vector<int> get_collected() const {
        if (sink == nullptr) {
            return {};
        }

        return sink->collected;
    }

protected:
    std::shared_ptr<MockSink> sink;
};

class MockManualMission final: public MockMission {
protected:
    void build() override {
        auto q = make_queue<int>(pt::queues::QueueType::LOCK_FREE);

        pt::flow::Pipeline p1;
        p1.add(std::make_shared<pt::modules::ConstantSource<int>>(1));
        p1.add(std::make_shared<pt::modules::QueueSink<int>>(q));
        add(p1);

        pt::flow::Pipeline p2;
        p2.add(std::make_shared<pt::modules::QueueSource<int>>(q));
        sink = p2.add(std::make_shared<MockSink>());
        add(p2);
    }
};

class MockAutoMission final: public MockMission {
protected:
    void build() override {
        add(std::make_tuple(
            std::make_shared<pt::modules::ConstantSource<int>>(1)
        ), {}, {});

        sink = std::make_shared<MockSink>();
        add({}, {},
            std::make_tuple(sink)
        );
    }
};

TEST(MissionTests, ManualDataFlow) {
    MockManualMission mission;

    EXPECT_NO_THROW(mission.start());

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    auto collected = mission.get_collected();
    ASSERT_GT(collected.size(), 0);
    EXPECT_EQ(collected.at(0), 1);

    EXPECT_NO_THROW(mission.stop());
}

TEST(MissionTests, AutomaticDataFlow) {
    MockAutoMission mission;

    EXPECT_NO_THROW(mission.start());

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    auto collected = mission.get_collected();
    ASSERT_GT(collected.size(), 0);
    EXPECT_EQ(collected.at(0), 1);

    EXPECT_NO_THROW(mission.stop());
}