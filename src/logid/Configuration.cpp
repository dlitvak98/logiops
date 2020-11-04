/*
 * Copyright 2019-2020 PixlOne
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stack>
#include <utility>
#include <vector>
#include <map>

#include "Configuration.h"
#include "util/log.h"

using namespace logid;
using namespace libconfig;
using namespace std::chrono;

Configuration::Configuration(std::stack<std::string>& config_files)
{
    bool config_file_read = false;
    while (!config_files.empty() && !config_file_read) {
        std::string config_file = config_files.top();
        logPrintf(INFO, "Attempting to read %s", config_file.c_str());
        config_files.pop();
        try {
            _config.readFile(config_file.c_str());
            config_file_read = true;
        } catch(const FileIOException &e) {
            logPrintf(WARN, "I/O Error while reading %s: %s",
                    config_file.c_str(), e.what());
        } catch(const ParseException &e) {
            logPrintf(ERROR, "Parse error in %s, line %d: %s", e.getFile(),
                    e.getLine(), e.getError());
        }
    }

    if (config_file_read == false) {
        logPrintf(ERROR, "No config files were read");
        throw new std::exception();
    }

    const Setting &root = _config.getRoot();

    try {
        auto& worker_count = root["workers"];
        if(worker_count.getType() == Setting::TypeInt) {
            _worker_threads = worker_count;
            if(_worker_threads < 0)
                logPrintf(WARN, "Line %d: workers cannot be negative.",
                        worker_count.getSourceLine());
        } else {
            logPrintf(WARN, "Line %d: workers must be an integer.",
                    worker_count.getSourceLine());
        }
    } catch(const SettingNotFoundException& e) {
        // Ignore
    }

    try {
        auto& timeout = root["io_timeout"];
        if(timeout.isNumber()) {
            if(timeout.getType() == Setting::TypeFloat)
                _io_timeout = duration_cast<milliseconds>(
                        duration<double, std::milli>(timeout));
            else
                _io_timeout = milliseconds((int)timeout);
        } else
            logPrintf(WARN, "Line %d: io_timeout must be a number.",
                    timeout.getSourceLine());
    } catch(const SettingNotFoundException& e) {
        // Ignore
    }

    try {
        auto& devices = root["devices"];

        for(int i = 0; i < devices.getLength(); i++) {
            const Setting& device = devices[i];
            std::string name;
            try {
                if(!device.lookupValue("name", name)) {
                    logPrintf(WARN, "Line %d: 'name' must be a string, skipping"
                                    " device.", device["name"].getSourceLine());
                    continue;
                }
            } catch(SettingNotFoundException &e) {
                logPrintf(WARN, "Line %d: Missing name field, skipping device."
                        , device.getSourceLine());
                continue;
            }
            _device_paths.insert({name, device.getPath()});
        }
    }
    catch(const SettingNotFoundException &e) {
        logPrintf(WARN, "No devices listed in config file.");
    }

    try {
        auto& ignore = root.lookup("ignore");
        if(ignore.getType() == libconfig::Setting::TypeInt) {
            _ignore_list.insert((int)ignore);
        } else if(ignore.isList() || ignore.isArray()) {
            int ignore_count = ignore.getLength();
            for(int i = 0; i < ignore_count; i++) {
                if(ignore[i].getType() != libconfig::Setting::TypeInt) {
                    logPrintf(WARN, "Line %d: ignore must refer to device PIDs",
                            ignore[i].getSourceLine());
                    if(ignore.isArray())
                        break;
                } else
                    _ignore_list.insert((int)ignore[i]);
            }
        }
    } catch(const SettingNotFoundException& e) {
        // May be called blacklist
        try {
            auto& ignore = root.lookup("blacklist");
            if(ignore.getType() == libconfig::Setting::TypeInt) {
                _ignore_list.insert((int)ignore);
            } else if(ignore.isList() || ignore.isArray()) {
                int ignore_count = ignore.getLength();
                for(int i = 0; i < ignore_count; i++) {
                    if(ignore[i].getType() != libconfig::Setting::TypeInt) {
                        logPrintf(WARN, "Line %d: blacklist must refer to "
                                        "device PIDs",
                                        ignore[i].getSourceLine());
                        if(ignore.isArray())
                            break;
                    } else
                        _ignore_list.insert((int)ignore[i]);
                }
            }
        } catch(const SettingNotFoundException& e) {
            // Ignore
        }
    }
}

libconfig::Setting& Configuration::getSetting(const std::string& path)
{
    return _config.lookup(path);
}

std::string Configuration::getDevice(const std::string& name)
{
    auto it = _device_paths.find(name);
    if(it == _device_paths.end())
        throw DeviceNotFound(name);
    else
        return it->second;
}

bool Configuration::isIgnored(uint16_t pid) const
{
    return _ignore_list.find(pid) != _ignore_list.end();
}

Configuration::DeviceNotFound::DeviceNotFound(std::string name) :
    _name (std::move(name))
{
}

const char * Configuration::DeviceNotFound::what() const noexcept
{
    return _name.c_str();
}

int Configuration::workerCount() const
{
    return _worker_threads;
}

std::chrono::milliseconds Configuration::ioTimeout() const
{
    return _io_timeout;
}
