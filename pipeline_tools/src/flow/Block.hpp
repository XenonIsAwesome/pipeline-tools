#pragma once
#include <string>

namespace pt::flow {
    class Block {
    public:
        virtual ~Block() = default;

        explicit Block(std::string name): name(std::move(name)) {}
        std::string get_name() { return name; }

    protected:
        std::string name;
    };
}
