#include <cwctype>
#include <cerrno>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <linux/sched.h>
#include <ranges>
#include <regex>
#include <sched.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <string_view>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <tuple>
#include <unistd.h>
#include <vector>

#include <fmt/format.h>
#include <fmt/xchar.h>

#include "termctl.h"
#include "xdg.h"

#define ptr_to_u64(ptr) ((__u64)((uintptr_t)(ptr)))

void print_env()
{
  extern char** environ;

  for (char** env = environ; *env != 0; ++env)
  {
    fmt::print("{}\n", *env);
  }
}

long spawn(std::string cmd, std::vector<std::string> args)
{
  int pidfd = -1;
  pid_t parent_tid = -1;

  clone_args cargs = {
    .flags = CLONE_PIDFD | CLONE_PARENT_SETTID,
    .pidfd = ptr_to_u64(&pidfd),
    .parent_tid = ptr_to_u64(&parent_tid),
    .exit_signal = SIGCHLD,
  };
    // .flags = CLONE_CLEAR_SIGHAND | CLONE_NEWCGROUP | CLONE_NEWIPC

  long res = syscall(SYS_clone3, &cargs, sizeof(cargs));
  if (res < 0) {
    std::perror("clone3");
  }
  if (res == 0) {
    char* cstrings[args.size() + 1];

    for (size_t i = 0; i < args.size(); ++i) {
      cstrings[i] = args[i].data();
    }
    cstrings[args.size()] = nullptr;

    int execres = execvpe(cmd.c_str(), cstrings, nullptr);
    if (execres < 0)
    {
      std::perror("execvpe");
    }
  }

  return res;
}

enum struct InputStatus { ok, eof, bad, fail };

std::tuple<std::string, std::vector<std::string>, InputStatus> bsh_read()
{
  std::string line;
  std::string cmd;
  std::vector<std::string> args;
  InputStatus status = InputStatus::ok;

  wint_t c;
  bool keep_reading = true;

  while (keep_reading) {
    c = std::fgetwc(stdin);
    if (std::iswcntrl(c)) {
      fmt::print("{}", c);
      keep_reading = false;
    }
    else {
      fmt::print("{:d} {:c}", c, c);
    }

  }
  return {cmd, args, status};
}

std::tuple<std::string, std::vector<std::string>, InputStatus> bsh_readline()
{
  std::string line;
  std::string cmd;
  std::vector<std::string> args;
  InputStatus status = InputStatus::ok;

  std::getline(std::cin, line);

  if (std::cin.bad()) {
    status = InputStatus::bad;
  }
  else if (std::cin.eof()) {
    status = InputStatus::eof;
  }
  else if (std::cin.fail()) {
    status = InputStatus::fail;
  }

  if (line.size() == 0) {
    return {cmd, args, status};
  }

  std::regex words_regex("[^\\s]+");
  auto words_begin = std::sregex_iterator(line.begin(), line.end(), words_regex);
  auto words_end = std::sregex_iterator();

  for (std::sregex_iterator i = words_begin; i != words_end; ++i)
  {
    std::smatch match = *i;
    std::string match_str = match.str();
    if (cmd.size() == 0) {
      cmd = match_str;
    }
    args.push_back(match_str);
  }

  return {cmd, args, status};
}

int bsh_loop()
{
  bool should_run = true;
  termctl tc;
  xdg xdg;

  xdg.print_env();

  tc.enableRawMode();

  while (should_run) {
    fmt::print("> ");
      // auto [cmd, args, status] = bsh_read();
    auto [cmd, args, status] = bsh_readline();

    if (status == InputStatus::eof) {
      should_run = false;
      continue;
    }

    if (cmd.size() == 0) {
      continue;
    }

    if (cmd == "env") {
      print_env();
      continue;
    }
    
    int cpid = spawn(cmd, args);

    int wstatus;
    pid_t w = waitpid(cpid, &wstatus,
                      WUNTRACED | WCONTINUED);
    do {
      if (w < 0) {
        std::perror("waitpid");
        should_run = false;
      }

    } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
  }

  tc.disableRawMode();

  return 0;
}

void handle_signal(int signal)
{
  fmt::print("Received signal {}\n", signal);
  std::exit(signal);
}

int main(int argc, char** argv)
{
  std::signal(SIGINT, handle_signal);
  return bsh_loop();
}
