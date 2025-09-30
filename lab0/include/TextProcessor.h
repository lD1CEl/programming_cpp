// TextProcessor.h
#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <map>
#include <string>
#include <vector>

class TextProcessor {
  std::map<std::string, int> wordCount;
  int totalWords = 0;
  std::string currentWord;

public:
  void processLine(const std::string& line);
  std::map<std::string, int> getResults() const;
  std::vector<std::pair<std::string, int>> getSortedResults();
  int getTotalWords() const;

private:
  void addCurrentWord();
};

#endif //TEXTPROCESSOR_H
