// WordCounter.h
#ifndef WORDCOUNTER_H
#define WORDCOUNTER_H

#include "FileHandler.h"
#include "TextProcessor.h"
#include "ResultWriter.h"
#include <string>

class WordCounter {
  FileHandler fileHandler;
  TextProcessor texProcessor;
  ResultWriter resultWriter;

  public:
    WordCounter(const std::string& inputFile, const std::string& outputFile);
    void run();
};

#endif //WORDCOUNTER_H
