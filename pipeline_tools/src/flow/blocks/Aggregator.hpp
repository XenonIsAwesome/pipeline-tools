#pragma once

#include <flow/Flow.hpp>
#include <unordered_map>
#include <vector>
#include <optional>

namespace pt::flow {

    enum class AggregationPolicy {
        NoAggregation,
        Synchronized,
        Continuous
    };

    template<typename Out>
    class Aggregator : public Flow {
    public:
        explicit Aggregator(std::string name, AggregationPolicy aggregation_policy = AggregationPolicy::Synchronized):
            Flow(std::move(name), ProductionPolicy::SingleOutput), policy(aggregation_policy) {}

        std::any process_any(const std::any& in, size_t producer_id = 0) override {
            latest[producer_id] = std::any_cast<Out>(in);

            switch (policy) {
                case AggregationPolicy::NoAggregation: {
                    auto result = process({std::any_cast<Out>(in)});
                    if (result) return std::any(*result);
                    return {};
                }

                case AggregationPolicy::Synchronized: {
                    if (latest.size() == producer_count) {
                        std::vector<Out> collected;
                        collected.reserve(latest.size());
                        for (auto& [_, v] : latest)
                            collected.push_back(v);
                        latest.clear();
                        auto result = process(collected);
                        if (result) return std::any(*result);
                    }
                    return {};
                }

                case AggregationPolicy::Continuous: {
                    std::vector<Out> collected;
                    collected.reserve(latest.size());
                    for (auto& [_, v] : latest)
                        collected.push_back(v);
                    auto result = process(collected);
                    if (result) return std::any(*result);
                    return {};
                }
            }
            return {};
        }

        virtual std::optional<Out> process(const std::vector<Out>& inputs) = 0;

    protected:
        size_t register_producer(std::shared_ptr<Flow>) override {
            return producer_count++;
        }

    private:
        AggregationPolicy policy;
        size_t producer_count{0};
        std::unordered_map<size_t, Out> latest;
    };
}
