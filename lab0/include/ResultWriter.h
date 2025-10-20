#ifndef RESULTWRITER_H
#define RESULTWRITER_H

#include <string>
#include <vector>
#include <fstream>

class ResultWriter {
  std::ofstream outputFile;
  std::string outputFilename;
public:
  explicit ResultWriter(const std::string& outputFilename);
  void writeToFile(const std::vector<std::pair<std::string, int>>& results, int totalWords);
  ~ResultWriter() = default;
};

#endif //RESULTWRITER_H
