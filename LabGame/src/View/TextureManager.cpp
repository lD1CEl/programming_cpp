#include "TextureManager.h"
#include "../Common.h"

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
      std::uint8_t r = 255, g = 255, b = 255;

      struct ColorMapping {
          const char* key;
          std::uint8_t r;
          std::uint8_t g;
          std::uint8_t b;
      };

      const ColorMapping mappings[] = {
          {"Terrain", 150, 100, 50},
          {"Digger", 0, 0, 255},
          {"Gold", 255, 215, 0},
          {"Sack", 100, 100, 100},
          {"Monster.png", 255, 0, 0},
          {"fire", 255, 165, 0},
          {"monster_digger", 100, 0, 150},
          {"win", 0, 255, 0},
          {"lost", 255, 0, 0}
      };

      for (const auto& mapping : mappings) {
          if (filename.find(mapping.key) != std::string::npos) {
              r = mapping.r;
              g = mapping.g;
              b = mapping.b;
              break;
          }
      }

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
