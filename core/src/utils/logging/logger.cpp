#include "logger.h"

#include <dawg-log/logger.hpp>
#include <stdexcept>
#include <string>

namespace pt::utils::logging {
/**
 * The default location of the DawgLogger configuration file
 * (inside the current directory)
 */
static constexpr const char* default_dawglogger_config_location_in_pwd = "/config/dawglogger.json";

/** The environment variable name that holds the DawgLogger configuration file location */
static constexpr const char* dawglogger_config_location_env_key = "DAWGLOGGER_CONFIGURATION_PATH";
}  // namespace pt::utils::logging

void pt::utils::logging::initialize_dawglogger() {
    std::string path;

    const auto path_raw = getenv(dawglogger_config_location_env_key);
    const auto pwd_raw = getenv("PWD");

    if (path_raw != nullptr) {
        path = std::string(path_raw);
    } else if (pwd_raw != nullptr) {
        path = std::string(pwd_raw) + default_dawglogger_config_location_in_pwd;
    } else {
        throw std::runtime_error("$" + std::string(dawglogger_config_location_env_key) +
                                 "and $PWD are not defined");
    }

    DawgLog::Logger::init(DawgLog::Config(path));
}
