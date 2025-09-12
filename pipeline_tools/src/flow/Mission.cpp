#include <flow/Mission.hpp>
#include <ranges>

void pt::flow::Mission::start(){
    // build all the pipelines
    this->build();

    // clear old workers if Mission was reused
    workers.clear();

    // start pipelines in reverse order
    for (auto& pipeline : std::ranges::reverse_view(pipelines)) {
        workers.push_back({{
            .cores = 1,
            .affinity_type = threads::AffinityType::PINNED
        }, [&pipeline](std::atomic_flag& flag) -> void {
            while (!flag.test()) {
                pipeline.execute();
            }
        }});

        workers.back().start();
    }
}

void pt::flow::Mission::stop(){
    for (auto& w : workers) {
        w.stop();
    }
    workers.clear();
}
