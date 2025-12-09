#include "TextureManager.h"
#include "../Common.h"
#include <iostream>

sf::Texture& TextureManager::Get(const std::string& filename) {
  if (textures.find(filename) == textures.end()) {
    sf::Texture t;

    auto tryLoadFromPath = [&](const std::filesystem::path& p)->bool {
      try {
        if (!std::filesystem::exists(p) || !std::filesystem::is_regular_file(p)) return false;

        if (t.loadFromFile(p.string())) {
          return true;
        }

        std::ifstream fin(p, std::ios::binary);
        if (!fin) return false;
        std::vector<char> buf((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        if (buf.empty()) return false;
        return t.loadFromMemory(buf.data(), buf.size());
      } catch(...) {
        return false;
      }
    };

    std::filesystem::path exeDir;
    #ifdef _WIN32
    char buf[MAX_PATH];
    if (GetModuleFileNameA(NULL, buf, MAX_PATH) != 0) exeDir = std::filesystem::path(buf).parent_path();
    #endif

    std::vector<std::filesystem::path> candidates;
    std::filesystem::path given(filename);

    if (!exeDir.empty()) candidates.push_back(exeDir / given);
    candidates.push_back(std::filesystem::current_path() / given);

    bool loaded = false;
    for (const auto &c : candidates) {
      if (tryLoadFromPath(c)) { loaded = true; break; }
    }

    if (!loaded) {
      std::uint8_t r=255, g=255, b=255;
      if(filename.find("Terrain") != std::string::npos) { r=150; g=100; b=50; }
      else if(filename.find("Digger") != std::string::npos) { r=0; g=0; b=255; }
      else if(filename.find("Gold") != std::string::npos) { r=255; g=215; b=0; }
      else if(filename.find("Sack") != std::string::npos) { r=100; g=100; b=100; }
      else if(filename.find("Monster.png") != std::string::npos) { r=255; g=0; b=0; }
      else if(filename.find("fire") != std::string::npos) { r=255; g=165; b=0; }
      else if(filename.find("monster_digger") != std::string::npos) { r=100; g=0; b=150; }
      else if(filename.find("win") != std::string::npos) { r=0; g=255; b=0; }
      else if(filename.find("lost") != std::string::npos) { r=255; g=0; b=0; }

      sf::Image img;
      img.create(TILE_SIZE, TILE_SIZE, sf::Color(r, g, b));

      sf::Texture tempTex;
      if (tempTex.loadFromImage(img)) {
        t = tempTex;
      } else {
        t.create(TILE_SIZE, TILE_SIZE);
      }
    }

    textures[filename] = t;
  }
  return textures[filename];
}
