#include <flow/Mission.hpp>

void pt::flow::Mission::start(){
    // clear old workers if Mission was reused
    workers.clear();
    stop_flag.clear();  // reset flag to "not stopped"

    // start pipelines in reverse order
    for (auto it = pipelines.rbegin(); it != pipelines.rend(); ++it) {
        workers.emplace_back([this, &pipeline = *it]() {
            while (!stop_flag.test(std::memory_order_relaxed)) {
                pipeline.execute();
            }
        });
    }
}

void pt::flow::Mission::stop(){
    // signal stop
    stop_flag.test_and_set(std::memory_order_relaxed);

    // join all threads
    for (auto& t : workers) {
        if (t.joinable()) {
            t.join();
        }
    }
    workers.clear();
}
