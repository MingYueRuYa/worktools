#pragma once
#include "settings.h"

#include <filesystem>
#include <fstream>

Settings::Settings() {
  this->_load();
}

std::string Settings::get_lang() const {
  if (_config.is_null()) {
    return "zh_CN";
  } else {
    return _config["lang"];
  }
}

void Settings::set_lang(const string& lang) {
  _config["lang"] = lang;
}

bool Settings::get_auto_start() const {
  if (_config.is_null()) {
    return false;
  } else {
    return _config["auto_start"];
  }
}

void Settings::set_auto_start(bool start) {
  _config["auto_start"] = start;
}

bool Settings::save() {
  std::ofstream o(this->_config_name);
  o << std::setw(4) << _config << std::endl;
  return true;
}

bool Settings::_load() {
  if (!std::filesystem::exists(this->_config_name)) {
    return false;
  }

  std::ifstream f(this->_config_name);
  _config = json::parse(f);
  return true;
}
