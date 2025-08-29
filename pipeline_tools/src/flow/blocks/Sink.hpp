#pragma once

#include <sstream>
#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename In>
    class Sink : public Flow {
    public:
        using input_type = In;

        explicit Sink() : Flow(ProductionPolicy::NoConsumer) {
        }

        virtual void process(In input) = 0;

        std::any process_any(std::any in, size_t producer_id) override {
            In cast_input;
            try {
                cast_input = std::any_cast<In>(in);
            } catch (const std::bad_any_cast &e) {
                /// TODO: throw custom error
                std::stringstream ss;
                ss << __FILE__ << ":" << __LINE__ << ":" << std::endl;
                ss << in.type().name() << ":" << typeid(In).name() << std::endl;
                ss << e.what();
                throw std::runtime_error(ss.str());
            }
            process(cast_input);
            return {};
        }
    };
}
