#pragma once

#include <flow/Flow.hpp>
#include <utils/exceptions/bad_any_cast_with_info.hpp>

namespace pt::flow {
    template<typename In>
    class Sink : public Flow {
    public:
        using input_type = In;

        virtual void process(In input) = 0;

        /**
         * Overriding with nothing because sink cannot be a producer.
         */
        void produce(std::any output) override {}

        std::any process_any(std::any in, size_t producer_id) override {
            process(std::move(utils::any_cast_with_info<In>(std::move(in))));
            return {};
        }
    };
}
