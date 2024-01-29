#include <cstdlib>

#include <fmt/format.h>

#include "xdg.h"

xdg::xdg()
{
  this->load_all_xdg_env();
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
