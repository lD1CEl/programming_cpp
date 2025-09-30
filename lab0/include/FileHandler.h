#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <fstream>

class FileHandler {
  std::string inputFilename;
  std::ifstream inputFile;
  bool isFileOpen{};

public:
  explicit FileHandler(const std::string& inputFilename);
  bool openFile();
  std::string readLine();
  ~FileHandler() = default;
};

#endif //FILEHANDLER_H
