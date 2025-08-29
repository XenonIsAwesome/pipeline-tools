#pragma once

#include <cmath>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename In, typename Out>
    class Aggregator : public Flow {
    public:
        using input_type = std::vector<In>;
        using output_type = Out;

        Aggregator(): Flow(ProductionPolicy::Fanout) {
        }

        std::any process_any(std::any in, size_t producer_id) override {
            auto idx = static_cast<size_t>(std::pow(2, producer_id));
            if (state & idx) {
                return {};
            }

            In cast_input;
            try {
                cast_input = std::any_cast<In>(in);
            } catch (const std::bad_any_cast &e) {
                /// TODO: throw custom error
                std::stringstream ss;
                ss << __FILE__ << ":" << __LINE__ << ":" << e.what();
                throw std::runtime_error(ss.str());
            }

            latest[producer_id] = cast_input;
            state |= idx;

            auto final_state = static_cast<size_t>(std::pow(2, producer_count)) - 1;
            if (state == final_state) {
                state = 0;
                std::vector<In> collected;
                collected.reserve(producer_count);

                for (auto [_, v]: latest) {
                    collected.emplace_back(v);
                }

                auto output = process(std::move(collected));
                if (output.has_value()) {
                    return output.value();
                }

                return {};
            }

            return {};
        }

        virtual std::optional<Out> process(std::vector<In> inputs) = 0;

    protected:
        size_t register_producer(std::shared_ptr<Flow>) override {
            return producer_count++;
        }

    private:
        size_t state{0};
        size_t producer_count{0};
        std::unordered_map<size_t, In> latest;
    };
}
