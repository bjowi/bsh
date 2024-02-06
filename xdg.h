#pragma once

#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>

const std::set<std::string> well_known_env_vars
{
  "EDITOR",
  "HOME",
  "PATH",
  "USER",
  "XDG_CACHE_HOME",
  "XDG_CONFIG_DIRS",
  "XDG_CONFIG_HOME",
  "XDG_CURRENT_DESKTOP",
  "XDG_DATA_DIRS",
  "XDG_DATA_HOME",
  "XDG_GREETER_DATA_DIR",
  "XDG_RUNTIME_DIR",
  "XDG_SEAT",
  "XDG_SEAT_PATH",
  "XDG_SESSION_CLASS",
  "XDG_SESSION_DESKTOP",
  "XDG_SESSION_ID",
  "XDG_SESSION_PATH",
  "XDG_SESSION_TYPE",
  "XDG_STATE_HOME",
  "XDG_VTNR",
};

struct xdg
{
  xdg();

  int load_all_xdg_env();
  void print_env();
  bool find_config_file();
  bool read_or_create_config();

  std::unordered_map<std::string, std::string> xdg_env;

private:
  void create_default_directories();

  std::filesystem::path config_path;
};
