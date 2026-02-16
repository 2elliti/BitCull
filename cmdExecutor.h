#ifndef CMD_EXECUTOR_H
#define CMD_EXECUTOR_H

#include <array>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <string_view>

class CmdExecutor {
  public:
  static std::string execute(std::string_view cmd) {
#ifdef _WIN32
    FILE *pipe = _popen(cmd.c_str(), "r");
#else
    FILE *pipe = popen(cmd.data(), "r");
#endif

    std::array<char, 128> buffer;
    std::string result;
    if (!pipe) {
      throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe)) {
      result += buffer.data();
    }
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    return result;
  }
};

#endif
