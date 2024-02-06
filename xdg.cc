#include <cstdlib>
#include <fstream>
#include <map>

// #include <figcone/figcone.h>
#include <fmt/format.h>

#include <toml.hpp>

#include "xdg.h"

namespace fs = std::filesystem;

xdg::xdg()
{
  this->load_all_xdg_env();
  bool t = this->find_config_file();
  fmt::print("path {}\n", t);

  this->read_or_create_config();
}

int xdg::load_all_xdg_env()
{
  for (const std::string& var : well_known_env_vars) {
    char* val = std::getenv(var.c_str());
    if (val) {
      this->xdg_env.insert_or_assign(var, val);
    }
  }
  return 0;
}

bool xdg::find_config_file()
{
  if (this->xdg_env.contains("XDG_CONFIG_HOME"))
  {
    this->config_path = fs::path(xdg_env["XDG_CONFIG_HOME"]) / "bsh" / "config";
  }
  else if (this->xdg_env.contains("HOME"))
  {
    this->config_path = fs::path(xdg_env["HOME"]) / ".config" / "bsh" / "config";
  }
  else if (this->xdg_env.contains("USER"))
  {
    this->config_path = "/home" / fs::path(xdg_env["USER"]) / ".config" / "bsh" / "config";
  }
  else
  {
    this->config_path = fs::path(".config") / "bsh" / "config";
  }

  if (fs::is_regular_file(config_path)) {
    return true;
  }
  else {
    fs::create_directories(this->config_path.parent_path());

    return false;
  }
}

void xdg::print_env()
{
  for (const auto& var : well_known_env_vars) {
    if (const auto& search = this->xdg_env.find(var); search != this->xdg_env.end()) {
      fmt::print("{}: {}\n", var, search->second);
    }
    else {
      fmt::print("{}: null\n", var);
    }
  }
}

bool xdg::read_or_create_config()
{
    // assumes xdg-env is loaded
  if (fs::is_regular_file(this->config_path)) {
    // std::ifstream config_file(this->config_path, std::ios_base::ate);
    // const auto size = config_file.tellg();
    // std::string str(size, '\0'); // construct string to stream size
    // config_file.seekg(0);
    // if (config_file.read(&str[0], size)) {
    //   fmt::print("config {}\n", str);
    // }
    try
    {
      const auto config = toml::parse_file(this->config_path.string());
    }
    catch (const toml::parse_error& err)
    {
      fmt::print("Failed to parse config file {}:\n\t{}\n",
                 this->config_path.string(),
                 err.description());
    }
  }
  else {
    return false;
  }

  return true;
}
