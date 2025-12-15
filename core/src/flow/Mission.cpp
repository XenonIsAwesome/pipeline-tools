#include <flow/Mission.hpp>
#include <ranges>
#include <utils/threads/Worker.h>

void pt::flow::Mission::start(bool strict) {
    build();

    for (auto& pipeline : std::ranges::reverse_view(pipelines)) {
        workers.emplace_back(std::make_unique<threads::Worker>(
            threads::ThreadPolicy{.cores = 1,
                                  .affinity_type = threads::AffinityType::PINNED,
                                  .priority = THREAD_PRIORITY_LOWEST},
            [&pipeline](const std::atomic_flag& worker_stop_flag) {
                while (!worker_stop_flag.test(std::memory_order_relaxed)) {
                    pipeline.execute();
                }
            }));

        workers.back()->start(strict);
    }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void pt::flow::Mission::stop() {
    for (auto& w : std::ranges::reverse_view(workers)) {
        w->stop();
    }
}
