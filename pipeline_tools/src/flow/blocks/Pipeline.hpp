#pragma once

#include <iostream>
#include <flow/blocks/Module.hpp>

#include <memory>
#include <vector>

#include "Sink.hpp"
#include "Source.hpp"


namespace pt::flow::blocks {
    class Pipeline final : public Block {
    public:
        explicit Pipeline(const std::string &name): Block(name), modules({}) {}

        template<typename S, typename Input = typename S::input_type>
        void set_sink(std::shared_ptr<S> s)
        requires std::derived_from<S, Sink<Input>>
        {
            sink = std::make_shared<SinkHolder<Input>>(std::move(s));
        }

        template<typename S, typename Output = typename S::output_type>
        void set_source(std::shared_ptr<S> s)
        requires std::derived_from<S, Source<Output>>
        {
            source = std::make_shared<SourceHolder<Output>>(std::move(s));
        }

        template<typename M, typename In = typename M::input_type, typename Out = typename M::output_type>
        void add_module(std::shared_ptr<M> m)
        requires std::derived_from<M, Module<In, Out>>
        {
            modules.push_back(std::make_shared<ModuleHolder<In, Out>>(std::move(m)));
        }

        void execute() const {
            std::optional<std::any> data = source->process_any();
            for (const auto &m : modules) {
                if (!data.has_value()) return;
                data = m->process_any(data.value());
            }
            if (!data.has_value()) return;
            sink->process_any(data.value());
        }

    private:
        std::shared_ptr<ISourceAny> source;
        std::vector<std::shared_ptr<IModuleAny>> modules;
        std::shared_ptr<ISinkAny> sink;
    };
} // namespace pt::flow::blocks
