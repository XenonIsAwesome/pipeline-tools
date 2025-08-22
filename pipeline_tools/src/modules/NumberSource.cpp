#include "NumberSource.h"

using namespace pt::modules;

std::optional<int> NumberSource::process() {
    return num;
}
