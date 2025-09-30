#include <iostream>
#include <cstdlib>
#include "WordCounter.h"

int main(int argNum, char* arg[]) {
  if (argNum != 3) {
    std::cerr << "Usage: ./word_count inputFile outputFile" << std::endl;
    return EXIT_FAILURE;
  }

  WordCounter counter(arg[1], arg[2]);
  counter.run();

  return 0;
}
