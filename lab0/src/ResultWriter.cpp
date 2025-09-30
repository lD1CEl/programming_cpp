#include "ResultWriter.h"

ResultWriter::ResultWriter(const std::string& outputFilename) {
  this->outputFilename = outputFilename;
}

void ResultWriter::writeToFile(const std::vector<std::pair<std::string, int>>& results, int totalWords) {
  outputFile.open(this->outputFilename);

  for (const auto& [word, count] : results) {
    double pers = (count * 100.0) / totalWords;
    outputFile << word << "," << count << "," << pers << "%\n";
  }
}
