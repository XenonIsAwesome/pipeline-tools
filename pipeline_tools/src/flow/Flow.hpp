#pragma once

#include <any>
#include <memory>
#include <vector>
#include <flow/Block.hpp>


namespace pt::flow {
    class Block;

    class Flow : public Block {
    public:
        explicit Flow(std::string name) : Block(std::move(name)) {}
        ~Flow() override = default;

        virtual std::vector<std::any> process_any(const std::any &in) = 0;

        void connect(std::shared_ptr<Flow> next) {
            next_nodes.push_back(std::move(next));
        }

        void execute(const std::any &in = {}) {
            auto results = process_any(in);

            if (results.empty()) return;

            // single result, many outputs => copy/move
            if (results.size() == 1 && next_nodes.size() > 1) {
                for (size_t i = 0; i < next_nodes.size(); ++i) {
                    auto val = (i + 1 == next_nodes.size())
                        ? std::move(results.front())
                        : results.front(); // copy for all but last
                    forward(next_nodes[i], val);
                }
            }
            // one-to-one: result[i] -> outputs[i]
            else {
                for (size_t i = 0; i < results.size() && i < next_nodes.size(); ++i) {
                    forward(next_nodes[i], std::move(results[i]));
                }
            }
        }

        template<typename T>
        void forward(const std::shared_ptr<Flow>& next, T&& val) {
            next->execute(val);
        }

    private:
        std::vector<std::shared_ptr<Flow>> next_nodes;
    };
} // namespace pt::flow
