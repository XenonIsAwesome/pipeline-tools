#pragma once

#include <string>

namespace pt::flow {
enum class ProductionPolicy {
    Fanout, /**< Copies a single value to all consumers, but moves to the last */
    RoundRobin /**< Moves each output to each
                  consumer.<br/>`consumers[i].execute(output[i])`<br/>with `0 < i <
                  min(consumers.size(), outputs.size())`*/
};

std::string policy_to_string(ProductionPolicy policy);
}  // namespace pt::flow