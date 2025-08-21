#pragma once

#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename Out>
    class Source : public Flow {
    public:
        explicit Source(std::string name): Flow(std::move(name)) {}

        virtual std::vector<Out> process() = 0;

        std::vector<std::any> process_any(const std::any&) override {
            auto result = process();
            std::vector<std::any> out;
            out.reserve(result.size());
            for (auto& r : result) out.push_back(r);
            return out;
        }
    };
}
