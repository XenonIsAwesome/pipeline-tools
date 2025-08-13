#pragma once

#include <any>
#include <memory>
#include <flow/Block.hpp>

namespace pt::flow::blocks {
    // TODO(xenon): add docs
    template<typename Input>
    class Sink : public Block {
    public:
        using input_type = Input;

        ~Sink() override = default;

        // TODO(xenon): add docs
        explicit Sink(std::string name): Block(std::move(name)) {}

        // TODO(xenon): add docs
        virtual void process(const Input &) = 0;
    };

    class ISinkAny {
    public:
        virtual ~ISinkAny() = default;

        virtual std::string get_name() = 0;
        virtual void process_any(std::any in) = 0;
    };

    template<typename Input>
    class SinkHolder final : public ISinkAny {
    public:
        explicit SinkHolder(std::shared_ptr<Sink<Input>> m):
            sink(std::move(m)) {}

        void process_any(std::any in) override {
            sink->process(std::any_cast<Input>(in));
        }

        std::string get_name() override {
            return sink->get_name();
        }

    private:
        std::shared_ptr<Sink<Input>> sink;

    };
} // namespace pt::flow::blocks
