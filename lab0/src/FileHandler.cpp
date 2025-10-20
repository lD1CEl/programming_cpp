// FileHandler.cpp
#include "FileHandler.h"

FileHandler::FileHandler(const std::string& inputFilename) {
  this->inputFilename = inputFilename;
}

bool FileHandler::openFile() {
  inputFile.open(inputFilename);
  isFileOpen = inputFile.is_open();
  return isFileOpen;
}

std::string FileHandler::readLine() {
  if (!isFileOpen) {
    return "";
  }

  if (std::string line; std::getline(inputFile, line)) {
    return line;
  }
  return "";
}
