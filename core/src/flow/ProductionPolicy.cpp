#include "ProductionPolicy.h"

std::string pt::flow::policy_to_string(ProductionPolicy policy) {
    switch (policy) {
        case ProductionPolicy::Fanout:
            return "Fanout";
        case ProductionPolicy::RoundRobin:
            return "RoundRobin";
        default:
            return "";
    }
}