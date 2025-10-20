#include "TextProcessor.h"
#include <algorithm>
#include <cctype>

void TextProcessor::processLine(const std::string& line) {
  currentWord.clear();
  for (char c : line) {
    if (std::isalnum(static_cast<unsigned char>(c))) {
      currentWord += std::tolower(static_cast<unsigned char>(c));
    } else {
      addCurrentWord();
    }
  }
  addCurrentWord();
}

std::map<std::string, int> TextProcessor::getResults() const {
  return wordCount;
}

std::vector<std::pair<std::string, int>> TextProcessor::getSortedResults() {
  std::vector<std::pair<std::string, int>> result(wordCount.begin(), wordCount.end());

  std::sort(result.begin(), result.end(),
      [](const auto& a, const auto& b) {
          return a.second > b.second;
      });
  return result;
}

int TextProcessor::getTotalWords() const {
  return totalWords;
}

void TextProcessor::addCurrentWord() {
  if (!currentWord.empty()) {
    wordCount[currentWord]++;
    totalWords++;
    currentWord.clear();
  }
}
