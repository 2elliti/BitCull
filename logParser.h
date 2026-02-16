#ifndef LOG_PARSER_H
#define LOG_PARSER_H

#include <cstddef>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

class LogParser{
  public:
  static std::optional<double> extractSsimScore(const std::string& ffmpeg_output){
    std::optional<double> out = std::nullopt;
    std::stringstream ss(ffmpeg_output);
    std::string token;

    while(std::getline(ss,token)){
      auto ssimPos = token.find("SSIM");
      if(ssimPos != std::string::npos){
        size_t start = token.find("All");
        if(start == std::string::npos) continue;
        std::string score = token.substr(start);
        
        auto poscolon = score.find_first_of(':');
        auto firstWspace = score.find_first_of(' ');
        
        std::string doubleStr = score.substr(poscolon + 1, firstWspace - poscolon - 1);

        out = std::stod(doubleStr);

      }
    }
    return out;
  }
};


#endif
