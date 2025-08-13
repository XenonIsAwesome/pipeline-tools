#pragma once

#include <any>
#include <memory>
#include <flow/Block.hpp>

#include <optional>

namespace pt::flow::blocks {
    // TODO(xenon): add docs
    template<typename Output>
    class Source : public Block {
    public:
        using output_type = Output;

        ~Source() override = default;

        // TODO(xenon): add docs
        explicit Source(std::string name): Block(std::move(name)) {}

        // TODO(xenon): add docs
        virtual std::optional<Output> process() = 0;
    };

    class ISourceAny {
    public:
        virtual ~ISourceAny() = default;

        virtual std::string get_name() = 0;
        virtual std::optional<std::any> process_any() = 0;
    };

    template<typename Output>
    class SourceHolder final : public ISourceAny {
    public:
        explicit SourceHolder(std::shared_ptr<Source<Output>> m):
            source(std::move(m)) {}

        std::optional<std::any> process_any() override {
            auto result = source->process();
            if (result.has_value())
                return std::any(result.value()); // wrap value into std::any
            return std::nullopt;
        }

        std::string get_name() override {
            return source->get_name();
        }

    private:
        std::shared_ptr<Source<Output>> source;

    };
} // namespace pt::flow::blocks
