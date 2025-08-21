#pragma once
#include <memory>
#include <vector>
#include <flow/Flow.hpp>

namespace pt::flow {
    class Pipeline final : public Block {
    public:
        explicit Pipeline(std::string name): Block(std::move(name)) {}

        // Add a flow object and auto-connect to previous
        template<typename F>
        std::shared_ptr<F> add(std::shared_ptr<F> f) {
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
            if (!nodes.empty()) {
                nodes.front()->execute();
            }
        }

    private:
        std::vector<std::shared_ptr<Flow> > nodes{};
    };
} // namespace pt::flow
