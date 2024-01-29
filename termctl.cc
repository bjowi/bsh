#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "termctl.h"

void termctl::disableRawMode()
{
  int res = tcsetattr(STDIN_FILENO, TCSAFLUSH, &this->orig_termios);
  if (res < 0) {
    std::perror("tcsetattr");
  }
}

void termctl::enableRawMode() {
  int res = tcgetattr(STDIN_FILENO, &this->orig_termios);
  if (res < 0) {
    std::perror("tcgetattr");
  }

  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  res = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  if (res < 0) {
    std::perror("tcgetattr 2");
  }
}
