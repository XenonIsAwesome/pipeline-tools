#include "flow_test_utils.hpp"

#include <flow/Mission.hpp>
#include <gtest/gtest.h>
#include <modules/io/ConstantSource.hpp>

class MockManualMission : public pt::flow::Mission {
public:
    std::vector<int> get_collected() const {
        if (sink == nullptr) {
            return {};
        }
        return std::move(sink->collected);
    }

protected:
    void build() override {
        auto q = pt::queues::make_queue<int>(pt::queues::QueueType::LOCK_FREE);

        pt::flow::Pipeline p1;
        p1.add(std::make_shared<pt::modules::ConstantSource<int>>(1));
        p1.add(std::make_shared<pt::modules::QueueSink<int>>(q));

        add(std::move(p1));

        pt::flow::Pipeline p2;
        p2.add(std::make_shared<pt::modules::QueueSource<int>>(q));
        sink = p2.add(std::make_shared<MockSink<int>>());

        add(std::move(p2));
    }

private:
    std::shared_ptr<MockSink<int>> sink;
};

class MockAutoMission : public pt::flow::Mission {
public:
    std::vector<int> get_collected() const {
        if (sink == nullptr) {
            return {};
        }
        return sink->collected;
    }

protected:
    void build() override {
        add(std::make_tuple(
            std::make_shared<pt::modules::ConstantSource<int>>(1)
        ), {}, {});

        sink = std::make_shared<MockSink<int>>();
        add({}, {}, std::make_tuple(sink));
    }

private:
    std::shared_ptr<MockSink<int>> sink;
};


TEST(MissionTests, ManualDataFlow) {
    MockManualMission mission;

    EXPECT_NO_THROW(mission.start(false));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto collected = mission.get_collected();
    ASSERT_GT(collected.size(), 0);
    EXPECT_EQ(collected.at(0), 1);

    EXPECT_NO_THROW(mission.stop());
}

TEST(MissionTests, AutoDataFlow) {
    MockAutoMission mission;

    EXPECT_NO_THROW(mission.start(false));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    auto collected = mission.get_collected();
    ASSERT_GT(collected.size(), 0);
    EXPECT_EQ(collected.at(0), 1);

    EXPECT_NO_THROW(mission.stop());
}
