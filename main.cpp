#include "optimizer.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string_view>


bool fileExist(std::string_view file){
  return std::filesystem::exists(file);
}


int main(int argc, char **argv) {

  if(argc < 3){
    std::cerr << "Usage: ./bitcull input.mp4 output.mp4\n";
    exit(1);
  }

  std::string_view inputFile = argv[1];
  std::string_view outputFile = argv[2];

  // Checks if Input file exists.
  if(!fileExist(inputFile)){
    std::cerr << "[FAILURE] Input file Doesnt exists\n";
    exit(1);
  }

  std::cout << "--- BitCull unit testing ---\n";

  VideoOptimizer optimizer(0.98, 18, 35);
  optimizer.setInputFile(inputFile);

  auto result = optimizer.findOptimalCrf();

  if (result) {
    std::cout << "  > Optimal CRF: " << result->optimalCrf << "\n";
    std::cout << "  > VMAF Score:  " << result->vmafScore << "\n";
    std::cout << "  > Iterations:  " << result->iterations << "\n";
    optimizer.saveOptimizedVideo(result->optimalCrf, std::string(outputFile));
  } else {
    std::cout << "\n[FAILURE] Could not find a valid CRF configuration.\n";
  }


  try {
    std::cout << "Testing Command Executor...\n";
    std::string output = CmdExecutor::execute("ffprobe -version");
    std::cout << "Output received:\n" << output << "\n";
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
  }

  return 0;
}
