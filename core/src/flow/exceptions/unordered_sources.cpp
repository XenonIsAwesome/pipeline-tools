#include "unordered_sources.h"
#include <utils/misc_utils.h>

std::string pt::flow::exceptions::unordered_sources::build_msg() {
    return "unordered_sources: Source added after module/aggregator/sink";
}
