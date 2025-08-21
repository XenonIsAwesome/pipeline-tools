#pragma once

#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename In, typename Out>
    class Module : public Flow {
    public:
        explicit Module(std::string name): Flow(std::move(name)) {}

        virtual std::vector<Out> process(const In& input) = 0;

        std::vector<std::any> process_any(const std::any& in) override {
            std::vector<std::any> out;
            if (!in.has_value()) return out;

            const In& typed_in = std::any_cast<const In&>(in);
            auto results = process(typed_in);
            out.reserve(results.size());
            for (auto& r : results) out.push_back(r);
            return out;
        }
    };
}
