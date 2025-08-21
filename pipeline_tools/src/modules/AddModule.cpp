#include "AddModule.h"

using namespace pt::modules;

std::vector<int> AddModule::process(const int& input) {
    return { input + addition };
}
