#include <cstdlib>
#include <filesystem>

// #include <figcone/figcone.h>
#include <fmt/format.h>

#include "xdg.h"

namespace fs = std::filesystem;

xdg::xdg()
{
  this->load_all_xdg_env();
  this->find_config_file();
    // this->create_default_directories();
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

void xdg::create_default_directories()
// make all standard directories.
// might be a bit messy creating dirs even if they are not needed?
{
  if (this->xdg_env.contains("XDG_CONFIG_HOME"))
  {
    fs::create_directories(fs::path(xdg_env["XDG_CONFIG_HOME"]) / "bsh");
  }
  else if (this->xdg_env.contains("HOME"))
  {
    fs::create_directories(fs::path(xdg_env["HOME"]) / ".config" / "bsh");
  }
  else if (this->xdg_env.contains("USER"))
  {
    fs::create_directories("/home" / fs::path(xdg_env["USER"]) / ".config" / "bsh");
  }
  else
  {
    fs::create_directories(fs::path(".config") / "bsh");
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

// std::map read_or_create_config()
// {
//     // assumes xdg-env is loaded

//   fs::path configfile = self->xdg_env;
// }
