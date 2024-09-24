#pragma once
#include "json/json.hpp"

#include <string>

using std::string;

using json = nlohmann::json;

class Settings {
 public:
  Settings();
  ~Settings() = default;
  Settings(const Settings& left) = delete;
  Settings& operator=(const Settings& left) = delete;

  string get_lang() const;
  void set_lang(const string& lang);
  bool get_auto_start() const;
  void set_auto_start(bool start);

  bool save();

 private:
  bool _load();

 private:
  json _config;

  const string _config_name = "config.json";
};
