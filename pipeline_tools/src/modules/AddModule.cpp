#include "AddModule.h"

using namespace pt::modules;

std::optional<int> AddModule::process(const int& input) {
    return input + addition;
}
