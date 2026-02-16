#ifndef OPTIMIZER_H
#define OPTIMIZER_H
#include <cstdint>
#include <iostream>
#include <optional>
#include <string_view>

#include "cmdExecutor.h"
#include "logParser.h"

struct OptimizationResults {
  int optimalCrf;          // The magic number we found.
  double vmafScore;        // Score we achived.
  uint32_t iterations = 0; // Iteration took to find target vmafScore.
};

class VideoOptimizer {
  double targetVmaf_;
  int minCrfLimit_;
  int maxCrfLimit_;
  int offset_;
  std::string inputFile_ = "movingCans.mp4";

public:
  VideoOptimizer(double targetVmaf = 0.98, int minCrfLimit = 18,
                 int maxCrfLimit = 35)
      : targetVmaf_(targetVmaf), minCrfLimit_(minCrfLimit),
        maxCrfLimit_(maxCrfLimit), offset_(minCrfLimit_) {}

  // NOTE:
  // Will do continous qunantization, till we find target vmaf score and under
  // max crf limits. Binary search...
  std::optional<OptimizationResults> findOptimalCrf() {
    int low = 0;
    int high = maxCrfLimit_ - minCrfLimit_;
    double achivedVmaf = 0.0;
    uint32_t iteration = 0;
    int currentCrf = 0;
    std::optional<OptimizationResults> bestResults = std::nullopt;

    while (low <= high) {
      iteration++;
      int middle = (low + high) / 2;
      currentCrf = middle + offset_;

      achivedVmaf = getVmafScore(currentCrf);
      if (achivedVmaf >= targetVmaf_) {
        bestResults = OptimizationResults{currentCrf, achivedVmaf, iteration};
        low = middle + 1;
      } else {
        high = middle - 1;
      }
    }
    return bestResults;
  }

  void setInputFile(std::string_view input) { inputFile_ = std::string(input); }

  bool saveOptimizedVideo(int crf, std::string output_path) {
    std::string cmd = "ffmpeg -i " + inputFile_ + " -c:v libx264 -crf " +
                      std::to_string(crf) +
                      " -preset medium " +
                                           
                      output_path + " -y"; 

    std::cout << "[System] Saving Final Video to " << output_path << "...\n";
    std::string output = CmdExecutor::execute(cmd);
    return true;
  }

private:
  double getVmafScore(int crf) {
    std::string cmd = "ffmpeg -i " + inputFile_ + " -c:v libx264 -crf " +
                      std::to_string(crf) +
                      " -preset ultrafast -f matroska - | "
                      "ffmpeg -i " +
                      inputFile_ +
                      " -i - "
                      "-lavfi \"[0:v][1:v]ssim\" -f null - 2>&1";

    std::cout << "   [System] Probing CRF " << crf << "...\n";

    std::string output = CmdExecutor::execute(cmd);

    auto score_opt = LogParser::extractSsimScore(output);

    if (score_opt.has_value()) {
      auto score = score_opt.value();
      std::cout << "   [System] Found Score: " << score
                << " (Target: " << targetVmaf_ << ")\n";

      return score;
    } else {
      std::cerr << "   [Error] FFmpeg failed to report SSIM. Command output:\n"
                << output << "\n";
      return 0.0;
    }
  }
};

#endif
