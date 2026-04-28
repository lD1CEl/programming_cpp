#ifndef LABGAME_SRC_VIEW_TEXTUREMANAGER_H_
#define LABGAME_SRC_VIEW_TEXTUREMANAGER_H_
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <filesystem>
#include <vector>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

class TextureManager {
private:
  std::map<std::string, sf::Texture> textures;

public:
  sf::Texture& Get(const std::string& filename);
};

#endif  // LABGAME_SRC_VIEW_TEXTUREMANAGER_H_
