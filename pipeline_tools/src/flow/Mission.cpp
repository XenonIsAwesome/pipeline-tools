#include <flow/Mission.hpp>
#include <ranges>

void pt::flow::Mission::start(){
    // build all the pipelines
    this->build();

    // clear old workers if Mission was reused
    workers.clear();
    workers.reserve(pipelines.size());

    // start pipelines in reverse order
    for (auto& pipeline : std::ranges::reverse_view(pipelines)) {
        threads::ThreadPolicy policy{
            .cores = 1,
            .affinity_type = threads::AffinityType::PINNED,
            .priority = THREAD_PRIORITY_LOWEST
        };

        auto func = [&pipeline](std::atomic_flag& flag) -> void {
            while (!flag.test(std::memory_order_relaxed)) {
                pipeline.execute();
            }
        };
        workers.emplace_back(std::make_shared<threads::Worker>(std::move(policy), std::move(func)));
        workers.back()->start();
    }
}

void pt::flow::Mission::stop(){
    for (auto& w : std::ranges::reverse_view(workers)) {
        w->stop();
    }
    workers.clear();
}
