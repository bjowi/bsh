#include <cstdio>
#include <cstdint>
#include <cerrno>
#include <iostream>
#include <sched.h>
#include <linux/sched.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <string_view>
#include <sys/syscall.h>
#include <unistd.h>

#include <fmt/format.h>

#define ptr_to_u64(ptr) ((__u64)((uintptr_t)(ptr)))

int run(const std::string& l)
{
  int pidfd = -1;
  pid_t parent_tid = -1;
  pid_t pid = -1;

  clone_args args = {
    .flags = CLONE_PIDFD | CLONE_PARENT_SETTID,
    .pidfd = ptr_to_u64(&pidfd),
    .parent_tid = ptr_to_u64(&parent_tid),
    .exit_signal = SIGCHLD,
  };
    // .flags = CLONE_CLEAR_SIGHAND | CLONE_NEWCGROUP | CLONE_NEWIPC

  long res = syscall(SYS_clone3, &args, sizeof(args));
  if (res < 0) {
    std::perror("log(-1) failed");
  }
  if (res == 0) {
    fmt::print("child ok {}\n", l);
    int execres = execvpe(l.c_str(), nullptr, nullptr);
    if (execres < 0)
    {
      std::perror("execvpe failed");
    }
  }
  if (res > 0) {
    fmt::print("got pid {}\n", res);
  }

  return res;
}

const std::string bsh_read_line()
{
  std::string line;
  std::getline(std::cin, line);

  return line;
}

int bsh_loop() 
{
  fmt::print("> ");
  std::string l = bsh_read_line();
  fmt::print("{}\n", l);
  return run(l);
}

int main(int argc, char **argv)
{
  return bsh_loop();
}
