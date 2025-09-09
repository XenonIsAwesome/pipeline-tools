#pragma once

#include <flow/blocks/Aggregator.hpp>
#include <flow/blocks/Module.hpp>
#include <flow/blocks/Sink.hpp>
#include <flow/blocks/Source.hpp>
#include <concepts>

namespace pt::flow::concepts {
template<typename F>
concept SourceLike = std::derived_from<F, Source<typename F::output_type>>;

template<typename F>
concept SinkLike = std::derived_from<F, Sink<typename F::input_type>>;

template<typename F>
concept ModuleLike = std::derived_from<F, Module<typename F::input_type, typename F::output_type>>;

template<typename F>
concept AggregatorLike = std::derived_from<F, Aggregator<typename F::input_type, typename F::output_type>>;

template<typename F>
concept NodeLike = ModuleLike<F> || AggregatorLike<F>;

template<typename F>
concept FlowLike = SourceLike<F> || NodeLike<F> || SinkLike<F>;
}