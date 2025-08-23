#pragma once
#include <memory>
#include <vector>
#include <flow/Flow.hpp>

#include "Source.hpp"

namespace pt::flow {
    class Pipeline final : public Block {
    public:
        explicit Pipeline(std::string name): Block(std::move(name)) {}

        // Add a flow object and auto-connect to previous
        template<typename F, typename FOut = typename F::output_type>
        std::shared_ptr<F> add(std::shared_ptr<F> f) {
            if constexpr (std::derived_from<F, Source<FOut>>) {
                bool sinks_empty = sinks.empty();
                sinks.push_back(f);
                if (!sinks_empty) {
                    return f;
                }
            }

            if (!nodes.empty()) {
                nodes.back()->connect(f); // auto-wire previous -> current
            }

            nodes.push_back(f);
            return f;
        }

        // Manual connect (for branching/fanout/etc.)
        static void connect(const std::shared_ptr<Flow> &from,
                            const std::shared_ptr<Flow> &to) {
            from->connect(to);
        }

        // Trigger execution (start from first node)
        void execute() const {
            if (!sinks.empty()) {
                for (auto& sink : sinks) {
                    sink->execute();
                }
            } else if (!nodes.empty()) {
                nodes.front()->execute();
            }
        }

    private:
        std::vector<std::shared_ptr<Flow> > sinks{};
        std::vector<std::shared_ptr<Flow> > nodes{};
    };
} // namespace pt::flow
