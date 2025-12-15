#include "logger.h"

#include <dawg-log/logger.hpp>
#include <stdexcept>
#include <string>

namespace pt::utils::logging {
/**
 * The default location of the DawgLogger configuration file
 * (inside the home directory of the user)
 */
static constexpr const char* default_dawglogger_config_location_in_home = "";

/** The environment variable name that holds the DawgLogger configuration file location */
static constexpr const char* dawglogger_config_location_env_key = "DAWGLOGGER_CONFIGURATION_PATH";
}  // namespace pt::utils::logging

void pt::utils::logging::initialize_dawglogger() {
    std::string home;
    std::string path;

    const auto path_raw = getenv(dawglogger_config_location_env_key);
    const auto home_raw = getenv("HOME");
    const auto user_raw = getenv("USER");

    if (path_raw != nullptr) {
        path = std::string(path_raw);
    } else if (home_raw != nullptr) {
        path = std::string(home_raw) + default_dawglogger_config_location_in_home;
    } else if (user_raw != nullptr) {
        path = "/home/" + std::string(user_raw) + default_dawglogger_config_location_in_home;
    } else {
        throw std::runtime_error("$" + std::string(dawglogger_config_location_env_key) +
                                 ", $HOME and $USER are not defined");
    }

    DawgLog::Logger::init(DawgLog::Config(path));
}
