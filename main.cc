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

#define ptr_to_u64(ptr) ((__u64)((uintptr_t)(ptr)))

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
    std::perror("");
  }
  if (res == 0) {
    fmt::print("child ok {} {}", cmd, fmt::join(args, " "));
      // std::for_each(l.begin(), l.end(), [l.size()](const std::string& s) 
      // {

      // std::vector<char*> cstrings;
    char* cstrings[args.size() + 1];
      // cstrings.reserve(args.size());

    for (size_t i = 0; i < args.size(); ++i) {
      cstrings[i] = args[i].data();
    }
    cstrings[args.size()] = nullptr;

    for (size_t i = 0; i < args.size(); ++i) {
      printf("%s ", cstrings[i]);
    }
      // int execres = execvpe(cmd.c_str(), cstrings, nullptr);
    int execres = execvp(cmd.c_str(), cstrings);
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

std::tuple<std::string, std::vector<std::string>> bsh_read_line()
{
  std::string line;
  std::getline(std::cin, line);

  std::regex words_regex("[^\\s]+");
  auto words_begin = std::sregex_iterator(line.begin(), line.end(), words_regex);
  auto words_end = std::sregex_iterator();

  std::string cmd;
  std::vector<std::string> args;

  for (std::sregex_iterator i = words_begin; i != words_end; ++i)
  {
    std::smatch match = *i;
    std::string match_str = match.str();
    if (cmd.size() == 0) {
      cmd = match_str;
    }

    std::cout << match_str << '\n';
    args.push_back(match_str);
  }

  return {cmd, args};
}

// std::vector<std::string> split(const std::string_view line)
// {
//     using std::operator""sv;
//     constexpr std::string_view delim{" "sv};
//     std::vector<std::string> x;

//     for (const auto word : std::views::split(line, delim)) {
//         // with string_view's C++23 range constructor:
//       x.push_back(word);
//         //std::cout << std::quoted(std::string_view(word)) << ' ';
//     }

//     std::cout << '\n';
//     return x;
// }

int bsh_loop() 
{
  bool should_run = true;

  while (should_run) {
    fmt::print("> ");
    auto [cmd, args] = bsh_read_line();
    fmt::print("cmd: {} {}\n", cmd, fmt::join(args, " "));
      // for (const auto& word : split(l)) {
      //   fmt::print("{}\n", word);
      // }

    int cpid = spawn(cmd, args);
    fmt::print("st {}\n", cpid);
    int wstatus;
    pid_t w = waitpid(cpid, &wstatus,
                      WUNTRACED | WCONTINUED);
    fmt::print("w {}\n", w);
  }
  return 0;
}

void handle_spawn_done(int signal) 
{exit(signal);}

int main(int argc, char **argv)
{
  std::signal(SIGINT, handle_spawn_done);
  return bsh_loop();
}
