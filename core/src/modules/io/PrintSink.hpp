#pragma once

#include <dawg-log/logger.hpp>
#include <flow/blocks/Sink.hpp>
#include <iostream>
#include <utils/logging/logger.h>

namespace pt::concepts {
/**
 * A concept that applies to types that can be printed.
 */
template<typename T>
concept Printable = requires(T t) { std::cout << t; };

}  // namespace pt::concepts

namespace pt::modules {
/**
 * Sink that logs/prints the input that goes into it.
 * @tparam In The input type to print
 */
template<concepts::Printable In>
class PrintSink : public flow::Sink<In> {
public:
    PrintSink() : logger("PrintSink") {}

    void process(In input) override {
        logger.info(LOG_SRC, "{}", input);
    }

private:
    DawgLog::TaggedLogger logger;
};

}  // namespace pt::modules