#include "WordCounter.h"
#include <iostream>
#include <cstdlib>

WordCounter::WordCounter(const std::string& inputFile, const std::string& outputFile)
   : fileHandler(inputFile), resultWriter(outputFile) {}

void WordCounter::run() {
  if (!fileHandler.openFile()) {
    std::cerr << "FileHandler::openFile() failed." << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string line;
  while (!(line = fileHandler.readLine()).empty()) {
    texProcessor.processLine(line);
  }

  auto results = texProcessor.getSortedResults();
  int totalWords = texProcessor.getTotalWords();

  resultWriter.writeToFile(results, totalWords);
}
