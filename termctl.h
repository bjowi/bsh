#pragma once

#include <termios.h>
#include <cstdio>

struct termctl
{
  struct termios orig_termios;

  termctl() {};

  ~termctl()
  {
    disableRawMode();
  };

  void disableRawMode();
  void enableRawMode();
};
