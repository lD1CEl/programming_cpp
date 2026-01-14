#include "GameView.h"
#include "../Model/Player.h"
#include "../Model/Monster.h"
#include "../Common.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

GameView::GameView() : musicLoaded(false), fontLoaded(false) {
  
  window.create(sf::VideoMode({
    (unsigned int)(GameState::MapWidth * TILE_SIZE * SCREEN_SCALE),
    (unsigned int)(GameState::MapHeight * TILE_SIZE * SCREEN_SCALE)
  }), "Digger C++ SFML");

  window.setFramerateLimit(60);

  LoadResources();
}

void GameView::LoadResources() {
  std::vector<std::string> musicCandidates;
  musicCandidates.push_back("Digger.mp3");
  
  #ifdef _WIN32
  char exePathBuf[MAX_PATH];
  if (GetModuleFileNameA(NULL, exePathBuf, MAX_PATH) != 0) {
    std::filesystem::path exeDir = std::filesystem::path(exePathBuf).parent_path();
    musicCandidates.push_back((exeDir / "Digger.mp3").string());
  }
  #endif
  
  // Try relative path from source root or execution dir
  musicCandidates.push_back("../Digger.mp3");

  for (const auto& path : musicCandidates) {
    if (music.openFromFile(path)) {
      musicLoaded = true;
      std::cout << "Music loaded from: " << path << std::endl;
      break;
    }
  }

  if (musicLoaded) {
    music.setLoop(true);
    music.setVolume(50.0f);
    music.play();
  } else {
    std::cerr << "Error: Could not load music Digger.mp3 from any candidate path." << std::endl;
  }

  // --- FONT ---
  #ifdef _WIN32
  std::filesystem::path exeDir;
  char buf[MAX_PATH];
  if (GetModuleFileNameA(NULL, buf, MAX_PATH) != 0) exeDir = std::filesystem::path(buf).parent_path();
  std::string customFontPath = (exeDir / "fonts" / "LCD16x2Display-Regular.otf").string();
  #else
  std::string customFontPath = "fonts/LCD16x2Display-Regular.otf";
  #endif

  if (font.loadFromFile(customFontPath)) {
    fontLoaded = true;
  } else if (font.loadFromFile("arial.ttf")) {
    fontLoaded = true;
  }

  if (fontLoaded) {
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(sf::Vector2f(10.0f, 10.0f));
  }

  // --- SCREENS ---
  winScreen.setTexture(texManager.Get("images/win.png"));
  lostScreen.setTexture(texManager.Get("images/lost.png"));

  if (winScreen.getTexture() && lostScreen.getTexture())
  {
    float screenScaleX = (float)window.getSize().x / winScreen.getTexture()->getSize().x;
    float screenScaleY = (float)window.getSize().y / winScreen.getTexture()->getSize().y;

    winScreen.setScale(screenScaleX, screenScaleY);
    lostScreen.setScale(screenScaleX, screenScaleY);
  }
}

bool GameView::IsOpen() const {
  return window.isOpen();
}

bool GameView::PollEvent(sf::Event& event) {
  return window.pollEvent(event);
}

void GameView::Close() {
  window.close();
}

void GameView::Clear() {
  window.clear(sf::Color::Black);
}

void GameView::Display() {
  window.display();
}

void GameView::PlayMusic() {
  if (musicLoaded && (music.getStatus() == sf::Music::Status::Paused || music.getStatus() == sf::Music::Status::Stopped)) {
    music.play();
  }
}

void GameView::PauseMusic() {
  if (musicLoaded) {
    music.pause();
  }
}

void GameView::Render() {
  if (!GameState::IsOver) {
    DrawMap();
    DrawUI();
  } else {
    DrawScreens();
  }
}

void GameView::DrawMap() {
  struct DrawItem {
    int x, y;
    ObjectPtr c;
  };
  std::vector<DrawItem> items;

  for (int x = 0; x < GameState::MapWidth; x++) {
    for (int y = 0; y < GameState::MapHeight; y++) {
      if (GameState::Map[x][y]) {
        items.push_back({x, y, GameState::Map[x][y]});
      }
    }
  }

  std::sort(items.begin(), items.end(), [](const DrawItem& a, const DrawItem& b) {
      return a.c->GetDrawingPriority() < b.c->GetDrawingPriority();
  });

  for (const auto& item : items) {
    sf::Sprite sprite(texManager.Get(item.c->GetImageFileName()));

    float rotation = 0.0f;
    float scaleX = SCREEN_SCALE;

    bool isPlayer = std::dynamic_pointer_cast<Player>(item.c) != nullptr;
    bool isDiggerMonster = std::dynamic_pointer_cast<MonsterDigger>(item.c) != nullptr;

    if (isPlayer || isDiggerMonster) {

      int directionX = GameState::PlayerDirectionX;

      if (isPlayer) {
        if (GameState::PlayerDirectionX == 1 && GameState::PlayerDirectionY == 0) { rotation = 0.0f; scaleX = SCREEN_SCALE; }
        else if (GameState::PlayerDirectionX == -1 && GameState::PlayerDirectionY == 0) { rotation = 0.0f; scaleX = -SCREEN_SCALE; }
        else if (GameState::PlayerDirectionX == 0 && GameState::PlayerDirectionY == -1) { rotation = 270.0f; scaleX = SCREEN_SCALE; }
        else if (GameState::PlayerDirectionX == 0 && GameState::PlayerDirectionY == 1) { rotation = 90.0f; scaleX = SCREEN_SCALE; }
      } else if (isDiggerMonster) {
        if (directionX < 0) {
          scaleX = -SCREEN_SCALE;
        } else {
          scaleX = SCREEN_SCALE;
        }
        rotation = 0.0f;
      }


      sprite.setOrigin(sf::Vector2f(TILE_SIZE / 2.0f, TILE_SIZE / 2.0f));
      sprite.setRotation(rotation);

      sprite.setPosition(sf::Vector2f(
        item.x * TILE_SIZE * SCREEN_SCALE + TILE_SIZE / 2.0f * SCREEN_SCALE,
        item.y * TILE_SIZE * SCREEN_SCALE + TILE_SIZE / 2.0f * SCREEN_SCALE
      ));
    } else {
      sprite.setPosition(sf::Vector2f(
        item.x * TILE_SIZE * SCREEN_SCALE,
        item.y * TILE_SIZE * SCREEN_SCALE
      ));
    }

    sprite.setScale(sf::Vector2f(scaleX, SCREEN_SCALE));
    window.draw(sprite);
  }
}

void GameView::DrawUI() {
  if (fontLoaded) {
    scoreText.setString("Score: " + std::to_string(GameState::Scores));
    window.draw(scoreText);
  }
}

void GameView::DrawScreens() {
  if (GameState::IsWin) {
    winScreen.setPosition(window.getSize().x / 2.0f - winScreen.getGlobalBounds().width / 2.0f,
                window.getSize().y / 2.0f - winScreen.getGlobalBounds().height / 2.0f);
    window.draw(winScreen);
  } else if (GameState::IsLost) {
    lostScreen.setPosition(window.getSize().x / 2.0f - lostScreen.getGlobalBounds().width / 2.0f,
                window.getSize().y / 2.0f - lostScreen.getGlobalBounds().height / 2.0f);
    window.draw(lostScreen);
  }
}
