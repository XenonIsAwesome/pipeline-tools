#pragma once

#include <any>
#include <iostream>
#include <memory>
#include <flow/Block.hpp>

#include <optional>

namespace pt::flow::blocks {
    // TODO(xenon): add docs
    template<typename Input, typename Output>
    class Module : public Block {
    public:
        using input_type = Input;
        using output_type = Output;

        ~Module() override = default;

        // TODO(xenon): add docs
        explicit Module(std::string name): Block(std::move(name)) {}

        // TODO(xenon): add docs
        virtual std::optional<Output> process(const Input &) = 0;
    };

    class IModuleAny {
    public:
        virtual ~IModuleAny() = default;

        virtual std::string get_name() = 0;
        virtual std::optional<std::any> process_any(std::any in) = 0;
    };

    template<typename Input, typename Output>
    class ModuleHolder final : public IModuleAny {
    public:
        explicit ModuleHolder(std::shared_ptr<Module<Input, Output>> m):
            module(std::move(m)) {}

        std::optional<std::any> process_any(std::any in) override {
            auto result = module->process(std::any_cast<Input>(in));
            if (result.has_value())
                return std::any(result.value());
            return std::nullopt;
        }


        std::string get_name() override {
            return module->get_name();
        }

    private:
        std::shared_ptr<Module<Input, Output>> module;
    };
} // namespace pt::flow::blocks
